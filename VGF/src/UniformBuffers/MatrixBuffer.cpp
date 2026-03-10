#include "MatrixBuffer.h"
#include "../Renderer.h"

namespace VGF
{
    MatrixBufferObject MatrixBufferObject::_defaultMatrixBuffer = {};

    void MatrixBufferObject::SetData(void* data)
    {
        _data = *(static_cast<MatrixData*>(data));

        if (Renderer::GetGraphicsApi() == GraphicsApis::Vulkan)
            this->_data.proj[1][1] *= -1;
    }

    MatrixBufferObject* MatrixBufferObject::getDefault()
    {
        _defaultMatrixBuffer._data.model = glm::mat4(1.f);
        _defaultMatrixBuffer._data.view = glm::mat4(1.f);
        _defaultMatrixBuffer._data.proj = glm::mat4(1.f);

        return &_defaultMatrixBuffer;
    }

    ShaderStage MatrixBufferObject::getShaderStage()
    {
        return ShaderStage::VERTEX;
    }

    size_t MatrixBufferObject::SizeOf() const
    {
        return sizeof(MatrixData);
    }

    const void* MatrixBufferObject::Data() const
    {
        return &_data;
    }
}