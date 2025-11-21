#pragma once

#include "RenderImpl.h"
#include "Renderer.h"

namespace VGF
{
    class Texture
    {
    private:
        TextureImpl* impl = nullptr;
    public:
        Texture(const char* filePath);
        Texture(const unsigned char* data, int format, int width, int height);
        ~Texture();

        virtual void Bind(textureType type = textureType::color);

        static Texture* GetDefaultTexture();
        static Texture* GetWhiteTexture();
        static Texture* GetBlackTexture();
        static Texture* GetBlueTexture();
    };

    
} 