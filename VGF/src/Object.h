#pragma once

#include "Renderer.h"
#include "Texture.h"

namespace VGF
{
    class Object
    {
    public:
        Object();
        ~Object();

        virtual void Render(PipelineConfig config, Camera* camera, glm::mat4 model = glm::mat4(1.0f));

		Texture* texture;  
        PipelineConfig pipelineConfig;

        glm::mat4 model = glm::mat4(1.0f);
	    glm::vec3 translation = glm::vec3(0.f, 0.f, 0.f);
	    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	    glm::vec3 scale = glm::vec3(0.7f, 0.7f, 0.7f);

        std::vector<GLfloat> vertices = {
            // Positions            // Colors         // Texture Coords
            // Front face (Z = -0.5)
            -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // 0
             0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // 1
             0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // 2
            -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  // 3

            // Back face (Z = +0.5)
            -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // 4
             0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // 5
             0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // 6
            -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  // 7

            // Left face (X = -0.5)
            -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // 8
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // 9
            -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // 10
            -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // 11

            // Right face (X = +0.5)
             0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // 12
             0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // 13
             0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // 14
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  // 15

             // Top face (Y = +0.5)
             -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // 16
              0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // 17
              0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // 18
             -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  // 19

             // Bottom face (Y = -0.5)
             -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // 20
              0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // 21
              0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // 22
             -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f   // 23
        };

        std::vector<GLuint> indices = {
            // Front face
            0,  1,  3,
            1,  2,  3,

            // Back face
            4,  5,  7,
            5,  6,  7,

            // Left face
            8,  9,  11,
            9,  10, 11,

            // Right face
            12, 13, 15,
            13, 14, 15,

            // Top face
            16, 17, 19,
            17, 18, 19,

            // Bottom face
            20, 21, 23,
            21, 22, 23
        };

    protected:
        Renderer* renderer;

    private:
       

    };
    
} 
