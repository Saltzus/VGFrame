#pragma once

#include <glm/glm.hpp>

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


    struct LightBufferObject
    {
        glm::vec3 ambientLightColor = glm::vec3(1.0, 1.0, 1.0);
        float ambientLightIntensity = 0.2;

        DirectionalLight directionalLights[30];
        PointLight pointLights[30];

        float directionLightAmount = 0;
        float pointLightAmount = 0;
        float pad3;
        float pad4;

        static VGF::LightBufferObject getDefaultLightBuffer();
    };
}