#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <vector>

#include "Renderer.h"

class ShaderImpl;

std::string get_file_contents(const char* filename);

namespace VGF
{
    class Shader
    {
    private:
        ShaderImpl* _impl = nullptr;
    public:

        const char* vertShader = "Resources/Shaders/default.vert.spv";
        const char* fragShader = "Resources/Shaders/default.frag.spv";

        Shader(const char* vertexFile, const char* fragmentFile);
        ~Shader();
        void Activate();
        void Delete();
        unsigned int& ID();
    };
}