#include "Shader.h"

#include "graphicsApi/opengl/OpenglShader.h"
#include "graphicsApi/vulkan/VulkanShader.h"

namespace VGF
{
    Shader::Shader(const char* vertexFile, const char* fragmentFile) 
    {
        vertShader = vertexFile;
        fragShader = fragmentFile;

        switch (Renderer::GetGraphicsApi())
        {
        case GraphicsApis::OpenGL:
            this->_impl = new VGF::Opengl::OpenglShader(vertexFile, fragmentFile);
            break;
        case GraphicsApis::Vulkan:
            this->_impl = new VGF::Vulkan::VulkanShader(vertexFile, fragmentFile);
            break;
        default:
            this->_impl = new VGF::Opengl::OpenglShader(vertexFile, fragmentFile);
            break;
        }
    }
    Shader::~Shader()
    {
        delete this->_impl;
    }

    void Shader::Activate()
    {
        this->_impl->Activate();
    }
    void Shader::Delete()
    {
        this->_impl->Delete();
    }
    unsigned int& Shader::ID()
    {
        return this->_impl->Id();
    }
}
