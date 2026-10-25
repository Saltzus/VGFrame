#pragma once

#include <vector>
#include <map>
#include <unordered_set>
#include <optional>
#include <set>
#include <chrono>

#include "Log.h"

#include "Shader.h"
#include "RenderImpl.h"
#include "Window.h"
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
        static inline ApiImpl* graphics = nullptr;
        RendererImpl* _impl = nullptr;
        const static GraphicsApis graphicApi;
        static inline std::unordered_set<Renderer*> renderers;
    public:
        Renderer(const Shader& shader, std::vector<unsigned int>& indices, std::vector<float>& vertices);
       
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        
        ~Renderer();
        static void DestroyRenderers();

        void Render(const RenderData& data, const Shader& shader) const;
        
        void BatchRender(const RenderData& data, const Shader& shader) const;
        
        template<typename T>
        void BatchRender(const Shader& shader, const std::vector<T> instances, const std::vector<UniformBufferObject*> uniformBuffers) const
        {
            BatchRender(shader, instances.data(), instances.size(), sizeof(T), uniformBuffers);
        }

        static GraphicsApis GetGraphicsApi() {return graphicApi;};

        static const ::VGF::Vulkan::Vulkan* GetVulkanData();

        static void InitGraphics(GLFWwindow* window);
        static void DeleteGraphics();

        static void RenderGraphics();
    };
}
