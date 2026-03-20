#include "TextManager.h"

namespace VGF
{
	void TextManager::Init()
	{
		defaultTextConfig = new PipelineConfig
		(
			"../../CubeCube/Shaders/default_text.vert",
			"../../CubeCube/Shaders/default_text.frag",
			VGF::Topology::TRIANGLE_LIST
		);
	}

	void TextManager::Destroy()
	{
		delete defaultTextConfig;
	}
}