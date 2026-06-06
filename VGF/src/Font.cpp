#include "Font.h"
#include "TextManager.h"
#include "Log.h"

namespace VGF
{
	Font::Font(const std::string path)
	{
        FT_Library& ft = TextManager::FTLib;

        FT_Face face;
        if (FT_New_Face(ft, path.c_str(), 0, &face)) {
            VGF::Log::Error("Failed to load font : " + path);
        }

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
                _characters.try_emplace(
                    chr,
                    nullptr,               // no texture
                    glm::ivec2(0, 0),
                    glm::ivec2(0, 0),
                    face->glyph->advance.x // advance is still valid — space needs this
                );
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

        _lineHeight = face->size->metrics.height;

        FT_Done_Face(face);
	}

	Font::~Font()
	{
	}
}