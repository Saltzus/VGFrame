#include "MatrixBuffer.h"
#include "../Renderer.h"

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
        if (Renderer::GetGraphicsApi() == GraphicsApis::Vulkan)
            data.proj[1][1] *= -1;

        return &data;
    }
}