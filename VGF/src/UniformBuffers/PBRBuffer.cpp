#include "PBRBuffer.h"

namespace VGF
{
    PBRbufferObject PBRbufferObject::defaultPbrBuffer = {};

    PBRbufferObject* PBRbufferObject::getDefault()
    {
        defaultPbrBuffer.data.cameraPosition = { 0,0,0 };
        defaultPbrBuffer.data.metallicFactor = 1.f;
        defaultPbrBuffer.data.baseColorFactor = { 1, 1, 1, 1 };
        defaultPbrBuffer.data.emissiveFactor = { 0,0,0 };
        defaultPbrBuffer.data.roughnessFactor = 1.f;
        defaultPbrBuffer.data.occlusionStrength = 0.f;

        return &defaultPbrBuffer;
    }

    ShaderStage PBRbufferObject::getShaderStage()
    {
        return ShaderStage::FRAGMENT;
    }

    size_t PBRbufferObject::SizeOf()
    {
        return sizeof(PBRData);
    }

    void* PBRbufferObject::Data()
    {
        return &data;
    }
}