#include "LightBuffer.h"

namespace VGF
{
    VGF::LightBufferObject LightBufferObject::getDefaultLightBuffer()
    {
        LightBufferObject buffer;
        buffer.directionalLights[0].direction = glm::vec3(0, 0.5f, 0.5f);
        buffer.directionalLights[0].color = glm::vec3(5, 0, 0);
        
        buffer.directionalLights[1].direction = glm::vec3(0, -0.5f, -0.5f);
        buffer.directionalLights[1].color = glm::vec3(0, 0, 5);
        buffer.directionLightAmount = 2;

        buffer.ambientLightColor = glm::vec3(1.0, 1.0, 1.0);
        buffer.ambientLightIntensity = 0.0;

        buffer.pointLights[0].position = glm::vec3(0, 1, 0);
        buffer.pointLights[0].color = glm::vec3(0, 0, 5);
        buffer.pointLightAmount = 1;

        return buffer;
    }
}