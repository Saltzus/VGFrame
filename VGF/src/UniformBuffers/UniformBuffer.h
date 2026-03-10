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
        virtual void SetData(void* data);

        bool operator==(const UniformBufferObject& other) const noexcept
        {
            const size_t s = SizeOf();
            if (s != other.SizeOf()) return false;
            return std::memcmp(Data(), other.Data(), s) == 0;
        }

        bool operator!=(const UniformBufferObject& other) const noexcept
        {
            return !(*this == other);
        }
    };
}