#pragma once
#include "VertexBuffer.h"

struct DebugVertex
{
    glm::vec3 position;
    glm::vec3 color;
};

struct DebugVertexBuffer : public VertexBuffer
{
    uint32_t GetSize() const override { return sizeof(DebugVertex); }

    std::vector<VkVertexInputAttributeDescription> GetVulkanAttributeDescriptions(uint32_t lastLocation) const override
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.resize(2);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0 + lastLocation;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(DebugVertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1 + lastLocation;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(DebugVertex, color);

        return attributeDescriptions;
    }
};