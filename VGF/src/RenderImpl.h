#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PBRBuffer.h"

namespace VGF
{
    enum textureType
    {
        color,
        metallicRoughness,
        emissive,
        occulsion,
        normal
    };

    class PipelineConfig;
    class Shader;
    class Camera;

    class RendererImpl 
    {
    public:
        virtual ~RendererImpl() {}
        virtual void Render(PipelineConfig& config, Camera* camera, glm::mat4 model, PBRbufferObject buffer) = 0;
    };

    class ApiImpl
    {
    public:
        virtual ~ApiImpl() {}
        virtual void render() {}
    };

    class TextureImpl
    {
    public:
        virtual ~TextureImpl() {}
        virtual void Bind(textureType type) = 0;
    };

	class ShaderImpl
	{
    public:
        virtual ~ShaderImpl() {}
        virtual void Activate() = 0;
        virtual void Delete() = 0;
        virtual unsigned int& Id() = 0;
	};
}