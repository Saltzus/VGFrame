#pragma once

#include <miniaudio/miniaudio.h>
#include <glm/common.hpp>
#include <iostream>

namespace VGF
{
    class Audio
    {
    public:
        Audio();
        ~Audio();

        void PlayAudio(const char* audioFile);
    private:
        ma_result result;
        ma_engine engine;
    };
    

    
}