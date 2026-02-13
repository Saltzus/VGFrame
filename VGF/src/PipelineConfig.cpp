#include "PipelineConfig.h"
#include "Renderer.h"

#include "graphicsApi/opengl/OpenglShader.h"
#include "graphicsApi/vulkan/VulkanShader.h"

namespace VGF
{
	bool configCreated = false;

	PipelineConfig::PipelineConfig(std::string_view vertShader, std::string_view fragShader, Topology topology)
	{
		this->topology = topology;
		this->vertShader = vertShader;
		this->fragShader = fragShader;

		switch (Renderer::GetGraphicsApi())
		{
		case GraphicsApis::OpenGL:
			this->_impl = new VGF::Opengl::OpenglShader(vertShader, fragShader);
			break;
		case GraphicsApis::Vulkan:
			this->_impl = new VGF::Vulkan::VulkanShader(vertShader, fragShader);
			break;
		default:
			this->_impl = new VGF::Opengl::OpenglShader(vertShader, fragShader);
			break;
		}

		configCreated = true;
	}

	PipelineConfig::PipelineConfig() { configCreated = false; }

	PipelineConfig::~PipelineConfig()
	{
		if (configCreated)
			this->_impl->Delete();
	}

	void PipelineConfig::Activate()
	{
		this->_impl->Activate();
	}
	void PipelineConfig::Delete()
	{
		this->_impl->Delete();
	}
	unsigned int& PipelineConfig::ID()
	{
		return this->_impl->Id();
	}
}


