#pragma once

#include "Texture.h"

#include <map>

namespace VGF
{
	struct Character
	{
		Character(Texture* tex = nullptr, glm::ivec2 s = { 0,0 }, glm::ivec2 b = { 0,0 }, unsigned int adv = 0, const bool ownsTexture = true)
			: texture(tex), size(s), bearing(b), advance(adv), ownsTexture(ownsTexture) {
		}
		~Character() { if (ownsTexture) delete texture; }

		const Texture* texture;
		glm::ivec2 size;
		glm::ivec2 bearing;
		unsigned int advance;
		const bool ownsTexture;
	};

	class Font
	{
	public:
		Font(const std::string path);
		~Font();

		const Character* GetCharacter(const char character) { return &_characters.at(character); }
		const bool HasCharacter(const char character) { return _characters.find(character) != _characters.end(); }
		const unsigned int GetLineHeight() const { return _lineHeight;  }

	private:
		std::map<char, const Character> _characters;
		unsigned int _lineHeight;
	};
} 
