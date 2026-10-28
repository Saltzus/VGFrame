#pragma once

#include "imgui.h"
#include "RenderImpl.h"


#include <unordered_set>

namespace VGF
{
    class Renderer;
    class Texture
    {
    private:
        TextureImpl* _impl = nullptr;
        unsigned int _width{}, _height{};
        static inline std::unordered_set<Texture*> textures;
    public:
        Texture(const std::string &filePath);
        Texture(const unsigned char* data, int format, unsigned int width, unsigned int height);

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        ~Texture();

        static ImVec2 GetImGuiUV0();
        static ImVec2 GetImGuiUV1();

        static void DestroyTextures();

        virtual void* GetNativeImage() const { return _impl->GetNativeImage(); }
        virtual ImTextureID GetImguiImage() const { return _impl->GetImGuiTexture(); }

        const unsigned int GetWidth() const { return _width; }
        const unsigned int GetHeight() const { return _height; }

        const static Texture* GetDefaultTexture();
        const static Texture* GetWhiteTexture();
        const static Texture* GetBlackTexture();
        const static Texture* GetBlueTexture();
    };

    
} 