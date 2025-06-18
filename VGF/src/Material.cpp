#include "Material.h"

namespace VGF
{
	Material Material::defaultMaterial;

	Material::Material()
	{
	}

	Material::~Material()
	{
	}

	Material* Material::getDefaultMaterial()
	{
		defaultMaterial.color = Texture::GetDefaultTexture();
		return &defaultMaterial;
	}
}
