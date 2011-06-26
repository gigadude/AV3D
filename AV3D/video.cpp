#include "video.h"

Frame::Frame()
{
    _frame = avcodec_alloc_frame();
}

Video::Video(const char* filename, FRAME_UPDATED_CALLBACK callback)
{
    static bool ls_first = true;
    if (ls_first)
    {
        av_register_all();
        ls_first = false;
    }

    _videoStreamIndex = -1;
    _audioStreamIndex = -1;

    _videoQueue = new AVPacketQueue();
    _audioQueue = new AVPacketQueue();

    _frameUpdatedCallback = callback;
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

    _audioCodecCtx = _formatCtx->streams[_audioStreamIndex]->codec;
    _audioCodec = avcodec_find_decoder(_audioCodecCtx->codec_id);
    if (!_audioCodec) throw new AVStreamException("Could not find audio decoder \"%s\"", _audioCodecCtx->codec_name);

    error = avcodec_open(_videoCodecCtx, _videoCodec);
    if (error < 0) throw new AVStreamException("Could not open video decoder \"%s\"", _videoCodecCtx->codec_name);

    error = avcodec_open(_audioCodecCtx, _audioCodec);
    if (error < 0) throw new AVStreamException("Could not open audio decoder \"%s\"", _audioCodecCtx->codec_name);

    _swsCtx = sws_getContext(
        _videoCodecCtx->width, _videoCodecCtx->height, _videoCodecCtx->pix_fmt,
        _videoCodecCtx->width, _videoCodecCtx->height, PIX_FMT_RGB24,
        SWS_BICUBIC, NULL, NULL, NULL);

    int size = avpicture_get_size(PIX_FMT_RGB24, _videoCodecCtx->width, _videoCodecCtx->height);
    _currentFrame = avcodec_alloc_frame();
    _currentBuffer = (uint8_t*) av_malloc(size * sizeof(uint8_t));
    avpicture_fill((AVPicture*)_currentFrame, _currentBuffer, PIX_FMT_RGB24, _videoCodecCtx->width, _videoCodecCtx->height);

    _waveout = new WaveOut(_audioCodecCtx->sample_rate, _audioCodecCtx->channels, 16);
}

void Video::Start()
{
    CreateThread(NULL, 0, AVStreamProc, this, NULL, NULL);
}

void Video::NextFrame()
{
    int frameDone = 0;
    AVFrame* frame = avcodec_alloc_frame();
    AVPacket* packet;
    while (!frameDone && (packet = _videoQueue->Dequeue()) != NULL)
    {
        avcodec_decode_video2(_videoCodecCtx, frame, &frameDone, packet);
        av_free_packet(packet);
    }
    sws_scale(_swsCtx, frame->data, frame->linesize, 0, _videoCodecCtx->height, _currentFrame->data, _currentFrame->linesize);
    if (_frameUpdatedCallback) _frameUpdatedCallback(this);
    av_free(frame);
}

void Video::NextAudioBuffer()
{
    int bufferDone = 0;
    AVPacket* packet;
    int16_t* audioBuffer = (int16_t*) av_malloc(AV_AUDIO_BUFFER_SIZE + FF_INPUT_BUFFER_PADDING_SIZE);
    while ((packet = _audioQueue->Dequeue()) != NULL)
    {
        int bufferSize = AVCODEC_MAX_AUDIO_FRAME_SIZE;
        int consumed = avcodec_decode_audio3(_audioCodecCtx, audioBuffer, &bufferSize, packet);
        if (consumed >= 0 && bufferSize > 0)        
        {
            _waveout->AddBuffer(audioBuffer, bufferSize, NULL);
            break;
        }
    }
    av_free(audioBuffer);
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
        instance->NextFrame();
        instance->NextAudioBuffer();
    }
    return 0;
}

