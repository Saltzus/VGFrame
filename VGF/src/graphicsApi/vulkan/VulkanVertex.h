#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>


namespace VGF::Vulkan
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 texCoord;
    };

    static std::vector<Vertex> fromGLFloats(const std::vector<GLfloat>& vertexData) {
        assert(vertexData.size() % 11 == 0 && "vertexData size must be divisible by 11");
        std::vector<Vertex> vertices;
        vertices.reserve(vertexData.size() / 11);

        for (size_t i = 0; i < vertexData.size(); i += 11) {
            Vertex vertex;
            vertex.position = glm::vec3(vertexData[i], vertexData[i + 1], vertexData[i + 2]);
            vertex.normal = glm::vec3(vertexData[i + 3], vertexData[i + 4], vertexData[i + 5]);
            vertex.color = glm::vec3(vertexData[i + 6], vertexData[i + 7], vertexData[i + 8]);
            vertex.texCoord = glm::vec2(vertexData[i + 9], vertexData[i + 10]);
            vertices.push_back(vertex);
        }
        return vertices;
    }
}