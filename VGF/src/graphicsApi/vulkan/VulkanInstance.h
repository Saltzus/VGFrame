#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>


namespace VGF::Vulkan
{
    struct Instance
    {
        alignas(16) glm::mat4 model = glm::mat4(1.f);
        alignas(16) uint32_t  textureIndex;
        

        static VkVertexInputBindingDescription getBindingDescription() 
        {
            VkVertexInputBindingDescription instanceBinding{};
            instanceBinding.binding = 1;
            instanceBinding.stride = sizeof(Instance);
            instanceBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

            return instanceBinding;
        }

        static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() 
        {
            std::array<VkVertexInputAttributeDescription, 5> instanceAttribs;
            for (int col = 0; col < 4; col++) {
                instanceAttribs[col].binding = 1;
                instanceAttribs[col].location = 4 + col;
                instanceAttribs[col].format = VK_FORMAT_R32G32B32A32_SFLOAT;
                instanceAttribs[col].offset = sizeof(glm::vec4) * col;
            }

            instanceAttribs[4].binding = 1;
            instanceAttribs[4].location = 8;
            instanceAttribs[4].format = VK_FORMAT_R32_UINT;
            instanceAttribs[4].offset = sizeof(glm::vec4) * 3;

            return instanceAttribs;
        }

        static std::vector<Instance> Create(std::vector<glm::mat4>& modelMatrices, std::vector<uint32_t>& textureIndex) {
            assert(modelMatrices.size() == textureIndex.size());
            std::vector<Instance> instances;
            instances.reserve(modelMatrices.size());

            for (size_t i = 0; i < modelMatrices.size(); i += 1) {
                Instance instance;
                instance.model = modelMatrices[i];
                instance.textureIndex = textureIndex[i];
                instances.push_back(instance);
            }
            return instances;
        }
    };
}