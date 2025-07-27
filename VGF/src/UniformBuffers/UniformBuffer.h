#pragma once

#include <glm/glm.hpp>

namespace VGF
{
    enum ShaderStage
    {
        VERTEX,
        FRAGMENT
    };

    struct UniformBufferObject
    {
        virtual ShaderStage getShaderStage();
        virtual size_t SizeOf();
        virtual void* Data();
    };
}