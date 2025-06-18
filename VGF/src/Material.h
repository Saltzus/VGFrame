#pragma once

#include "Texture.h"

namespace VGF
{
	class Material
	{
	public:
		Material();
		~Material();

		static Material* getDefaultMaterial();

		Texture* color;
		Texture* normal;
		Texture* roughness;

	private:
		static Material defaultMaterial;
	};
} 