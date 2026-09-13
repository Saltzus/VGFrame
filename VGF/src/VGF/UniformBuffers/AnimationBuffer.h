#pragma once

#include "UniformBuffer.h"

namespace VGF
{
    constexpr size_t MAX_JOINTS = 128;

    struct AnimationData
    {
        glm::mat4 jointMatrices[MAX_JOINTS]{};
    };

    class AnimationBufferObject : public UniformBufferObject
    {
    public:
        void SetData(AnimationData data) { _data = data; }
        AnimationData GetData() { return _data; }
        virtual const void* Data() const override;

        static AnimationBufferObject* getDefault();
        virtual ShaderStage getShaderStage() override;
        virtual size_t SizeOf() const override;
    private:
        AnimationData _data;
        static AnimationBufferObject _defaultAnimationBuffer;
    };
}

