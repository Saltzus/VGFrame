#include "PBRBuffer.h"

namespace VGF
{
    PBRbufferObject PBRbufferObject::_defaultPbrBuffer = {};

    PBRbufferObject* PBRbufferObject::getDefault()
    {
        _defaultPbrBuffer._data.cameraPosition = { 0,0,0 };
        _defaultPbrBuffer._data.metallicFactor = 1.f;
        _defaultPbrBuffer._data.baseColorFactor = { 1, 1, 1, 1 };
        _defaultPbrBuffer._data.emissiveFactor = { 0,0,0 };
        _defaultPbrBuffer._data.roughnessFactor = 1.f;
        _defaultPbrBuffer._data.occlusionStrength = 0.f;

        return &_defaultPbrBuffer;
    }

    ShaderStage PBRbufferObject::getShaderStage()
    {
        return ShaderStage::FRAGMENT;
    }

    size_t PBRbufferObject::SizeOf() const
    {
        return sizeof(PBRData);
    }

    const void* PBRbufferObject::Data() const
    {
        return &_data;
    }
}