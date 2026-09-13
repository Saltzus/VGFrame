#pragma once
#include "VertexBuffer.h"

struct DefaultInstance
{
    glm::mat4 model = glm::mat4(1.f);
    alignas(16) unsigned int textureID = 0;
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

    void GetOpenGLAttributeDescriptions(uint32_t& lastLocation) const override
    {
        for (int i = 0; i < 4; i++)
        {
            glVertexAttribPointer
            (
                lastLocation,
                4, GL_FLOAT, GL_FALSE,
                sizeof(DefaultInstance),
                (void*)(offsetof(DefaultInstance, model) + i * sizeof(glm::vec4))
            );

            glEnableVertexAttribArray(lastLocation);
            glVertexAttribDivisor(lastLocation, 1);

            lastLocation++;
        }

        glVertexAttribIPointer
        (
            lastLocation,
            1, GL_UNSIGNED_INT,
            sizeof(DefaultInstance),
            (void*)offsetof(DefaultInstance, textureID)
        );

        glEnableVertexAttribArray(lastLocation);
        glVertexAttribDivisor(lastLocation, 1);

        lastLocation++;
    }
};
