#include "Log.h"
#include <iostream>

namespace VGF::Log
{
    void const Info(const std::string text)
    {
        std::cout << "\x1B[36m[INFO]\033[0m\t" << text << "\n";
    }
    void const Warn(const std::string text)
    {
        std::cout << "\x1B[33m[WARNING]\033[0m\t" << text << "\n";
    }
    void const Error(const std::string text)
    {
        std::cout << "\x1B[31m[ERROR]\033[0m\t" << text << "\n";
        throw std::runtime_error(text);
    }
}
