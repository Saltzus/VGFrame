#include "UniformBuffer.h"

namespace VGF
{
    ShaderStage UniformBufferObject::getShaderStage()
    {
        return ShaderStage::VERTEX;
    }

    size_t UniformBufferObject::SizeOf() const
    {
        return 0;
    }

    const void* UniformBufferObject::Data() const
    {
        return nullptr;
    }

    void UniformBufferObject::SetData(void* data) {}
}