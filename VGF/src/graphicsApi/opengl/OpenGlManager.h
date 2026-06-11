#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "../../RenderImpl.h"

namespace VGF::Opengl
{
    // Structure to standardize the vertices used in the sprites
    struct Vertex
    {
    	glm::vec3 position;
    	glm::vec2 texUV;
    };
    
    class OpenglTexture : public TextureImpl
    {
    public:
        OpenglTexture(std::string filePath);
        OpenglTexture(const unsigned char* data, int format, int width, int height);
        ~OpenglTexture();  
        virtual void Bind(textureType type) override;
        virtual void* GetNativeImage() override;
    private:
        GLuint texture;
    };

    class Opengl : public ApiImpl
    {
    public:
        Opengl(GLFWwindow* window);
        ~Opengl();


        inline static Opengl* openglInstance;
        inline static GLFWwindow* window;
        inline static unsigned int textureColorbuffer;

        inline static GLuint framebuffer = 0;
        inline static GLuint renderbufferObject;
    private:
    };

    class OpenglRenderer : public RendererImpl
    {
    public:
        OpenglRenderer(std::vector<GLuint>& indices, std::vector<GLfloat>& vertices, std::vector<UniformBufferObject*> uniformBuffers);
        ~OpenglRenderer();    

        virtual void Render(const PipelineConfig& config, std::vector<UniformBufferObject*> uniformBuffers) override; // Declare draw
        virtual void BatchRender(const PipelineConfig& config, std::vector<UniformBufferObject*> onetimeUniformBuffers, std::vector<UniformBufferObject*> instanceUniformBuffers) override;
    private:

        Opengl* opengl;
        std::vector<GLuint> openglUniformBuffers;

        int indicesSize;

        GLuint VAO;
	    GLuint VBO;
    	GLuint EBO;
    };
}
    