#pragma once

#include "Texture.h"

namespace VGF
{
	struct Character
	{
		Character(Texture* tex = nullptr, glm::ivec2 s = { 0,0 }, glm::ivec2 b = { 0,0 }, unsigned int adv = 0)
			: texture(tex), size(s), bearing(b), advance(adv) {
		}
		~Character() { delete texture; }

		Texture* texture;
		glm::ivec2 size;
		glm::ivec2 bearing;
		unsigned int advance;
	};

	class Font
	{
	public:
		Font(const std::string path);
		~Font();

		const Character* GetCharacter(const char character) { return &_characters.at(character); }
		const bool HasCharacter(const char character) { return _characters.find(character) != _characters.end(); }

	private:
		std::map<char, const Character> _characters;
	};
} 
