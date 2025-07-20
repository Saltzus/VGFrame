#pragma once

#include <glm/glm.hpp>

namespace VGF
{
    struct PBRbufferObject
    {
        glm::vec3 cameraPosition = { 0,0,0 };
        float metallicFactor = 1.f;
        glm::vec4 baseColorFactor = { 1, 1, 1, 1 };
        glm::vec3 emissiveFactor = { 0,0,0 };
        float roughnessFactor = 1.f;
        float occlusionStrength = 0.f;
    };
}

