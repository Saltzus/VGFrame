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

        static VkVertexInputBindingDescription getBindingDescription() 
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() 
        {
            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, normal);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, color);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }
    };

    static std::vector<Vertex> fromGLFloats(const std::vector<GLfloat>& vertexData) {
        std::vector<Vertex> vertices;
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