#pragma once

#ifndef LMTZ_WAVEOUT_H
#define LMTZ_WAVEOUT_H

#pragma comment(lib, "winmm")

#include <windows.h>
#include <MMReg.h>
#include <ks.h>
#include <ksmedia.h>

#include "exception.h"

GENERATE_EXCEPTION(WaveOutException, Exception);

class AudioProvider
{
    public:
    virtual int NextAudioBuffer(void** buffer, int* length) = 0;
};

class WaveOut
{
    public:
    WaveOut(AudioProvider* provider, int sampleRate, int nrChannels, int bitsPerSample);
    ~WaveOut();

    void Start();
    void Stop();

    private:
    AudioProvider* _provider;
    HWAVEOUT _waveout;
    bool _started;
    int _buffered;
    static void CALLBACK Callback(HWAVEOUT waveout, UINT msg, DWORD_PTR userData, DWORD_PTR p1, DWORD_PTR p2);
    static DWORD WINAPI WaveOutCacheProc(LPVOID param);
};

#endif