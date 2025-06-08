#pragma once

#include "RenderImpl.h"
#include "vulkan/vulkan.hpp"

namespace VGF
{
    enum Topology
    {
        LINE_LIST,
        TRIANGLE_LIST
    };

    class PipelineConfig {
    public:
        PipelineConfig(const char* vertShader, const char* fragShader, Topology topology = Topology::TRIANGLE_LIST);
        PipelineConfig();

        ~PipelineConfig();

        Topology topology = Topology::TRIANGLE_LIST;
        const char* vertShader = "../../../Examples/HelloWorld/Shaders/default.vert.spv";
        const char* fragShader = "../../../Examples/HelloWorld/Shaders/default.frag.spv";

        bool operator==(const PipelineConfig& other) const {
            return topology == other.topology &&
                vertShader == other.vertShader &&
                fragShader == other.fragShader;
        }

        void Activate();
        void Delete();
        unsigned int& ID();

    private:
        ShaderImpl* impl = nullptr;
    };

    // Custom hash function
    struct PipelineConfigHash {
        std::size_t operator()(const PipelineConfig& config) const {
            size_t h1 = std::hash<int>()(static_cast<int>(config.topology));
            size_t h2 = std::hash<const char*>()(config.vertShader);
            size_t h3 = std::hash<const char*>()(config.fragShader);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}