#include "waveout.h"

WaveOut::WaveOut(int sampleRate, int nrChannels, int bitsPerSample)
{
    _cache = 0;
    _mutex = CreateMutex(NULL, FALSE, NULL);

    WAVEFORMATEXTENSIBLE fmt;
	fmt.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
	fmt.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE ;
	fmt.Format.nChannels = nrChannels;
	fmt.Format.nSamplesPerSec = sampleRate;
	fmt.Format.wBitsPerSample = bitsPerSample;
	fmt.Format.nBlockAlign = (fmt.Format.wBitsPerSample >> 3) * fmt.Format.nChannels;
	fmt.Format.nAvgBytesPerSec = fmt.Format.nBlockAlign * fmt.Format.nSamplesPerSec;

    switch (nrChannels)
    {
        case 1:
        fmt.dwChannelMask = KSAUDIO_SPEAKER_MONO;
        break;

        case 2:
	    fmt.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
        break;

        default:
        throw new WaveOutException("%d channel audio is not supported", nrChannels);
    }

	fmt.Samples.wValidBitsPerSample = fmt.Format.wBitsPerSample;
    switch (bitsPerSample)
    {
        case 32:
        fmt.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        break;

        default:
	    fmt.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
        break;
    }
	
    MMRESULT error = waveOutOpen(&_waveout, WAVE_MAPPER, &fmt.Format, (DWORD_PTR) Callback, (DWORD_PTR) this, CALLBACK_FUNCTION);
    if (error != MMSYSERR_NOERROR) throw new WaveOutException("Could not open the audio device (0x%08X)", error);
    
    waveOutPause(_waveout);
}

void WaveOut::AddBuffer(const void* data, size_t length, const void* parameter)
{
    static int inBuffer = 0;

    WAVEHDR* hdr = (WAVEHDR*) calloc(1, sizeof(WAVEHDR) + length);
    hdr->dwBufferLength = length;
    hdr->lpData = ((char*)hdr) + sizeof(WAVEHDR);
    hdr->dwUser = (DWORD_PTR) parameter;
    memcpy(hdr->lpData, data, length);
 
    if (inBuffer < 4)
    {
        waveOutPrepareHeader(_waveout, hdr, sizeof(WAVEHDR));
        waveOutWrite(_waveout, hdr, sizeof(WAVEHDR));
        inBuffer++;
    }
    else
    {
        WaitForSingleObject(_mutex, INFINITE);

        CachedHeader** cache = &_cache;
        while (*cache) cache = &(*cache)->next;
        *cache = new CachedHeader();
        (*cache)->header = hdr;
        (*cache)->next = 0;
        
        ReleaseMutex(_mutex);
    }
}

void WaveOut::Start()
{
    waveOutRestart(_waveout);
}

void WaveOut::Stop()
{
    waveOutPause(_waveout);
}

void CALLBACK WaveOut::Callback(HWAVEOUT waveout, UINT msg, DWORD_PTR userData, DWORD_PTR p1, DWORD_PTR p2)
{
    switch (msg)
    {
        case WOM_DONE:
            WaveOut* instance = (WaveOut*) userData;
            WAVEHDR* hdr = (WAVEHDR*) p1;
            waveOutUnprepareHeader(waveout, hdr, sizeof(WAVEHDR));
            free(hdr);

            WaitForSingleObject(instance->_mutex, INFINITE);
            if (instance->_cache)
            {
                waveOutPrepareHeader(waveout, instance->_cache->header, sizeof(WAVEHDR));
                waveOutWrite(waveout, instance->_cache->header, sizeof(WAVEHDR));
                CachedHeader* to_delete = instance->_cache;
                instance->_cache = instance->_cache->next;
                delete to_delete;
            }
            ReleaseMutex(instance->_mutex);
            break;
    }
}
