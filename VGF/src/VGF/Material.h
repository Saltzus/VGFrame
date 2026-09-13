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
		std::vector<const Texture*> getVector();

		static Material* getDefaultMaterial();
		static void setDefaultMaterial(Material material);

		const Texture* color;
		const Texture* metallicRoughness;
		const Texture* emissive;
		const Texture* occulsion;
		const Texture* normal;



	private:
	};
} 