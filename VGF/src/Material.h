#pragma once

#include "Texture.h"

namespace VGF
{
	class Material
	{
	public:
		Material();
		~Material();

		void Bind();
		std::vector<Texture*> getVector();

		static Material* getDefaultMaterial();
		static void setDefaultMaterial(Material material);

		Texture* color;
		Texture* metallicRoughness;
		Texture* emissive;
		Texture* occulsion;
		Texture* normal;



	private:
	};
} 