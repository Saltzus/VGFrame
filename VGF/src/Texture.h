#pragma once

#include "imgui.h"
#include "RenderImpl.h"
#include "Renderer.h"

namespace VGF
{
    class Texture
    {
    private:
        TextureImpl* _impl = nullptr;
        unsigned int _width{}, _height{};
    public:
        Texture(const std::string &filePath);
        Texture(const unsigned char* data, int format, unsigned int width, unsigned int height);

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        ~Texture();

        static ImVec2 GetImGuiUV0()
        {
            if (Renderer::GetGraphicsApi() == GraphicsApis::OpenGL) return ImVec2(0.0f, 1.0f);
            return ImVec2(0.0f, 0.0f);
        }

        static ImVec2 GetImGuiUV1()
        {
            if (Renderer::GetGraphicsApi() == GraphicsApis::OpenGL) return ImVec2(1.0f, 0.0f);
            return ImVec2(1.0f, 1.0f);
        }

        virtual void Bind(textureType type = textureType::color) const;
        virtual void* GetNativeImage() { return _impl->GetNativeImage(); }
        virtual ImTextureID GetImguiImage() const { return _impl->GetImGuiTexture(); }

        const unsigned int GetWidth() const { return _width; }
        const unsigned int GetHeight() const { return _height; }

        const static Texture* GetDefaultTexture();
        const static Texture* GetWhiteTexture();
        const static Texture* GetBlackTexture();
        const static Texture* GetBlueTexture();
    };

    
} 