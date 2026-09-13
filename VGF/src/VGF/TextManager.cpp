#include "TextManager.h"
#include "Resource.h"
#include "Log.h"

namespace VGF
{
	void TextManager::Init()
	{
		if (FT_Init_FreeType(&FTLib))
			VGF::Log::Error("Could not init FreeType Library");

		_fonts.try_emplace
		(
			"default",
			Resource::Get("Fonts/Gabarito.ttf")
		);

		_defaultTextConfig = new PipelineConfig
		(
			Resource::Get("Shaders/default_text.vert"),
			Resource::Get("Shaders/default_text.frag"),
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