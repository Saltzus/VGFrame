#include "AnimationBuffer.h"

namespace VGF
{
    AnimationBufferObject AnimationBufferObject::_defaultAnimationBuffer = {};

    AnimationBufferObject* AnimationBufferObject::getDefault()
    {
        return &_defaultAnimationBuffer;
    }

    ShaderStage AnimationBufferObject::getShaderStage()
    {
        return ShaderStage::VERTEX;
    }

    size_t AnimationBufferObject::SizeOf() const
    {
        return sizeof(AnimationData);
    }

    const void* AnimationBufferObject::Data() const
    {
        return &_data;
    }
}