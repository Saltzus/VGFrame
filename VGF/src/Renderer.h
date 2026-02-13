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
    enum class GraphicsApis 
    {
        OpenGL,
        DirectX,
        Vulkan
    };

    class Renderer
    {
    private:
        inline static ApiImpl* Api = nullptr;
        RendererImpl* _impl = nullptr;
        const static GraphicsApis graphicApi;
    public:
        Renderer(std::vector<unsigned int>& indices, std::vector<float>& vertices, std::vector<UniformBufferObject*> uniformBuffers);
        ~Renderer();

        void Render(PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers);
        static GraphicsApis GetGraphicsApi() {return graphicApi;};
        static void InitApi(GLFWwindow* window); 
        static void RenderGraphics();
    };
}
