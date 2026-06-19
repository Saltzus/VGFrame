#pragma once
#include "VertexBuffer.h"

struct DefaultVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoord;
};

struct DefaultVertexBuffer : public VertexBuffer
{
    uint32_t GetSize() const override { return sizeof(DefaultVertex); }

    std::vector<VkVertexInputAttributeDescription> GetVulkanAttributeDescriptions(uint32_t lastLocation) const override
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.resize(4);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0 + lastLocation;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(DefaultVertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1 + lastLocation;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(DefaultVertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2 + lastLocation;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(DefaultVertex, color);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3 + lastLocation;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(DefaultVertex, texCoord);

        return attributeDescriptions;
    }
};