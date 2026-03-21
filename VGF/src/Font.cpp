#include "Font.h"
#include "TextManager.h"

namespace VGF
{
	Font::Font(const std::string path)
	{
        FT_Library& ft = TextManager::FTLib;

        FT_Face face;
        if (FT_New_Face(ft, "../../CubeCube/Fonts/OpenSans.ttf", 0, &face))
            VGF::Log::Error("Failed to load font : ../../CubeCube/Fonts/OpenSans.ttf");

        FT_Set_Pixel_Sizes(face, 0, 248);

        for (unsigned char chr = 0; chr < 128; chr++)
        {
            if (FT_Load_Char(face, chr, FT_LOAD_RENDER))
            {
                VGF::Log::Error("Failed to load Glyph : " + (char)chr);
                continue;
            }

            if (face->glyph->bitmap.buffer == nullptr ||
                face->glyph->bitmap.width == 0 ||
                face->glyph->bitmap.rows == 0)
            {
                continue;
            }

            _characters.try_emplace
            (
                chr,
                new Texture(face->glyph->bitmap.buffer, 1, face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            );
        }

        FT_Done_Face(face);
	}

	Font::~Font()
	{
	}
}