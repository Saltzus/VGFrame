#include "PipelineConfig.h"
#include "Renderer.h"

#include "graphicsApi/opengl/OpenglShader.h"
#include "graphicsApi/vulkan/VulkanShader.h"

namespace VGF
{
	PipelineConfig::PipelineConfig(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent)
	{
		if (!_vertexBuffer) _vertexBuffer = std::make_shared<DefaultVertexBuffer>();
		if (!_instanceBuffer) _instanceBuffer = std::make_shared<DefaultInstanceBuffer>();

		this->translucent = translucent;
		this->topology = topology;
		this->vertShader = vertShader;
		this->fragShader = fragShader;

		switch (Renderer::GetGraphicsApi())
		{
		case GraphicsApis::OpenGL:
			this->_impl = std::make_shared<VGF::Opengl::OpenglShader>(vertShader, fragShader);
			break;
		case GraphicsApis::Vulkan:
			this->_impl = std::make_shared<VGF::Vulkan::VulkanShader>(vertShader, fragShader);
			break;
		default:
			this->_impl = std::make_shared<VGF::Opengl::OpenglShader>(vertShader, fragShader);
			break;
		}

	}

	PipelineConfig::~PipelineConfig()
	= default;

	void PipelineConfig::SetVertexBuffer(VertexBuffer* vertexBuffer)
	{
		_vertexBuffer = std::shared_ptr<VertexBuffer>(vertexBuffer);
	}

	void PipelineConfig::SetInstanceBuffer(VertexBuffer* instanceBuffer)
	{
		_instanceBuffer = std::shared_ptr<VertexBuffer>(instanceBuffer);
	}



	const VkPrimitiveTopology PipelineConfig::GetVulkanTopology() const
	{
		switch (topology)
		{
		case VGF::Topology::LINE_LIST:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			break;
		case VGF::Topology::TRIANGLE_LIST:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			break;
		default:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			break;
		}
	}


	void PipelineConfig::Activate() const
	{
		this->_impl->Activate();
	}

	const unsigned int& PipelineConfig::ID() const
	{
		return this->_impl->Id();
	}
}


