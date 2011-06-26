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

struct CachedHeader
{
    WAVEHDR* header;
    CachedHeader* next;
};

class WaveOut
{
    public:
    WaveOut(int sampleRate, int nrChannels, int bitsPerSample);
    ~WaveOut();

    void Start();
    void Stop();
    void AddBuffer(const void* data, size_t length, const void* parameter);

    private:
    CachedHeader* _cache;
    HANDLE _mutex;
    HWAVEOUT _waveout;
    static void CALLBACK Callback(HWAVEOUT waveout, UINT msg, DWORD_PTR userData, DWORD_PTR p1, DWORD_PTR p2);
};

#endif