#pragma once

#include <vector>
#include "graphicsApi/opengl/OpenGlManager.h"
#include "graphicsApi/vulkan/VulkanManager.h"
#include "Shader.h"
#include "Camera.h"

#include "RenderImpl.h"

#include "UniformBuffers/LightBuffer.h"
#include "UniformBuffers/MatrixBuffer.h"
#include "UniformBuffers/PBRBuffer.h"

namespace VGF
{
    class PostProcess
    {
    private:
        PostProcessImpl* impl = nullptr;
    public:
        PostProcess(bool renderToScreen, std::vector<unsigned int>& indices, std::vector<float>& vertices, std::vector<UniformBufferObject*> uniformBuffers, PostProcess* inProcess = nullptr);
        PostProcess(bool renderToScreen, std::vector<UniformBufferObject*> uniformBuffers, PostProcess* inProcess = nullptr);
        ~PostProcess();

        void Render(PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers);
    };
}
