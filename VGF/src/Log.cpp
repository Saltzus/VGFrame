#include "Log.h"
#include <iostream>

namespace VGF::Log
{
    void const Info(const std::string text)
    {
        std::cout << "[INFO] " << text << "\n";
    }
    void const Warn(const std::string text)
    {
        std::cout << "[WARNING] " << text << "\n";
    }
    void const Error(const std::string text)
    {
        std::cout << "[ERROR] " << text << "\n";
        throw std::runtime_error(text);
    }
}
