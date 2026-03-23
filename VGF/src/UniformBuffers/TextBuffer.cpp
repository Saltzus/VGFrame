#include "TextBuffer.h"
#include "../Renderer.h"

namespace VGF
{
    TextBufferObject TextBufferObject::_defaultTextBuffer = {};

    void TextBufferObject::SetData(void* data)
    {
        _data = *(static_cast<TextData*>(data));

        if (Renderer::GetGraphicsApi() == GraphicsApis::Vulkan) 
        {
            this->_data.proj[1][1] *= -1;
            this->_data.proj[3][1] *= -1;
        }
    }

    TextBufferObject* TextBufferObject::getDefault()
    {
        _defaultTextBuffer._data.model = glm::mat4(1.f);
        _defaultTextBuffer._data.proj = glm::mat4(1.f);
        _defaultTextBuffer._data.color = glm::vec3(1.f);
        _defaultTextBuffer._data.uvScale = glm::vec2(1.f);

        return &_defaultTextBuffer;
    }

    ShaderStage TextBufferObject::getShaderStage()
    {
        return ShaderStage::VERTEX;
    }

    size_t TextBufferObject::SizeOf() const
    {
        return sizeof(TextData);
    }

    const void* TextBufferObject::Data() const
    {
        return &_data;
    }
}