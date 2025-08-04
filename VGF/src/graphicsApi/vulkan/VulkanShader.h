#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <vector>

#include "../../RenderImpl.h"

std::string get_file_contents(std::string_view filename);

namespace VGF::Vulkan
{
    class VulkanShader : public ShaderImpl
    {
    public:
        VulkanShader(std::string_view vertexFile, std::string_view fragmentFile);

        void Activate() override;
        void Delete() override;
        unsigned int& Id() override { return ID; };

    private:
        unsigned int ID;
        void compileErrors(unsigned int shader, const char* type);
    };
}