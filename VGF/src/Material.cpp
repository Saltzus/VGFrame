#include "Material.h"

namespace VGF
{
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

	std::vector<const Texture*> Material::getVector()
	{
		std::vector<const Texture*> textures;
		textures.push_back(color);
		textures.push_back(metallicRoughness);
		textures.push_back(emissive);
		textures.push_back(occulsion);
		textures.push_back(normal);

		return textures;
	}

	Material* Material::getDefaultMaterial()
	{
		static Material defaultMaterial = []()
			{
				Material material;
				material.color = Texture::GetWhiteTexture();
				material.metallicRoughness = Texture::GetBlackTexture();
				material.emissive = Texture::GetBlackTexture();
				material.occulsion = Texture::GetWhiteTexture();
				material.normal = Texture::GetBlueTexture();
				return material;
			}();
		
		return &defaultMaterial;
	}

	void Material::setDefaultMaterial(Material material)
	{
		*getDefaultMaterial() = material;
		material.Bind();
	}

}
