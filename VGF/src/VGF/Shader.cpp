#include "Shader.h"
#include "Renderer.h"

#include "graphicsApi/opengl/OpenglShader.h"
#include "graphicsApi/vulkan/VulkanShader.h"

#include "slang.h"
#include "slang-com-ptr.h"
#include "slang-com-helper.h"

namespace VGF
{
	Shader::Shader(std::string_view vertShader, std::string_view fragShader, Topology topology, bool translucent)
	{
		reflect = ShaderCompiler::LoadShader(*this,std::string(vertShader), std::string(fragShader));
		 
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

	Shader::~Shader()
	= default;

	void Shader::SetVertexBuffer(VertexBuffer* vertexBuffer)
	{
		_vertexBuffer = std::shared_ptr<VertexBuffer>(vertexBuffer);
	}

	void Shader::SetInstanceBuffer(VertexBuffer* instanceBuffer)
	{
		_instanceBuffer = std::shared_ptr<VertexBuffer>(instanceBuffer);
	}



	const VkPrimitiveTopology Shader::GetVulkanTopology() const
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


	void Shader::Bind() const
	{
		this->_impl->Bind();
	}

	const unsigned int& Shader::ID() const
	{
		return this->_impl->Id();
	}
}


