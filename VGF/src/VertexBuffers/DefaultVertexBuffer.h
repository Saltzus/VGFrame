#pragma once
#include "VertexBuffer.h"

struct DefaultVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoord;
    //glm::vec2 padding;

    glm::vec4 jointIndices;
    glm::vec4 jointWeights;
};

struct DefaultVertexBuffer : public VertexBuffer
{
    uint32_t GetSize() const override { return sizeof(DefaultVertex); }

    std::vector<VkVertexInputAttributeDescription> GetVulkanAttributeDescriptions(uint32_t lastLocation) const override
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.resize(6);

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

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4 + lastLocation;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(DefaultVertex, jointIndices);

        attributeDescriptions[5].binding = 0;
        attributeDescriptions[5].location = 5 + lastLocation;
        attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[5].offset = offsetof(DefaultVertex, jointWeights);

        return attributeDescriptions;
    }

    void GetOpenGLAttributeDescriptions(uint32_t& lastLocation) const override
    {
        // position attribute
        glVertexAttribPointer(lastLocation, 3, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void*)offsetof(DefaultVertex, position));
        glEnableVertexAttribArray(lastLocation);
        lastLocation++;

        // normals
        glVertexAttribPointer(lastLocation, 3, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void*)offsetof(DefaultVertex, normal));
        glEnableVertexAttribArray(lastLocation);
        lastLocation++;

        // vertex color
        glVertexAttribPointer(lastLocation, 3, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void*)offsetof(DefaultVertex, color));
        glEnableVertexAttribArray(lastLocation);
        lastLocation++;

        // Texture position attribute
        glVertexAttribPointer(lastLocation, 2, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void*)offsetof(DefaultVertex, texCoord));
        glEnableVertexAttribArray(lastLocation);
        lastLocation++;

        // Texture position attribute
        glVertexAttribPointer(lastLocation, 4, GL_UNSIGNED_INT, GL_FALSE, sizeof(DefaultVertex), (void*)offsetof(DefaultVertex, jointIndices));
        glEnableVertexAttribArray(lastLocation);
        lastLocation++;

        // Texture position attribute
        glVertexAttribPointer(lastLocation, 4, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void*)offsetof(DefaultVertex, jointWeights));
        glEnableVertexAttribArray(lastLocation);
        lastLocation++;
    }
};