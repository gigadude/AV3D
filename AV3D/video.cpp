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

    if (_videoStreamIndex < 0) throw new AVStreamException("No input video stream found");
    if (_audioStreamIndex < 0) throw new AVStreamException("No input audio stream found");

    _videoCodecCtx = _formatCtx->streams[_videoStreamIndex]->codec;
    _videoCodec = avcodec_find_decoder(_videoCodecCtx->codec_id);
    if (!_videoCodec) throw new AVStreamException("Could not find video decoder \"%s\"", _videoCodecCtx->codec_name);
	_videoCodecCtx->get_buffer = Video::GetBuffer;
	_videoCodecCtx->release_buffer = Video::ReleaseBuffer;

    _audioCodecCtx = _formatCtx->streams[_audioStreamIndex]->codec;
    _audioCodec = avcodec_find_decoder(_audioCodecCtx->codec_id);
    if (!_audioCodec) throw new AVStreamException("Could not find audio decoder \"%s\"", _audioCodecCtx->codec_name);

    error = avcodec_open(_videoCodecCtx, _videoCodec);
    if (error < 0) throw new AVStreamException("Could not open video decoder \"%s\"", _videoCodecCtx->codec_name);

    error = avcodec_open(_audioCodecCtx, _audioCodec);
    if (error < 0) throw new AVStreamException("Could not open audio decoder \"%s\"", _audioCodecCtx->codec_name);

    _swsCtx = sws_getContext(
        _videoCodecCtx->width, _videoCodecCtx->height, _videoCodecCtx->pix_fmt,
        _reqWidth, _reqHeight, PIX_FMT_RGB24,
        SWS_BICUBIC, NULL, NULL, NULL);

    int size = avpicture_get_size(PIX_FMT_RGB24, _reqWidth, _reqHeight);
    _currentFrame = avcodec_alloc_frame();
    _currentBuffer = (uint8_t*) av_malloc(size * sizeof(uint8_t));
    avpicture_fill((AVPicture*)_currentFrame, _currentBuffer, PIX_FMT_RGB24, _reqWidth, _reqHeight);

    _waveout = new WaveOut(this, _audioCodecCtx->sample_rate, _audioCodecCtx->channels, 16);
}

void Video::Start()
{
    _started = false;
    CreateThread(NULL, 0, AVStreamProc, this, NULL, NULL);
}

int Video::NextFrame(void* buffer)
{
    int frameDone = 0;
	int result = 0;
	double pts = 0;

    if (!_started) return 0;

    AVFrame* frame = avcodec_alloc_frame();
    AVPacket* packet;

    uint64_t msPassed = (1000 * (clock() - _startTime)) / CLOCKS_PER_SEC;
    if (msPassed >= _currentPts)
	{
		if (_currentFramePts != _currentPts)
		{
            _currentFramePts = _currentPts;
			memcpy(buffer, _currentBuffer, 3 * _reqWidth * _reqHeight);
			result = 1;
		}
	
		while (!frameDone && (packet = _videoQueue->Dequeue()) != NULL)
		{
			if (packet == (AVPacket*)-1) return -1;

			_s_pts = packet->pts;
			avcodec_decode_video2(_videoCodecCtx, frame, &frameDone, packet);
			av_free_packet(packet);

			if (packet->dts == AV_NOPTS_VALUE)
			{
				if (frame->opaque && *(uint64_t*)frame->opaque != AV_NOPTS_VALUE) pts = (double) *(uint64_t*)frame->opaque;
				else pts = 0;
			}
			else pts = (double) packet->dts;
            
			pts *= av_q2d(_videoCodecCtx->time_base);
		}
		if (frameDone)
		{
			sws_scale(_swsCtx, frame->data, frame->linesize, 0, _videoCodecCtx->height, _currentFrame->data, _currentFrame->linesize);
			_currentPts = (uint64_t) (pts * 1000);
		}
		av_free(frame);
	}
	return result;
}

int Video::NextAudioBuffer(void** buffer, int* len)
{
    int bufferDone = 0;
    AVPacket* packet;
    static int16_t* audioBuffer = (int16_t*) av_malloc(AV_AUDIO_BUFFER_SIZE + FF_INPUT_BUFFER_PADDING_SIZE);

    *len = 0;
    *buffer = NULL;

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
        _started = true;
        _startTime = clock();
        _currentFramePts = 0;
        _currentPts = 0;
    }
    return bufferDone;
}

DWORD WINAPI Video::AVStreamProc(LPVOID data)
{
    Video* instance = (Video*) data;
    instance->_waveout->Start();
    
    AVPacket packet;
    
    while (av_read_frame(instance->_formatCtx, &packet) >= 0)
    {
        AVPacketQueue* queue = 0;
        if (packet.stream_index == instance->_videoStreamIndex) queue = instance->_videoQueue;
        else if (packet.stream_index == instance->_audioStreamIndex) queue = instance->_audioQueue;
        if (queue) queue->Enqueue(&packet);
    }
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


