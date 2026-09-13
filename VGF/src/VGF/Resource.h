#pragma once
#include <string>
#include <vector>

#include <filesystem>

namespace VGF 
{
	class Resource
	{
	public:
		static std::string Get(const std::string path);
		static void AddResourcePath(const std::string path) { GetResourcePaths().push_back(path); }
	private:
		static std::vector<std::string>& GetResourcePaths();
	};
}