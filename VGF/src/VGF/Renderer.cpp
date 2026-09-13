#include "Renderer.h"

#include "graphicsApi/opengl/OpenGlManager.h"
#include "graphicsApi/vulkan/VulkanManager.h"

#include "Physics.h"

namespace VGF
{
    const GraphicsApis Renderer::graphicApi = GraphicsApis::Vulkan;// TODO: add to a config file using YAML or something like that.

    Renderer::Renderer(std::vector<unsigned int>& indices, std::vector<float>& vertices, std::vector<UniformBufferObject*> uniformBuffers)
    {
        switch (this->graphicApi)
        {
        case GraphicsApis::OpenGL:
            this->_impl = new VGF::Opengl::OpenglRenderer(indices, vertices, uniformBuffers);
            break;
        case GraphicsApis::Vulkan:
            this->_impl = new VGF::Vulkan::VulkanRenderer(indices, vertices, uniformBuffers);
            break;
        default:
            this->_impl = new VGF::Opengl::OpenglRenderer(indices, vertices, uniformBuffers);
            break;
        }
    }
    
    Renderer::~Renderer()
    {
        delete this->_impl;
    }

    void Renderer::Render(const PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers) const
    {
        if (config.vertShader == "" || config.fragShader == "") VGF::Log::Error("Config is null");

        config.Activate();
        this->_impl->Render(config, uniformBuffers);
    }

    void Renderer::BatchRender(const PipelineConfig& config, const void* instanceData, size_t instanceCount, size_t instanceStride, std::vector<UniformBufferObject*> uniformBuffers) const
    {
        if (config.vertShader == "" || config.fragShader == "") VGF::Log::Error("Config is null");

        config.Activate();
        this->_impl->BatchRender(config, instanceData, instanceCount, instanceStride, uniformBuffers);
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
        if (graphics) delete graphics;
    }

    void Renderer::RenderGraphics()
    {
        if (graphicApi == GraphicsApis::Vulkan)
            graphics->render();
    }

    const ::VGF::Vulkan::Vulkan* Renderer::GetVulkanData() { return ::VGF::Vulkan::Vulkan::vulkan; }
}
