#include "PostProcess.h"

namespace VGF
{
    PostProcess::PostProcess(bool renderToScreen, std::vector<unsigned int>& indices, std::vector<float>& vertices, std::vector<UniformBufferObject*> uniformBuffers, PostProcess* inProcess)
    {
        switch (Renderer::GetGraphicsApi())
        {
        case GraphicsApis::OpenGL:
            this->impl = new VGF::Opengl::OpenglPostProcess(renderToScreen, uniformBuffers, inProcess->impl, indices, vertices);
            break;
        case GraphicsApis::Vulkan:
            this->impl = new VGF::Vulkan::VulkanPostProcess(uniformBuffers, inProcess->impl, indices, vertices);
            break;
        default:
            this->impl = new VGF::Opengl::OpenglPostProcess(renderToScreen, uniformBuffers, inProcess->impl, indices, vertices);
            break;
        }
    }

    PostProcess::PostProcess(bool renderToScreen, std::vector<UniformBufferObject*> uniformBuffers, PostProcess* inProcess)
    {
        bool sendImpl = false;
        if (inProcess != nullptr)
            sendImpl = true;

        switch (Renderer::GetGraphicsApi())
        {
        case GraphicsApis::OpenGL:
            this->impl = new VGF::Opengl::OpenglPostProcess(renderToScreen, uniformBuffers, sendImpl ? inProcess->impl : nullptr);
            break;
        case GraphicsApis::Vulkan:
            this->impl = new VGF::Vulkan::VulkanPostProcess(uniformBuffers, sendImpl ? inProcess->impl : nullptr);
            break;
        default:
            this->impl = new VGF::Opengl::OpenglPostProcess(renderToScreen, uniformBuffers, sendImpl ? inProcess->impl : nullptr);
            break;
        }
    }
    
    PostProcess::~PostProcess()
    {
        delete this->impl;
    }

    void PostProcess::Render(PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers)
    {
        config.Activate();
        this->impl->Render(config, uniformBuffers);
    }
}
