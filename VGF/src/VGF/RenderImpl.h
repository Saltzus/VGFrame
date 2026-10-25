#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "imgui.h"
#include "UniformBuffers/UniformBuffer.h"
#include "RenderData.h"

namespace VGF
{
    enum textureType
    {
        color,
        normal,
        metallicRoughness,
        emissive,
        occulsion
    };

    class Shader;
    class Shader;
    class Camera;
    class Window;

    struct RendererImpl
    {
        virtual ~RendererImpl() {}
        virtual void Render(const RenderData& renderData, const Shader& shader) = 0;
        virtual void BatchRender(const RenderData& renderData, const Shader& shader) = 0;
    };

    struct ApiImpl
    {
        virtual ~ApiImpl() {}
        virtual void render() {}
    };

    struct TextureImpl
    {
        virtual ~TextureImpl() {}
        virtual void Bind(textureType type) = 0;
        virtual void* GetNativeImage() = 0;
        virtual ImTextureID GetImGuiTexture() = 0;
    };

	struct ShaderImpl
	{
        virtual ~ShaderImpl() {}
        virtual void Bind() = 0;
        virtual void Delete() = 0;
        virtual unsigned int& Id() = 0;
	};

    struct FrameBufferImpl
    {
        virtual ~FrameBufferImpl() {}
        virtual void Bind() = 0;
        virtual void UnBind() = 0;
        virtual void Delete() = 0;
    };

    struct GuiImpl 
    {
        virtual ~GuiImpl() {}
        virtual void NewFrame() = 0;
        virtual void Render() = 0;
    };
}