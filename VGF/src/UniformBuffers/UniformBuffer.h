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
        virtual ~UniformBufferObject() = default;

        virtual ShaderStage getShaderStage();
        virtual size_t SizeOf() const;
        virtual const void* Data() const;
    };
}