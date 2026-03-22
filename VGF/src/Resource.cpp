#include "Resource.h"
#include "Log.h"

namespace VGF
{
	std::string Resource::Get(const std::string path)
	{
		for (auto location : _resourcePaths)
		{
			std::filesystem::path filePath(location + path);

			if (std::filesystem::is_regular_file(filePath)){
				return filePath.string();
			}
		}

		Log::Error("File not found in resource locations : " + path);
		return "";
	}
}
