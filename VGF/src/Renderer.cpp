#include "Renderer.h"

namespace VGF
{
    ApiImpl* Renderer::Api = nullptr;
    const GraphicsApis Renderer::graphicApi = GraphicsApis::OpenGL;

    Renderer::Renderer(std::vector<unsigned int>& indices, std::vector<float>& vertices)
    {
        switch (this->graphicApi)
        {
        case GraphicsApis::OpenGL:
            this->impl = new VGF::Opengl::OpenglRenderer(indices, vertices);
            break;
        case GraphicsApis::Vulkan:
            this->impl = new VGF::Vulkan::VulkanRenderer(indices, vertices);
            break;
        default:
            this->impl = new VGF::Opengl::OpenglRenderer(indices, vertices);
            break;
        }
    }
    
    Renderer::~Renderer()
    {
        delete this->impl;
    }

    void Renderer::Render(PipelineConfig& config, Camera* camera, glm::mat4 model, PBRbufferObject PBRbuffer, LightBufferObject lightBuffer)
    {
        config.Activate();
        this->impl->Render(config, camera, model, PBRbuffer, lightBuffer);
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
