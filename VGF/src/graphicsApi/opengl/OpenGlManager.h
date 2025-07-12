#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
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
        OpenglTexture(const char* filePath);
        OpenglTexture(const unsigned char* data, int format, int width, int height);
        ~OpenglTexture();  
        void Bind(textureType type) override;
    private:
        GLuint texture;
    };

    class Opengl : public ApiImpl
    {
    public:
        Opengl(GLFWwindow* window);
        ~Opengl();

        static GLuint UBO;

        static std::vector<std::pair<GLuint, GLuint>> vbos_ebos;
    private:
    };

    class OpenglRenderer : public RendererImpl
    {
    public:
        OpenglRenderer(std::vector<GLuint>& indices, std::vector<GLfloat>& vertices);
        ~OpenglRenderer();    

        virtual void Render(PipelineConfig& config, Camera* camera, glm::mat4 model, PBRbufferObject buffer) override; // Declare draw
    private:
        Opengl& opengl;

        int indicesSize;

        GLuint VAO;
	    GLuint VBO;
    	GLuint EBO;
    };
}
    