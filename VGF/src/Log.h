#pragma once
#include <string>

namespace VGF::Log
{
    void const Info(const std::string text);
    void const Warn(const std::string text);
    void const Error(const std::string text);
}
