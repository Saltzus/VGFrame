#include "Texture.h"
#include "Resource.h"

namespace VGF
{
    Texture::Texture(std::string filePath)
    {
        switch (Renderer::GetGraphicsApi())
        {
        case GraphicsApis::OpenGL:
            this->_impl = new VGF::Opengl::OpenglTexture(filePath);
            break;
        case GraphicsApis::Vulkan:
            this->_impl = new VGF::Vulkan::VulkanTexture(filePath);
            break;
        default:
            this->_impl = new VGF::Opengl::OpenglTexture(filePath);
            break;
        }
    }
    Texture::Texture(const unsigned char* data, int format, unsigned int width, unsigned int height) : _width(width), _height(height)
    {
        switch (Renderer::GetGraphicsApi())
        {
        case GraphicsApis::OpenGL:
            this->_impl = new VGF::Opengl::OpenglTexture(data, format, width, height);
            break;
        case GraphicsApis::Vulkan:
            this->_impl = new VGF::Vulkan::VulkanTexture(data, format, width, height);
            break;
        default:
            this->_impl = new VGF::Opengl::OpenglTexture(data, format, width, height);
            break;
        }
    }
    
    Texture::~Texture()
    {
        delete this->_impl;
    }

    void Texture::Bind(textureType type) const
    {
        _impl->Bind(type);
    }

    const Texture* Texture::GetDefaultTexture()
    {
        const static Texture* DEFAULT_TEXTURE = new Texture(Resource::Get("Textures/default.png").c_str());
        return DEFAULT_TEXTURE;
    }
    const Texture* Texture::GetWhiteTexture()
    {
        const unsigned char whitePixel[4] = { 255, 255, 255, 255 };
        const static Texture* WHITE_TEXTURE = new Texture(whitePixel, 4, 1, 1);
        return WHITE_TEXTURE;
    }
    const Texture* Texture::GetBlackTexture()
    {
        const unsigned char blackPixel[4] = { 0, 0, 0, 0 };
        const static Texture* BLACK_TEXTURE = new Texture(blackPixel, 4, 1, 1);
        return BLACK_TEXTURE;
    }

    const Texture* Texture::GetBlueTexture()
    {
        const unsigned char bluePixel[4] = { 128, 128, 255, 255 };
        const static Texture* BLUE_TEXTURE = new Texture(bluePixel, 4, 1, 1);
        return BLUE_TEXTURE;
    }
}
