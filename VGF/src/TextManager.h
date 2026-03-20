#pragma once

#include "PipelineConfig.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace VGF
{
	class TextManager
	{
	public:

		static void Init();
		static inline PipelineConfig* defaultTextConfig = nullptr;
		static void Destroy();

	private:

	};


} 
