#include "OpenGlManager.h"
#include "../../Camera.h"

namespace VGF::Opengl
{
    OpenglTexture::OpenglTexture(const char* filePath)
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

        unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);

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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLint internalFmt = (format == 4 ? GL_SRGB8_ALPHA8 : format == 3 ? GL_SRGB8 : format);
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

    Opengl::Opengl(GLFWwindow* window)
    {
        this->window = window;
        openglInstance = this;
    }
    Opengl::~Opengl()
    {
    }

    // default vertices for post processing quad
    std::vector<GLfloat> defaultVertices =
    {
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f
    };

    // default indices for post processing quad
    std::vector<GLuint> defaultIndices =
    {
        0, 1, 2,
        2, 3, 0
    };

    OpenglPostProcess::OpenglPostProcess(bool renderToScreen, std::vector<UniformBufferObject*> uniformBuffers, PostProcessImpl* inProcess, std::vector<GLuint> inds, std::vector<GLfloat> verts) : opengl(Opengl::openglInstance)
    {
        this->renderToScreen = renderToScreen;
        this->inProcess = inProcess;

        postProcesses += 1;
        id = postProcesses;

        this->vertices = verts;
        this->indices = inds;

        if (indices.size() <= 0 || vertices.size() <= 0)
        {
            this->vertices = defaultVertices;
            this->indices = defaultIndices;
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

        glGenVertexArrays(1, &VAO);

        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        indicesSize = indices.size();

        // adds vertices and indices inside buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // vertex color
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // Texture position attribute
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
        glEnableVertexAttribArray(3);

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        postImages.push_back((void*)textureColorbuffer);

        int width, height;
        glfwGetWindowSize(Opengl::window, &width, &height);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    }
    OpenglPostProcess::~OpenglPostProcess()
    {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &textureColorbuffer);
        postProcesses -= 1;
    }

    void OpenglPostProcess::Render(PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers)
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

        assert(uniformBuffers.size() == openglUniformBuffers.size() && "sent uniform buffers did not match initialized buffers!!");
        for (size_t i = 0; i < uniformBuffers.size(); i++)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, openglUniformBuffers[i]);
            glBindBufferBase(GL_UNIFORM_BUFFER, i, openglUniformBuffers[i]);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, uniformBuffers[i]->SizeOf(), uniformBuffers[i]->Data());
        }

        GLint loc;
        loc = glGetUniformLocation(config.ID(), "colorSampler");
        glUniform1i(loc, 2);

        glActiveTexture(GL_TEXTURE2);

        if (!renderToScreen) 
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        if (inProcess != nullptr && inProcess->postImages.size() >= 1)
            glBindTexture(GL_TEXTURE_2D, (GLuint)inProcess->postImages[0]);
        else
            glBindTexture(GL_TEXTURE_2D, opengl->textureColorbuffer);

        glBindVertexArray(VAO);
        glDrawElements(topology, indicesSize, GL_UNSIGNED_INT, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    OpenglRenderer::OpenglRenderer(std::vector<GLuint>& indices, std::vector<GLfloat>& vertices, std::vector<UniformBufferObject*> uniformBuffers) : opengl(Opengl::openglInstance)
    {
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

        glGenVertexArrays(1, &VAO);

        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // adds vertices and indices inside buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // vertex color
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // Texture position attribute
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
        glEnableVertexAttribArray(3);

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

    void OpenglRenderer::Render(PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers)
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

        assert(uniformBuffers.size() == openglUniformBuffers.size() && "sent uniform buffers did not match initialized buffers!!");
        for (size_t i = 0; i < uniformBuffers.size(); i++)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, openglUniformBuffers[i]);
            glBindBufferBase(GL_UNIFORM_BUFFER, i, openglUniformBuffers[i]);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, uniformBuffers[i]->SizeOf(), uniformBuffers[i]->Data());
        }

        GLint loc;
        loc = glGetUniformLocation(config.ID(), "colorSampler");
        glUniform1i(loc, 2);
        loc = glGetUniformLocation(config.ID(), "metallicRoughnessSampler");
        glUniform1i(loc, 3);
        loc = glGetUniformLocation(config.ID(), "emissiveSampler");
        glUniform1i(loc, 4);
        loc = glGetUniformLocation(config.ID(), "occulsionSampler");
        glUniform1i(loc, 5);
        loc = glGetUniformLocation(config.ID(), "normalSampler");
        glUniform1i(loc, 6);

        // Draws the pixel
        glBindFramebuffer(GL_FRAMEBUFFER, opengl->framebuffer);

        glBindVertexArray(VAO);
        glDrawElements(topology, indicesSize, GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        int error = glGetError();
    }
}