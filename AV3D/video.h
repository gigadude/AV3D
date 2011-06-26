#pragma once

#ifndef LMTZ_VIDEO_H
#define LMTZ_VIDEO_H

#include <windows.h>
#include "avinclude.h"
#include "waveout.h"
#include "exception.h"

class Video;

#define AV_AUDIO_BUFFER_SIZE AVCODEC_MAX_AUDIO_FRAME_SIZE

typedef void (*FRAME_UPDATED_CALLBACK)(Video* sender);

GENERATE_EXCEPTION(FileNotFoundException, Exception);
GENERATE_EXCEPTION(AVStreamException, Exception);

class Frame
{
    public:
    Frame();
    const void* PixelData() const;

    private:
    AVFrame* _frame;
    uint8_t* _buffer;
    friend class Video;
};

class Video
{
    public:
    Video(const char* filename, FRAME_UPDATED_CALLBACK callback);
    ~Video();

    void Start();
    void Stop();

    const void* PixelBuffer() const { return _currentBuffer; }

    private:
    static DWORD WINAPI VideoStreamProc(LPVOID parameter);
    void Load(const char* filename);
    
    FRAME_UPDATED_CALLBACK _frameUpdatedCallback;
    
    
    SwsContext* _swsCtx;
    AVFormatContext* _formatCtx;
    AVCodecContext* _videoCodecCtx;
    AVCodecContext* _audioCodecCtx;
    AVCodec* _videoCodec;
    AVCodec* _audioCodec;
    AVFrame* _currentFrame;
    uint8_t* _currentBuffer;
    WaveOut* _waveout;
    int _videoStreamIndex, _audioStreamIndex;
};

#endif