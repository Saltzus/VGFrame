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
        PipelineConfig(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent = false);

        PipelineConfig(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent, VertexBuffer& vertexBuffer, VertexBuffer& instanceBuffer);

        PipelineConfig(std::string_view vertShader, std::string_view fragShader, bool translucent = false, Topology topology = Topology::TRIANGLE_LIST)
            : PipelineConfig(vertShader, fragShader, topology, translucent) {}
        
        ~PipelineConfig();


        bool translucent = false;
        Topology topology = Topology::TRIANGLE_LIST;

        std::string vertShader;
        std::string fragShader;

        const std::type_index GetVertexBufferTypeId() const { return typeid(*_vertexBuffer); }
        const std::type_index GetInstanceBufferTypeId() const { return typeid(*_instanceBuffer); }

        const VertexBuffer* GetVertexBuffer() const { return _vertexBuffer.get(); }
        const VertexBuffer* GetInstanceBuffer() const { return _instanceBuffer.get(); }

        void SetVertexBuffer(VertexBuffer* vertexBuffer);
        template<typename T> void SetVertexBuffer()
        {
            _vertexBuffer = std::make_shared<T>();
        }

        void SetInstanceBuffer(VertexBuffer* vertexBuffer);
        template<typename T> void SetInstanceBuffer()
        {
            _instanceBuffer = std::make_shared<T>();
        }

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
        const unsigned int& ID() const;

    private:
        std::shared_ptr<ShaderImpl> _impl;
        std::shared_ptr<VertexBuffer> _vertexBuffer;
        std::shared_ptr<VertexBuffer> _instanceBuffer;
    };

    struct PipelineHashKey
    {
        bool translucent;
        Topology topology;
        std::string vertShader;
        std::string fragShader;
        bool offscreen;
        std::type_index vertexBufferType;
        std::type_index instanceBufferType;

        PipelineHashKey(const PipelineConfig& config, bool offscreen)
            : translucent(config.translucent)
            , topology(config.topology)
            , vertShader(config.vertShader)
            , fragShader(config.fragShader)
            , offscreen(offscreen)
            , vertexBufferType(config.GetVertexBufferTypeId())
            , instanceBufferType(config.GetInstanceBufferTypeId())
        {}

        bool operator==(const PipelineHashKey& other) const noexcept
        {
            return translucent == other.translucent &&
                   vertShader == other.vertShader &&
                   fragShader == other.fragShader &&
                   topology == other.topology &&
                   offscreen == other.offscreen &&
                   vertexBufferType == other.vertexBufferType &&
                   instanceBufferType == other.instanceBufferType;
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
            size_t h6 = std::hash<std::type_index>()(key.vertexBufferType);
            size_t h7 = std::hash<std::type_index>()(key.instanceBufferType);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6);
        }
    };
}
