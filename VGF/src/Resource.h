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
		static void AddResourcePath(const std::string path) { _resourcePaths.push_back(path); }
	private:
		static inline std::vector<std::string> _resourcePaths =
		{
			"../../CubeCube/"
		};
	};
}