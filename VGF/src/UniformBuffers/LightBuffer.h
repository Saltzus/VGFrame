#pragma once

#include "UniformBuffer.h"

namespace VGF
{
    struct DirectionalLight
    {
        alignas(16) glm::vec3 direction;
        alignas(16) glm::vec3 color;
    };

    struct PointLight
    {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
    };

    struct LightData
    {
        glm::vec3 ambientLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float ambientLightIntensity = 0.2f;

        DirectionalLight directionalLights[30];
        PointLight pointLights[30];

        float directionLightAmount = 0;
        float pointLightAmount = 0;
        float pad3;
        float pad4;
    };

    class LightBufferObject : public UniformBufferObject
    {
    public:
        LightData data;

        static LightBufferObject* getDefault();
        virtual ShaderStage getShaderStage() override;
        virtual size_t SizeOf() override;
        virtual void* Data() override;
    private:
        static LightBufferObject defaultLightBuffer;
    };
}