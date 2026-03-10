#include "LightBuffer.h"

namespace VGF
{
    LightBufferObject LightBufferObject::_defaultLightBuffer = {};

    LightBufferObject* LightBufferObject::getDefault()
    {
        _defaultLightBuffer._data.ambientLightColor = glm::vec3(1.0, 1.0, 1.0);
        _defaultLightBuffer._data.ambientLightIntensity = 0.5;

        _defaultLightBuffer._data.pointLights[0].position = glm::vec3(0, 10, 0);
        _defaultLightBuffer._data.pointLights[0].color = glm::vec3(0.9, 0.9, 1);
        _defaultLightBuffer._data.pointLightAmount = 1;

        //_defaultLightBuffer._data.directionalLights[0].direction = glm::vec3(0, 0.5f, 0.5f);
        //_defaultLightBuffer._data.directionalLights[0].color = glm::vec3(5, 0, 0);
        //_defaultLightBuffer._data.directionalLights[1].direction = glm::vec3(0, -0.5f, -0.5f);
        //_defaultLightBuffer._data.directionalLights[1].color = glm::vec3(0, 0, 5);
        //_defaultLightBuffer._data.directionLightAmount = 2;

        return &_defaultLightBuffer;
    }

    ShaderStage LightBufferObject::getShaderStage()
    {
        return ShaderStage::FRAGMENT;
    }

    size_t LightBufferObject::SizeOf() const
    {
        return sizeof(LightData);
    }

    const void* LightBufferObject::Data() const
    {
        return &_data;
    }
}