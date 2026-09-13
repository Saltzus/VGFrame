#pragma once

#include "PipelineConfig.h"
#include "Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace VGF
{
	class TextManager
	{
	public:

		static void Init();
		static void Destroy();

		static Font* LoadFont(const std::string name, const std::string filepath) { _fonts.try_emplace(name, filepath); return &_fonts.at(name); }
		static Font* GetFont(const std::string name) { return &_fonts.at(name); }


		static Font* GetDefaultFont() { return &_fonts.at("default"); }
		static PipelineConfig* GetDefaultConfig() { return _defaultTextConfig; }

		static inline FT_Library FTLib;

		static bool Initialized() { return _initialized; }

	private:
		static inline std::map<std::string, Font> _fonts;
		
		static inline bool _initialized = false;
		static inline PipelineConfig* _defaultTextConfig = nullptr;
	};
} 
