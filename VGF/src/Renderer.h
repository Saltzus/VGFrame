#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <optional>
#include <set>
#include <chrono>

#include "Log.h"

#include "PipelineConfig.h"
#include "RenderImpl.h"
#include "Window.h"
#include "Shader.h"
#include "Camera.h"

#include "UniformBuffers/LightBuffer.h"
#include "UniformBuffers/MatrixBuffer.h"
#include "UniformBuffers/PBRBuffer.h"

namespace VGF::Vulkan {
    class Vulkan;
}

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
       
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        
        ~Renderer();

        void Render(const PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers) const;
        void BatchRender(const PipelineConfig& config, std::vector<UniformBufferObject*> onetimeUniformBuffers, std::vector<UniformBufferObject*> instanceUniformBuffers) const;
        static GraphicsApis GetGraphicsApi() {return graphicApi;};

        static const ::VGF::Vulkan::Vulkan* GetVulkanData();

        static void InitApi(GLFWwindow* window); 
        static void RenderGraphics();
    };
}
