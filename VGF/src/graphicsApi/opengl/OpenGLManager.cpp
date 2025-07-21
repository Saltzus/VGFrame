#include "OpenGlManager.h"
#include "../../Camera.h"

namespace VGF::Opengl
{
    OpenglTexture::OpenglTexture(const char* filePath)
    {
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


    GLuint Opengl::UBO = 0;
    GLuint Opengl::PBRUBO = 0;
    GLuint Opengl::LIGHTUBO = 0;

    Opengl::Opengl(GLFWwindow* window)
    {
        glGenBuffers(1, &UBO);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, NULL, GL_STATIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 3 * sizeof(glm::mat4));

        glGenBuffers(1, &PBRUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, PBRUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(PBRbufferObject), NULL, GL_STATIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, PBRUBO, 0, sizeof(PBRbufferObject));

        glGenBuffers(1, &LIGHTUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, LIGHTUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBufferObject), NULL, GL_STATIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 7, LIGHTUBO, 0, sizeof(LightBufferObject));
    }
    Opengl::~Opengl()
    {
        glDeleteBuffers(1, &UBO);
        glDeleteBuffers(1, &PBRUBO);
        glDeleteBuffers(1, &LIGHTUBO);
    }
    int d = 0;
    OpenglRenderer::OpenglRenderer(std::vector<GLuint>& indices, std::vector<GLfloat>& vertices) : opengl(opengl)
    {
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
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    int i = 0;

    void OpenglRenderer::Render(PipelineConfig& config, Camera* camera, glm::mat4 model, PBRbufferObject buffer, LightBufferObject lightBuffer)
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

        glm::mat4 matrices[3];
        matrices[0] = model;
        matrices[1] = camera->view;
        matrices[2] = camera->projection;

        GLuint blockIndex = glGetUniformBlockIndex(config.ID(), "UniformBufferObject");
        glUniformBlockBinding(config.ID(), blockIndex, 0);

        GLuint blockIndexPBR = glGetUniformBlockIndex(config.ID(), "PBRbufferObject");
        glUniformBlockBinding(config.ID(), blockIndexPBR, 1);

        // Update the UBO with matrix data
        glBindBuffer(GL_UNIFORM_BUFFER, Opengl::UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 3, &matrices[0]);

        glBindBuffer(GL_UNIFORM_BUFFER, Opengl::PBRUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PBRbufferObject), &buffer);

        glBindBuffer(GL_UNIFORM_BUFFER, Opengl::LIGHTUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBufferObject), &lightBuffer);

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
        glBindVertexArray(VAO);

        glDrawElements(topology, indicesSize, GL_UNSIGNED_INT, 0);
    }
}