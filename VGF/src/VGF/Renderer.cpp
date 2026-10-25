#include "Renderer.h"

#include "graphicsApi/opengl/OpenGlManager.h"
#include "graphicsApi/vulkan/VulkanManager.h"

#include "Physics.h"
#include "Texture.h"

namespace VGF
{
    const GraphicsApis Renderer::graphicApi = GraphicsApis::Vulkan;// TODO: add to a config file using YAML or something like that.

    Renderer::Renderer(const Shader& shader, std::vector<unsigned int>& indices, std::vector<float>& vertices)
    {
        switch (this->graphicApi)
        {
        case GraphicsApis::OpenGL:
            this->_impl = new VGF::Opengl::OpenglRenderer(indices, vertices);
            break;
        case GraphicsApis::Vulkan:
            this->_impl = new VGF::Vulkan::VulkanRenderer(indices, vertices, shader);
            break;
        default:
            this->_impl = new VGF::Opengl::OpenglRenderer(indices, vertices);
            break;
        }

        renderers.insert(this);
    }
    
    Renderer::~Renderer()
    {
        if (_impl) delete _impl;
        renderers.erase(this);
    }

    void Renderer::DestroyRenderers()
    {
        for (Renderer* renderer : renderers)
        {
            if (renderer && renderer->_impl)
            {
                delete renderer->_impl;
                renderer->_impl = nullptr;
            }
        }

        renderers.clear();
    }

    void Renderer::Render(const RenderData& data, const Shader& shader) const
    {
        if (shader.vertShader == "" || shader.fragShader == "") VGF::Log::Error("Shader is null");

        shader.Bind();
        this->_impl->Render(data, shader);
    }

    void Renderer::BatchRender(const RenderData& data, const Shader& shader) const
    {
        if (shader.vertShader == "" || shader.fragShader == "") VGF::Log::Error("Shader is null");

        shader.Bind();
        this->_impl->BatchRender(data, shader);
    }

    void Renderer::InitGraphics(GLFWwindow* window)
    {
        switch (graphicApi)
        {
        case GraphicsApis::OpenGL:
            graphics = new VGF::Opengl::Opengl(window);
            break;
        case GraphicsApis::Vulkan:
            graphics = new VGF::Vulkan::Vulkan(window);
            break;
        default:
            graphics = new VGF::Opengl::Opengl(window);
            break;
        }
    }

    void Renderer::DeleteGraphics()
    {
        Texture::DestroyTextures();
        DestroyRenderers();
        if (graphics) delete graphics;
    }

    void Renderer::RenderGraphics()
    {
        if (graphicApi == GraphicsApis::Vulkan)
            graphics->render();
    }

    const ::VGF::Vulkan::Vulkan* Renderer::GetVulkanData() { return ::VGF::Vulkan::Vulkan::vulkan; }
}
