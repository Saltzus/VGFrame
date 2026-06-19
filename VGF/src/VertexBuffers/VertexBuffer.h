#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "vulkan/vulkan_core.h"

class VertexBuffer
{
public:
    virtual uint32_t GetSize() const = 0;
    virtual std::vector<VkVertexInputAttributeDescription> GetVulkanAttributeDescriptions(uint32_t lastLocation) const = 0;
};

