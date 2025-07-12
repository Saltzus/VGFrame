#pragma once

#include <glm/glm.hpp>

namespace VGF
{
    struct PBRbufferObject
    {
        glm::vec4 baseColorFactor = { 1, 0, 0, 1 };
        glm::vec3 emissiveFactor = { 0,0,0 };
        float metallicFactor = 1.f;
        float roughnessFactor = 1.f;
        float occlusionStrength = 0.f;
    };
}

