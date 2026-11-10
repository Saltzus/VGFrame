#include "Resource.h"
#include "Log.h"

#include <whereami.h>

namespace VGF
{
	std::vector<std::string>& Resource::GetResourcePaths()
	{
		static std::vector<std::string> paths = 
		{ 
			"../VGF/",
			"VGF/",
			"../../../../VGFrame/VGF/",
			"../Resources/",
			"Resources/",
			"../../../../Multplr/Client/Resources/",
			"./"
		};

		return paths;
	}


	std::string Resource::Get(const std::string path)
	{
		for (auto location : GetResourcePaths())
		{
			std::filesystem::path filePath((GetExePath() + std::string("/") + location + path));

			if (std::filesystem::is_regular_file(filePath)){
				return filePath.string();
			}
		}

		Log::Error("File not found in resource locations : " + path);
		return "";
	}

	char* Resource::GetExePath()
	{
		static bool gotExePath = false;
		static char* exePath = NULL;

		if (gotExePath) return exePath;

		int length, dirname_length;
		length = wai_getExecutablePath(NULL, 0, &dirname_length);
		if (length > 0)
		{
			exePath = (char*)malloc(length + 1);
			if (!exePath) Log::Error("Could not get exe path");
			wai_getExecutablePath(exePath, length, &dirname_length);
			
			//exePath[length] = '\0';
			exePath[dirname_length] = '\0';
		}

		return exePath;
	}
}
