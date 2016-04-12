#include "video.h"


Video::Video(const char* filename, int reqWidth, int reqHeight)
{
    static bool ls_first = true;
    if (ls_first)
    {
        av_register_all();
        ls_first = false;
    }
    _reqWidth = reqWidth;
    _reqHeight = reqHeight;

    _videoStreamIndex = -1;
    _audioStreamIndex = -1;

    _videoQueue = new AVPacketQueue();
    _audioQueue = new AVPacketQueue();

    _started = false;

    Load(filename);
}

void Video::Load(const char* filename)
{
    int i, error;
    
    error = av_open_input_file(&_formatCtx, filename, NULL, 0, NULL);
    if (error) throw new FileNotFoundException("File \"%s\" not found", filename);

    error = av_find_stream_info(_formatCtx);
    if (error < 0) throw new AVStreamException("Could not find stream information");

    // Find the video and audio stream
    for (i=0; i<(int)_formatCtx->nb_streams; i++)
    {
        switch (_formatCtx->streams[i]->codec->codec_type)
        {
        case AVMEDIA_TYPE_VIDEO:
            if (_videoStreamIndex == -1) _videoStreamIndex = i;
            break;
        case AVMEDIA_TYPE_AUDIO:
            if (_audioStreamIndex == -1) _audioStreamIndex = i;
            break;
        }
    }

    // We need both
    if (_videoStreamIndex < 0) throw new AVStreamException("No input video stream found");
    if (_audioStreamIndex < 0) throw new AVStreamException("No input audio stream found");

    // Get the video codec and set up memory management
    _videoCodecCtx = _formatCtx->streams[_videoStreamIndex]->codec;
    _videoCodec = avcodec_find_decoder(_videoCodecCtx->codec_id);
    if (!_videoCodec) throw new AVStreamException("Could not find video decoder \"%s\"", _videoCodecCtx->codec_name);
    _videoCodecCtx->get_buffer = Video::GetBuffer;
    _videoCodecCtx->release_buffer = Video::ReleaseBuffer;

    // Get the audio codec
    _audioCodecCtx = _formatCtx->streams[_audioStreamIndex]->codec;
    _audioCodec = avcodec_find_decoder(_audioCodecCtx->codec_id);
    if (!_audioCodec) throw new AVStreamException("Could not find audio decoder \"%s\"", _audioCodecCtx->codec_name);

    // Try to open the codecs
    error = avcodec_open(_videoCodecCtx, _videoCodec);
    if (error < 0) throw new AVStreamException("Could not open video decoder \"%s\"", _videoCodecCtx->codec_name);

    error = avcodec_open(_audioCodecCtx, _audioCodec);
    if (error < 0) throw new AVStreamException("Could not open audio decoder \"%s\"", _audioCodecCtx->codec_name);

    // Set the software scaling context for converting raw frames to the texture buffer
    _swsCtx = sws_getContext(
        _videoCodecCtx->width, _videoCodecCtx->height, _videoCodecCtx->pix_fmt,
        _reqWidth, _reqHeight, PIX_FMT_RGB24,
        SWS_BICUBIC, NULL, NULL, NULL);

    int size = avpicture_get_size(PIX_FMT_RGB24, _reqWidth, _reqHeight);
    _currentFrame = avcodec_alloc_frame();
    _currentBuffer = (uint8_t*) av_malloc(size * sizeof(uint8_t));
    avpicture_fill((AVPicture*)_currentFrame, _currentBuffer, PIX_FMT_RGB24, _reqWidth, _reqHeight);

    // Create the wave out interface for the audio
    _waveout = new WaveOut(this, _audioCodecCtx->sample_rate, _audioCodecCtx->channels, 16);
}

void Video::Start()
{
    // Started will be set to true the first time an audio buffer is retrieved
    _started = false;
    CreateThread(NULL, 0, AVStreamProc, this, NULL, NULL);
}

const static int SCALE = 1;

int Video::NextFrame(void* buffer)
{
    int frameDone = 0;
    int result = 0;
    double pts = 0;

    if (!_started) return 0;

    AVFrame* frame = avcodec_alloc_frame();
    AVPacket* packet;

    // Get the number of milliseconds passed and see if we should display a new frame
    double elapsedMs = SCALE * (1000.0 / CLOCKS_PER_SEC) * clock() - _baseTimeMs;

    //char buff[256];
    //sprintf(buff, "ms %f frame pts %f current pts %f\n", elapsedMs, _lastDisplayedMs, _currentFrameMs);
    //OutputDebugString(buff);

    if (elapsedMs >= _currentFrameMs)
    {
        // If this is not the current frame, copy it to the buffer
        if (_lastDisplayedMs != _currentFrameMs)
        {
            _lastDisplayedMs = _currentFrameMs;
            memcpy(buffer, _currentBuffer, 3 * _reqWidth * _reqHeight);
            result = 1;
        }
	
        // Try to load a new frame from the video packet queue
        while (!frameDone && (packet = _videoQueue->Dequeue()) != NULL)
        {
            if (packet == (AVPacket*)-1) return -1;

            _s_pts = packet->pts;
            int err = avcodec_decode_video2(_videoCodecCtx, frame, &frameDone, packet);
            av_free_packet(packet);

            if (packet->dts == AV_NOPTS_VALUE)
            {
	        if (frame->opaque && *(uint64_t*)frame->opaque != AV_NOPTS_VALUE) pts = (double) *(uint64_t*)frame->opaque;
	        else pts = 0;
            }
            else pts = packet->dts * av_q2d(_videoCodecCtx->time_base);
        }
        if (frameDone)
        {
            // if a frame was loaded scale it to the current texture frame buffer, but also set the pts so that it won't be copied to the texture until it's time
            sws_scale(_swsCtx, frame->data, frame->linesize, 0, _videoCodecCtx->height, _currentFrame->data, _currentFrame->linesize);
            _currentFrameMs = pts;
        }
        av_free(frame);
    }
    return result;
}

