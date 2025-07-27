#pragma once

#include "UniformBuffer.h"

namespace VGF
{
    struct PBRData
    {
        glm::vec3 cameraPosition = { 0,0,0 };            
        float metallicFactor = 1.f;
        glm::vec4 baseColorFactor = { 1, 1, 1, 1 };
        glm::vec3 emissiveFactor = { 0,0,0 };
        float roughnessFactor = 1.f;
        float occlusionStrength = 0.f;
    };

    class PBRbufferObject : public UniformBufferObject
    {
    public:
        PBRData data;

        static PBRbufferObject* getDefault();
        virtual ShaderStage getShaderStage() override;
        virtual size_t SizeOf() override;
        virtual void* Data() override;
    private:
        static PBRbufferObject defaultPbrBuffer;
    };
}

