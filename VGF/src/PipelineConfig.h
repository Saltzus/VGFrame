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

    class PipelineConfig 
    {
    public:
        PipelineConfig(std::string_view vertShader, std::string_view fragShader, Topology topology = Topology::TRIANGLE_LIST);
        PipelineConfig();

        ~PipelineConfig();

        Topology topology = Topology::TRIANGLE_LIST;
        std::string vertShader;
        std::string fragShader;

        bool operator==(const PipelineConfig& other) const 
        {
            return topology    == other.topology &&
                   vertShader   == other.vertShader &&
                   fragShader   == other.fragShader;
        }

        void Activate();
        void Delete();
        unsigned int& ID();

    private:
        ShaderImpl* _impl = nullptr;
    };

    // Custom hash function
    struct PipelineConfigHash 
    {
        std::size_t operator()(const PipelineConfig& config) const noexcept 
        {
            // note: std::hash<std::string> is well-defined
            size_t h1 = std::hash<int>()(static_cast<int>(config.topology));
            size_t h2 = std::hash<std::string>()(config.vertShader);
            size_t h3 = std::hash<std::string>()(config.fragShader);
            // simple combinator—feel free to replace with boost::hash_combine
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}