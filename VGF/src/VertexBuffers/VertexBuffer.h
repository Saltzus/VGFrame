#pragma once
#include <glm/glm.hpp>
#include <vector>

#include <vulkan/vulkan_core.h>
#include <glad/glad.h>

class VertexBuffer
{
public:
    virtual uint32_t GetSize() const = 0;
    virtual std::vector<VkVertexInputAttributeDescription> GetVulkanAttributeDescriptions(uint32_t lastLocation) const = 0;
    virtual void GetOpenGLAttributeDescriptions(uint32_t& lastLocation) const = 0;
};

