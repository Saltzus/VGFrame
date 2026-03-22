#pragma once

#include <functional>


#include "Font.h"
#include "VGFTime.h"

namespace VGF
{
	class TextEffect
	{
	public:
		TextEffect() {}
		~TextEffect() {}

		std::function<Character(const Character*)> runPerCharacter = [](const Character* c) -> Character {
			return Character(const_cast<Texture*>(c->texture), c->size, c->bearing, c->advance, false);
		};

		std::function<std::string(const std::string&)> runEffect = [](const std::string& text) -> std::string {
			return text;
		};

		static TextEffect Typewriter(double interval = 0.25);
		

	private:

	};
} 
