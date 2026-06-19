#include "OpenGlManager.h"
#include "../../Camera.h"
#include "../../Log.h"

#include "../../PipelineConfig.h"

#include <array>

namespace VGF::Opengl
{
    OpenglTexture::OpenglTexture(std::string filePath)
    {
        int sus1 = glGetError();
        // generate textures
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

        unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

        int format;
        switch (nrChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGBA;
            break;
        }

        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D,
                0,
                GL_SRGB8_ALPHA8,
                width,
                height,
                0,
                format,
                GL_UNSIGNED_BYTE,
                data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else std::cout << "Failed to load texture" << std::endl;

        stbi_image_free(data);
    }
    OpenglTexture::OpenglTexture(const unsigned char* data, int format, int width, int height)
    {
        if (!data) {
            VGF::Log::Error("Failed to load texture image!");
        }

        std::vector<unsigned char> rgbaData(width * height * 4);

        if (format == 1)
        {
            for (int i = 0; i < width * height; i++)
            {
                rgbaData[i * 4 + 0] = data[i];
                rgbaData[i * 4 + 1] = data[i];
                rgbaData[i * 4 + 2] = data[i];
                rgbaData[i * 4 + 3] = data[i];
            }
        }
        else {
            memcpy(rgbaData.data(), data, width * height * 4);
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_SRGB8_ALPHA8,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            rgbaData.data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    OpenglTexture::~OpenglTexture()
    {
    }
    void OpenglTexture::Bind(textureType type)
    {
        int sus1 = glGetError();

        switch (type)
        {
        case VGF::color:
            glActiveTexture(GL_TEXTURE2);
            break;
        case VGF::metallicRoughness:
            glActiveTexture(GL_TEXTURE3);
            break;
        case VGF::emissive:
            glActiveTexture(GL_TEXTURE4);
            break;
        case VGF::occulsion:
            glActiveTexture(GL_TEXTURE5);
            break;
        case VGF::normal:
            glActiveTexture(GL_TEXTURE6);
            break;
        default:
            glActiveTexture(GL_TEXTURE2);
            break;
        }

        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void* OpenglTexture::GetNativeImage() { return (void*)texture; }

    Opengl::Opengl(GLFWwindow* window)
    {
        this->window = window;
        openglInstance = this;
    }
    Opengl::~Opengl()
    {
    }



    OpenglRenderer::OpenglRenderer(std::vector<GLuint>& indices, std::vector<GLfloat>& vertices, std::vector<UniformBufferObject*> uniformBuffers) : opengl(Opengl::openglInstance)
    {
        _indices = indices;
        _vertices = vertices;

        if (opengl->framebuffer == 0)
        { 
            glGenFramebuffers(1, &opengl->framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, opengl->framebuffer);

            int width, height;
            glfwGetWindowSize(Opengl::window, &width, &height);

            glGenTextures(1, &opengl->textureColorbuffer);
            glBindTexture(GL_TEXTURE_2D, opengl->textureColorbuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opengl->textureColorbuffer, 0);

            glGenRenderbuffers(1, &opengl->renderbufferObject);
            glBindRenderbuffer(GL_RENDERBUFFER, opengl->renderbufferObject);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, opengl->renderbufferObject); // now actually attach it

            GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, drawBuffers);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Framebuffer is not complete!\n";

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        for (size_t i = 0; i < uniformBuffers.size(); i++)
        {
            GLuint openglBuffer;
            glGenBuffers(1, &openglBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, openglBuffer);
            glBufferData(GL_UNIFORM_BUFFER, uniformBuffers[i]->SizeOf(), NULL, GL_STATIC_DRAW);
            glBindBufferRange(GL_UNIFORM_BUFFER, i, openglBuffer, 0, uniformBuffers[i]->SizeOf());

            openglUniformBuffers.push_back(openglBuffer);
        }

        indicesSize = indices.size();
    }
    OpenglRenderer::~OpenglRenderer()
    {
        for (auto buffer : openglUniformBuffers)
            glDeleteBuffers(1, &buffer);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void OpenglRenderer::Render(const PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers)
    {
        std::vector<UniformBufferObject*> instanceUniformBuffers;

        for (UniformBufferObject* buffer : uniformBuffers)
        {
            if (buffer->getType() == "matrix")
                instanceUniformBuffers.emplace_back(buffer);
        }

        BatchRender(config, uniformBuffers, instanceUniformBuffers);
    }


    void OpenglRenderer::BatchRender(const PipelineConfig& config, std::vector<UniformBufferObject*> onetimeUniformBuffers, std::vector<UniformBufferObject*> instanceUniformBuffers)
    {
        if (_createdVao == false) CreateVAO(config);

        int topology = GetTopology(config);

        instanceData.clear();

        bool matrix = false;
        for (UniformBufferObject* buffer : instanceUniformBuffers)
        {
            if (buffer->getType() == "matrix")
            {
                matrix = true;
                MatrixBufferObject* mBuffer = (MatrixBufferObject*)buffer;

                DefaultInstance data;
                data.model = mBuffer->GetData().model;
                data.textureID = 0;
                instanceData.emplace_back(data);
            }
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, IBO);

        if (lastSize != instanceData.size())
        {
            glBufferData(GL_ARRAY_BUFFER, instanceData.size() * config.GetInstanceBuffer()->GetSize(), instanceData.data(), GL_DYNAMIC_DRAW);
            lastSize = instanceData.size();

            if (!_configuredInstanceAttributes)
            {
                uint32_t location = lastLocation;
                config.GetInstanceBuffer()->GetOpenGLAttributeDescriptions(location);
                _configuredInstanceAttributes = true;
            }
        }
        else if (!instanceData.empty())
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * config.GetInstanceBuffer()->GetSize(), instanceData.data());
        }

        for (size_t i = 0; i < onetimeUniformBuffers.size(); i++)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, openglUniformBuffers[i]);
            glBindBufferBase(GL_UNIFORM_BUFFER, i, openglUniformBuffers[i]);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, onetimeUniformBuffers[i]->SizeOf(), onetimeUniformBuffers[i]->Data());
        }

        std::array<const char*, 5> samplers =
        {
            "colorSampler",
            "metallicRoughnessSampler",
            "emissiveSampler",
            "occulsionSampler",
            "normalSampler"
        };

        for (size_t binding = 0; binding < samplers.size(); binding++)
        {
            GLint location = glGetUniformLocation(config.ID(), samplers[binding]);
            glUniform1i(location, binding + 2);
        }
        glDrawElementsInstanced(topology, indicesSize, GL_UNSIGNED_INT, 0, instanceData.size());

        int error = glGetError();
    }

    const int OpenglRenderer::GetTopology(const PipelineConfig& config) const
    {
        int topology;

        switch (config.topology)
        {
        case VGF::Topology::LINE_LIST:
            topology = GL_LINES;
            break;
        case VGF::Topology::TRIANGLE_LIST:
            topology = GL_TRIANGLES;
            break;
        default:
            topology = GL_TRIANGLES;
            break;
        }

        return topology;
    }

    void OpenglRenderer::CreateVAO(const PipelineConfig& config)
    {
        glGenVertexArrays(1, &VAO);

        glGenBuffers(1, &VBO);
        glGenBuffers(1, &IBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // adds vertices and indices inside buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

        config.GetVertexBuffer()->GetOpenGLAttributeDescriptions(lastLocation);

        _createdVao = true;
    }
}
