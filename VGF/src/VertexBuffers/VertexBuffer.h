#pragma once
#include <glm/glm.hpp>

#include "vulkan/vulkan_core.h"

class VertexBuffer
{
public:
    virtual uint32_t GetSize() const = 0;
    virtual std::vector<VkVertexInputAttributeDescription> GetVulkanAttributeDescriptions() = 0;
};

