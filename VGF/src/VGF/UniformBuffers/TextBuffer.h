#pragma once

#include "UniformBuffer.h"


namespace VGF
{
    struct TextData
    {
        alignas(16) glm::mat4 model = glm::mat4(1.f);
        alignas(16) glm::mat4 proj = glm::mat4(1.f);
        alignas(16) glm::vec4 color = glm::vec4(1.f);
        alignas(16) glm::vec2 uvScale = glm::vec2(1.f);
    };

    class TextBufferObject : public UniformBufferObject
    {
    public:
        void SetData(void* data) override;
        TextData GetData() { return _data; }
        virtual const void* Data() const override;

        static TextBufferObject* getDefault();
        virtual ShaderStage getShaderStage() override;
        virtual size_t SizeOf() const override;
    private:
        TextData _data;
        static TextBufferObject _defaultTextBuffer;
    };
}

