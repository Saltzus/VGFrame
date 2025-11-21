#include "UniformBuffer.h"

namespace VGF
{
    ShaderStage UniformBufferObject::getShaderStage()
    {
        return ShaderStage::VERTEX;
    }

    size_t UniformBufferObject::SizeOf()
    {
        return 0;
    }

    void* UniformBufferObject::Data()
    {
        return nullptr;
    }
}