int Video::NextAudioBuffer(void** buffer, int* len, uint64_t bufferSize)
{
    int bufferDone = 0;
    AVPacket* packet;
    static int16_t* audioBuffer = (int16_t*) av_malloc(AV_AUDIO_BUFFER_SIZE + FF_INPUT_BUFFER_PADDING_SIZE);

    *len = 0;
    *buffer = NULL;

    // Try to get a new audio buffer from the audio packet queue
    while ((packet = _audioQueue->Dequeue()) != NULL)
    {
        if (packet == (AVPacket*)-1) return -1;

        int bufferSize = AVCODEC_MAX_AUDIO_FRAME_SIZE;
        int consumed = avcodec_decode_audio3(_audioCodecCtx, audioBuffer, &bufferSize, packet);
        if (consumed > 0 && bufferSize > 0)        
        {
            bufferDone = 1;
            *len = bufferSize;
            *buffer = malloc(bufferSize);
            memcpy(*buffer, audioBuffer, bufferSize);
            break;
        }
        av_free_packet(packet);
    }

    if (!_started)
    {
        // First time we get an audio buffer, set up our params
        _started = true;
        _baseTimeMs = (1000.0 / CLOCKS_PER_SEC) * clock();
        _currentFrameMs = 0;
        _lastDisplayedMs = 0;
        _audioSamplesPlayed = 0;
    }
    else if (bufferDone)
    {
        // See how much time should have elapsed based on how many audio samples were played up to this point
        // Then check it against our own clock
        _audioSamplesPlayed += bufferSize / (_audioCodecCtx->channels * sizeof(short));
        double elapsedMs = SCALE * (1000.0 / CLOCKS_PER_SEC) * clock() - _baseTimeMs;
        double samplesElapsedMs = (1000.0 * _audioSamplesPlayed) / _audioCodecCtx->sample_rate;
        
        // If our clock is out of sync with the audio we add or subtract some time from our base time
        // this will have its effect on the GetNextFrame function.
        // Adjust by at most 10% per frame
        _baseTimeMs += (elapsedMs - samplesElapsedMs) / 10;
    }
    
    return bufferDone;
}

DWORD WINAPI Video::AVStreamProc(LPVOID data)
{
    Video* instance = (Video*) data;
    instance->_waveout->Start();
    
    AVPacket packet;

    const int max_qsize = 10000;
    while (av_read_frame(instance->_formatCtx, &packet) >= 0)
    {
        // Keep reading packets and split them into queues
        AVPacketQueue* queue = 0;
        if (packet.stream_index == instance->_videoStreamIndex)
        {
            queue = instance->_videoQueue;
        }
        else if (packet.stream_index == instance->_audioStreamIndex)
        {
            queue = instance->_audioQueue;
        }
        if (queue)
        {
            queue->Enqueue(&packet);
            auto cnt = queue->Count();
#if 0
            static int pcnt = 0;
            char buff[256];
            sprintf(buff, "Queuing %d @%lld\n", pcnt++, cnt);
            OutputDebugString(buff);
            // Let's not burn our CPU, this will be sufficient to buffer a nice number of packets in the queue, if your CPU is FAST enough
            while (cnt > max_qsize)
            {
                char buff[256];
                sprintf(buff, "Sleep for count %lld\n", cnt);
                OutputDebugString(buff);
                Sleep(10);
                cnt = queue->Count();
            }
#else
            ::Sleep(1);
#endif
        }
    }

    //Add NULL packets to the queues which will signal end of stream to the application when dequeued
    instance->_videoQueue->Enqueue(NULL);
    instance->_audioQueue->Enqueue(NULL);
    return 0;
}

uint64_t Video::_s_pts = 0;

int Video::GetBuffer(AVCodecContext* c, AVFrame* pic) {
  int ret = avcodec_default_get_buffer(c, pic);
  uint64_t *pts = (uint64_t*) av_malloc(sizeof(uint64_t));
  *pts = _s_pts;
  pic->opaque = pts;
  return ret;
}
void Video::ReleaseBuffer(AVCodecContext* c, AVFrame* pic) {
  if(pic) av_freep(&pic->opaque);
  avcodec_default_release_buffer(c, pic);
}


