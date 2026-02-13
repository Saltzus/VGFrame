#pragma once

#include "UniformBuffer.h"


namespace VGF
{
    struct MatrixData
    {
        alignas(16) glm::mat4 model = glm::mat4(1.f);
        alignas(16) glm::mat4 view = glm::mat4(1.f);
        alignas(16) glm::mat4 proj = glm::mat4(1.f);
    };

    class MatrixBufferObject : public UniformBufferObject
    {
    public:
        void SetData(MatrixData data);
        MatrixData GetData() { return _data; }
        virtual const void* Data() const override;

        static MatrixBufferObject* getDefault();
        virtual ShaderStage getShaderStage() override;
        virtual size_t SizeOf() const override;
    private:
        MatrixData _data;
        static MatrixBufferObject _defaultMatrixBuffer;
    };
}

