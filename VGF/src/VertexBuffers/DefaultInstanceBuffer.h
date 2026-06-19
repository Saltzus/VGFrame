#pragma once
#include "VertexBuffer.h"

struct DefaultInstance
{
    glm::mat4 model;
    alignas(16) unsigned int textureID;
};

struct DefaultInstanceBuffer : public VertexBuffer
{
    uint32_t GetSize() const override { return sizeof(DefaultInstance); }

    std::vector<VkVertexInputAttributeDescription> GetVulkanAttributeDescriptions(uint32_t lastLocation) const override
    {
        std::vector<VkVertexInputAttributeDescription> instanceAttribs;
        instanceAttribs.resize(5);

        for (int col = 0; col < 4; col++) {
            instanceAttribs[col].binding = 1;
            instanceAttribs[col].location = col + lastLocation;
            instanceAttribs[col].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            instanceAttribs[col].offset = offsetof(DefaultInstance, model) + sizeof(glm::vec4) * col;
        }

        instanceAttribs[4].binding = 1;
        instanceAttribs[4].location = 4 + lastLocation;
        instanceAttribs[4].format = VK_FORMAT_R32_UINT;
        instanceAttribs[4].offset = offsetof(DefaultInstance, textureID);

        return instanceAttribs;
    }
};
