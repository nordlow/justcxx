#pragma once

#include <iostream>

extern "C"
{
#include "wavfile.h"
}

class WavFile
{
public:
    WavFile() { WAV_init(&wav); }
    ~WavFile() { WAV_clear(&wav); }
    int load(const char *path) { return WAV_read(path, &wav); }
    friend std::ostream & operator << (std::ostream & os,
                                       const WavFile & wav)
    {
        WAV_print(&(wav.wav));
        return os;
    }
    const float *getData() const { return wav.data; }
private:
    WavFile_t wav;              // \todo MAKe private
};
