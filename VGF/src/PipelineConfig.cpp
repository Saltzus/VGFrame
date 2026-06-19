#include "PipelineConfig.h"
#include "Renderer.h"

#include "graphicsApi/opengl/OpenglShader.h"
#include "graphicsApi/vulkan/VulkanShader.h"

namespace VGF
{
	bool configCreated = false;

	PipelineConfig::PipelineConfig(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent, VertexBuffer& vertexBuffer, VertexBuffer& instanceBuffer)
		: PipelineConfig(vertShader, fragShader, topology, translucent)
	{
		_createdInstanceBuffer = false;
		_createdVertexBuffer = false;

		_vertexBuffer = &vertexBuffer;
		_instanceBuffer = &instanceBuffer;
	}

	PipelineConfig::PipelineConfig(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent)
	{
		if (_createdInstanceBuffer == true)
		{
			_vertexBuffer = new DefaultVertexBuffer;
			_instanceBuffer = new DefaultInstanceBuffer;
		}

		this->translucent = translucent;
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

	PipelineConfig::PipelineConfig() 
	{ 
		configCreated = false; 
		_createdInstanceBuffer = false; 
		_createdVertexBuffer = false;
	}

	PipelineConfig::~PipelineConfig()
	{
		if (_createdInstanceBuffer) delete _instanceBuffer;
		if (_createdVertexBuffer) delete _vertexBuffer;
	}

	void PipelineConfig::SetVertexBuffer(VertexBuffer* vertexBuffer)
	{
		if (_createdVertexBuffer) delete _vertexBuffer;
		_createdVertexBuffer = false;
		_vertexBuffer = vertexBuffer;
	}

	void PipelineConfig::SetInstanceBuffer(VertexBuffer* instanceBuffer)
	{
		if (_createdInstanceBuffer) delete _instanceBuffer;
		_createdInstanceBuffer = false;
		_instanceBuffer = instanceBuffer;
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
	void PipelineConfig::Delete()
	{
		if (configCreated)
			this->_impl->Delete();
	}
	const unsigned int& PipelineConfig::ID() const
	{
		return this->_impl->Id();
	}
}


