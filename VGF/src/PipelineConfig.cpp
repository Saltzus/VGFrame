#include "PipelineConfig.h"
#include "Renderer.h"

#include "graphicsApi/opengl/OpenglShader.h"
#include "graphicsApi/vulkan/VulkanShader.h"

namespace VGF
{
	PipelineConfig::PipelineConfig(const char* vertShader, const char* fragShader, Topology topology)
	{
		this->topology = topology;
		this->vertShader = vertShader;
		this->fragShader = fragShader;

		switch (Renderer::GetGraphicsApi())
		{
		case GraphicsApis::OpenGL:
			this->impl = new VGF::Opengl::OpenglShader(vertShader, fragShader);
			break;
		case GraphicsApis::Vulkan:
			this->impl = new VGF::Vulkan::VulkanShader(vertShader, fragShader);
			break;
		default:
			this->impl = new VGF::Opengl::OpenglShader(vertShader, fragShader);
			break;
		}
	}

	PipelineConfig::PipelineConfig() {}

	PipelineConfig::~PipelineConfig()
	{

	}

	void PipelineConfig::Activate()
	{
		this->impl->Activate();
	}
	void PipelineConfig::Delete()
	{
		this->impl->Delete();
	}
	unsigned int& PipelineConfig::ID()
	{
		return this->impl->Id();
	}
}


