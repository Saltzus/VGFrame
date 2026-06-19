#pragma once

#include "RenderImpl.h"
#include "Resource.h"
#include "VertexBuffers/VertexBuffer.h"

#include <vulkan/vulkan.hpp>
#include <typeindex>

#include "VertexBuffers/DefaultInstanceBuffer.h"
#include "VertexBuffers/DefaultVertexBuffer.h"


namespace VGF
{
    enum Topology
    {
        LINE_LIST,
        TRIANGLE_LIST
    };

    class PipelineConfig 
    {
    public:

        PipelineConfig();
        PipelineConfig(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent = false);

        PipelineConfig(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent, VertexBuffer& vertexBuffer, VertexBuffer& instanceBuffer);

        PipelineConfig(std::string_view vertShader, std::string_view fragShader, bool translucent = false, Topology topology = Topology::TRIANGLE_LIST)
            : PipelineConfig(vertShader, fragShader, topology, translucent) {}
        
        ~PipelineConfig();

        bool operator==(const PipelineConfig& other) const 
        {
            return
            { 
                translucent == other.translucent &&
                topology    == other.topology    &&
                vertShader  == other.vertShader  &&
                fragShader  == other.fragShader
            };

        }

        bool translucent = false;
        Topology topology = Topology::TRIANGLE_LIST;

        std::string vertShader;
        std::string fragShader;

        const std::type_index GetVertexBufferTypeId() const { return typeid(*_vertexBuffer); }
        const std::type_index GetInstanceBufferTypeId() const { return typeid(*_instanceBuffer); }

        const VertexBuffer* GetVertexBuffer() const { return _vertexBuffer; }
        const VertexBuffer* GetInstanceBuffer() const { return _instanceBuffer; }

        void SetVertexBuffer(VertexBuffer* vertexBuffer);
        void SetInstanceBuffer(VertexBuffer* vertexBuffer);

        static const VGF::PipelineConfig& GetDefault()
        {
            static const VGF::PipelineConfig config
            (
                VGF::Resource::Get("Shaders/default.vert"),
                VGF::Resource::Get("Shaders/default.frag"),
                VGF::Topology::TRIANGLE_LIST
            );
            return config;
        }

        const VkPrimitiveTopology GetVulkanTopology() const;
        void Activate() const;
        void Delete();
        const unsigned int& ID() const;

    private:
        ShaderImpl* _impl = nullptr;

        VertexBuffer* _vertexBuffer;
        VertexBuffer* _instanceBuffer;

        bool _createdVertexBuffer = true;
        bool _createdInstanceBuffer = true;
    };

    struct PipelineHashKey
    {
        const PipelineConfig& config;
        bool offscreen;

        bool operator==(const PipelineHashKey& other) const noexcept
        {
            return
            {
                config.translucent == other.config.translucent &&
                config.vertShader == other.config.vertShader &&
                config.fragShader == other.config.fragShader &&
                config.topology == other.config.topology &&
                offscreen == other.offscreen &&

                config.GetVertexBufferTypeId() == other.config.GetVertexBufferTypeId() &&
                config.GetInstanceBufferTypeId() == other.config.GetInstanceBufferTypeId()
            };
        }
    };

    struct PipelineConfigKeyHash
    {
        std::size_t operator()(const PipelineHashKey& key) const noexcept
        {
            size_t h1 = std::hash<bool>()(key.config.translucent);
            size_t h2 = std::hash<int>()(static_cast<int>(key.config.topology));
            size_t h3 = std::hash<std::string>()(key.config.vertShader);
            size_t h4 = std::hash<std::string>()(key.config.fragShader);
            size_t h5 = std::hash<bool>()(key.offscreen);
            size_t h6 = std::hash<std::type_index>()(std::type_index(key.config.GetVertexBufferTypeId()));
            size_t h7 = std::hash<std::type_index>()(std::type_index(key.config.GetInstanceBufferTypeId()));
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6);
        }
    };
}
