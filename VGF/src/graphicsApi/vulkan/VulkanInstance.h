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
        alignas(16) uint32_t  textureIndex = 0;
    };
}