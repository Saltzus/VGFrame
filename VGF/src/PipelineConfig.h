#pragma once

#include "RenderImpl.h"
#include "Resource.h"

#include <vulkan/vulkan.hpp>



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
        PipelineConfig(std::string_view vertShader, std::string_view fragShader, bool translucent, Topology topology = Topology::TRIANGLE_LIST)
            : PipelineConfig(vertShader, fragShader, topology, translucent) {}

        PipelineConfig(std::string_view vertShader, std::string_view fragShader)
            : PipelineConfig(vertShader, fragShader, topology, translucent) {
        }
        
        PipelineConfig();
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
    };

    // Custom hash function
    struct PipelineConfigHash 
    {
        std::size_t operator()(const PipelineConfig& config) const noexcept 
        {
            size_t h1 = std::hash<bool>()(config.translucent);
            size_t h2 = std::hash<int>()(static_cast<int>(config.topology));
            size_t h3 = std::hash<std::string>()(config.vertShader);
            size_t h4 = std::hash<std::string>()(config.fragShader);

            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };
}