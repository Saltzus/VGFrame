#include "MatrixBuffer.h"

namespace VGF
{
    MatrixBufferObject MatrixBufferObject::defaultMatrixBuffer = {};

    MatrixBufferObject* MatrixBufferObject::getDefault()
    {
        defaultMatrixBuffer.data.model = glm::mat4(1.f);
        defaultMatrixBuffer.data.view = glm::mat4(1.f);
        defaultMatrixBuffer.data.proj = glm::mat4(1.f);

        return &defaultMatrixBuffer;
    }

    ShaderStage MatrixBufferObject::getShaderStage()
    {
        return ShaderStage::VERTEX;
    }

    size_t MatrixBufferObject::SizeOf()
    {
        return sizeof(MatrixData);
    }

    void* MatrixBufferObject::Data()
    {
        return &data;
    }
}