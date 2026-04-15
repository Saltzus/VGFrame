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
        Texture(std::string filePath);
        Texture(const unsigned char* data, int format, unsigned int width, unsigned int height);

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        ~Texture();

        virtual void Bind(textureType type = textureType::color) const;
        virtual void* GetNativeImage() { return _impl->GetNativeImage(); }

        const unsigned int GetWidth() const { return _width; }
        const unsigned int GetHeight() const { return _height; }

        const static Texture* GetDefaultTexture();
        const static Texture* GetWhiteTexture();
        const static Texture* GetBlackTexture();
        const static Texture* GetBlueTexture();
    };

    
} 