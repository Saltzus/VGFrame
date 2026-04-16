#include "Renderer.h"

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
        if (config.vertShader == "" || config.fragShader == "") VGF::Log::Error("Config is null"); // TODO : make pipeline config check if config already made with parameters or something to make pointer or reference not needed.

        config.Activate();
        this->_impl->Render(config, uniformBuffers);
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
