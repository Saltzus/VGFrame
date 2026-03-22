#include "TextManager.h"

namespace VGF
{
	void TextManager::Init()
	{
		if (FT_Init_FreeType(&FTLib))
			VGF::Log::Error("Could not init FreeType Library");

		_fonts.try_emplace
		(
			"default",
			"../../CubeCube/Fonts/Gabarito.ttf"
		);

		_defaultTextConfig = new PipelineConfig
		(
			"../../CubeCube/Shaders/default_text.vert",
			"../../CubeCube/Shaders/default_text.frag",
			VGF::Topology::TRIANGLE_LIST
		);

		_initialized = true;
	}

	void TextManager::Destroy()
	{
		delete _defaultTextConfig;
		FT_Done_FreeType(FTLib);
	}

}