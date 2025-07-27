#include "LightBuffer.h"

namespace VGF
{
    LightBufferObject LightBufferObject::defaultLightBuffer = {};

    LightBufferObject* LightBufferObject::getDefault()
    {
        defaultLightBuffer.data.directionalLights[0].direction = glm::vec3(0, 0.5f, 0.5f);
        defaultLightBuffer.data.directionalLights[0].color = glm::vec3(5, 0, 0);
        defaultLightBuffer.data.directionalLights[1].direction = glm::vec3(0, -0.5f, -0.5f);
        defaultLightBuffer.data.directionalLights[1].color = glm::vec3(0, 0, 5);
        defaultLightBuffer.data.directionLightAmount = 2;
        defaultLightBuffer.data.ambientLightColor = glm::vec3(1.0, 1.0, 1.0);
        defaultLightBuffer.data.ambientLightIntensity = 0.0;
        defaultLightBuffer.data.pointLights[0].position = glm::vec3(0, 1, 0);
        defaultLightBuffer.data.pointLights[0].color = glm::vec3(0, 0, 5);
        defaultLightBuffer.data.pointLightAmount = 1;

        return &defaultLightBuffer;
    }

    ShaderStage LightBufferObject::getShaderStage()
    {
        return ShaderStage::FRAGMENT;
    }

    size_t LightBufferObject::SizeOf()
    {
        return sizeof(LightData);
    }

    void* LightBufferObject::Data()
    {
        return &data;
    }
}