#pragma once

#include "RenderImpl.h"
#include "Resource.h"
#include "ShaderCompiler.h"
#include <typeindex>

#include <vulkan/vulkan.hpp>

namespace VGF
{
    enum Topology
    {
        LINE_LIST,
        TRIANGLE_LIST
    };

    class Shader 
    {
    public:
        Shader(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent = false);

        template<typename VertBuffr1, typename VertBuffr2>
        Shader(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent, const VertBuffr1& vertexBuffer, const VertBuffr2& instanceBuffer)
            : Shader(vertShader, fragShader, topology, translucent)
        {
        }

        Shader(std::string_view vertShader, std::string_view fragShader, bool translucent = false, Topology topology = Topology::TRIANGLE_LIST)
            : Shader(vertShader, fragShader, topology, translucent) {}
        
        ~Shader();


        bool translucent = false;
        Topology topology = Topology::TRIANGLE_LIST;

        ShaderReflection reflect;

        std::string vertShader;
        std::string fragShader;

        Slang::ComPtr<slang::IBlob> spirvfrag;
        Slang::ComPtr<slang::IBlob> spirvvert;

        static const VGF::Shader& GetDefault()
        {
            static const VGF::Shader shader
            (
                VGF::Resource::Get("Shaders/default_vert.slang"),
                VGF::Resource::Get("Shaders/default_frag.slang"),
                VGF::Topology::TRIANGLE_LIST
            );
            return shader;
        }

        const VkPrimitiveTopology GetVulkanTopology() const;
        void Bind() const;
        const unsigned int& ID() const;

    private:
        std::shared_ptr<ShaderImpl> _impl;
    };

    struct PipelineHashKey
    {
        bool translucent;
        Topology topology;
        std::string vertShader;
        std::string fragShader;
        bool offscreen;

        PipelineHashKey(const Shader& config, bool offscreen)
            : translucent(config.translucent)
            , topology(config.topology)
            , vertShader(config.vertShader)
            , fragShader(config.fragShader)
            , offscreen(offscreen)
        {}

        bool operator==(const PipelineHashKey& other) const noexcept
        {
            return translucent == other.translucent &&
                vertShader == other.vertShader &&
                fragShader == other.fragShader &&
                topology == other.topology &&
                offscreen == other.offscreen;
        }
    };


    struct PipelineConfigKeyHash
    {
        std::size_t operator()(const PipelineHashKey& key) const noexcept
        {
            size_t h1 = std::hash<bool>()(key.translucent);
            size_t h2 = std::hash<int>()(static_cast<int>(key.topology));
            size_t h3 = std::hash<std::string>()(key.vertShader);
            size_t h4 = std::hash<std::string>()(key.fragShader);
            size_t h5 = std::hash<bool>()(key.offscreen);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
        }
    };
}
