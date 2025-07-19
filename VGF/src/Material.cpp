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

	void Material::Bind()
	{
		color->Bind(textureType::color);
		metallicRoughness->Bind(textureType::metallicRoughness);
		emissive->Bind(textureType::emissive);
		occulsion->Bind(textureType::occulsion);
		normal->Bind(textureType::normal);
	}

	std::vector<Texture*> Material::getVector()
	{
		std::vector<Texture*> textures;
		textures.push_back(color);
		textures.push_back(metallicRoughness);
		textures.push_back(emissive);
		textures.push_back(occulsion);
		textures.push_back(normal);

		return textures;
	}

	Material* Material::getDefaultMaterial()
	{
		defaultMaterial.color = Texture::GetDefaultTexture();
		defaultMaterial.metallicRoughness = Texture::GetBlackTexture();
		defaultMaterial.emissive = Texture::GetBlackTexture();
		defaultMaterial.occulsion = Texture::GetWhiteTexture();
		defaultMaterial.normal = Texture::GetBlueTexture();

		return &defaultMaterial;
	}
}
