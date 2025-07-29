#include "Renderer.h"

namespace VGF
{
    const GraphicsApis Renderer::graphicApi = GraphicsApis::Vulkan; // TODO: add to a config file using YAML or something like that.

    Renderer::Renderer(std::vector<unsigned int>& indices, std::vector<float>& vertices, std::vector<UniformBufferObject*> uniformBuffers)
    {
        switch (this->graphicApi)
        {
        case GraphicsApis::OpenGL:
            this->impl = new VGF::Opengl::OpenglRenderer(indices, vertices, uniformBuffers);
            break;
        case GraphicsApis::Vulkan:
            this->impl = new VGF::Vulkan::VulkanRenderer(indices, vertices, uniformBuffers);
            break;
        default:
            this->impl = new VGF::Opengl::OpenglRenderer(indices, vertices, uniformBuffers);
            break;
        }
    }
    
    Renderer::~Renderer()
    {
        delete this->impl;
    }

    void Renderer::Render(PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers)
    {
        config.Activate();
        this->impl->Render(config, uniformBuffers);
    }

    void Renderer::InitApi(GLFWwindow* window)
    {
        switch (graphicApi)
        {
        case GraphicsApis::OpenGL:
            Api = new VGF::Opengl::Opengl(window);
            break;
        case GraphicsApis::Vulkan:
            Api = new VGF::Vulkan::Vulkan(window);
            break;
        default:
            Api = new VGF::Opengl::Opengl(window);
            break;
        }
    }

    void Renderer::RenderGraphics()
    {
        if (graphicApi == GraphicsApis::Vulkan)
            Api->render();
    }
}
