#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <vector>

#include <vulkan/vulkan.hpp>
#include "VulkanVertex.h"
#include "VulkanInstance.h"
#include "../../PipelineConfig.h"

namespace VGF::Vulkan
{
    class VulkanGraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const PipelineConfig& config, VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout, VkRenderPass& renderPass, VkPipelineLayout& pipelineLayout, VkPipeline& graphicsPipeline);

        void Bind();
        void Delete();
        unsigned int& Id() { return ID; };

        std::vector<char> vertShaderCode;
        std::vector<char> fragShaderCode;

    private:


        unsigned int ID;
        void compileErrors(unsigned int shader, const char* type);
    };
}