#include "FrameBuffer.h"

#include "Renderer.h"

#include "graphicsApi/opengl/OpenglFrameBuffer.h"
#include "graphicsApi/vulkan/VulkanFrameBuffer.h"

namespace VGF
{
    FrameBuffer::FrameBuffer(Texture& framebufferTexture) : _framebufferTexture(framebufferTexture)
    {
        switch (Renderer::GetGraphicsApi())
        {
        case GraphicsApis::OpenGL:
            this->_impl = new VGF::Opengl::OpenglFrameBuffer(framebufferTexture.GetNativeImage(), framebufferTexture.GetWidth(), framebufferTexture.GetHeight());
            break;
        case GraphicsApis::Vulkan:
            this->_impl = new VGF::Vulkan::VulkanFrameBuffer(framebufferTexture.GetNativeImage(), framebufferTexture.GetWidth(), framebufferTexture.GetHeight());
            break;
        default:
            this->_impl = new VGF::Opengl::OpenglFrameBuffer(framebufferTexture.GetNativeImage(), framebufferTexture.GetWidth(), framebufferTexture.GetHeight());
            break;
        }
    }
    
    FrameBuffer::~FrameBuffer()
    {
        delete this->_impl;
    }

    void FrameBuffer::Bind() { _framebufferTexture.Bind(textureType::framebuffer); _impl->Bind(); }
    void FrameBuffer::UnBind() { _impl->UnBind(); }
    void FrameBuffer::Delete() {}
}
