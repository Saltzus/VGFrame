#pragma once

#include "RenderImpl.h"
#include "Renderer.h"

namespace VGF
{
    class Texture
    {
    private:
        TextureImpl* _impl = nullptr;
        unsigned int _width, _height;
    public:
        Texture(const char* filePath);
        Texture(const unsigned char* data, int format, unsigned int width, unsigned int height);
        ~Texture();

        virtual void Bind(textureType type = textureType::color);
        virtual void* GetNativeImage() { return _impl->GetNativeImage(); }

        unsigned int GetWidth() { return _width; }
        unsigned int GetHeight() { return _height; }

        static Texture* GetDefaultTexture();
        static Texture* GetWhiteTexture();
        static Texture* GetBlackTexture();
        static Texture* GetBlueTexture();
    };

    
} 