#include "Text.h"

namespace VGF
{
    Text::Text()
    {
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "[ERROR] Could not init FreeType Library" << std::endl;
        }

        FT_Face face;
        if (FT_New_Face(ft, "../../CubeCube/Fonts/OpenSans.ttf", 0, &face))
        {
            std::cout << "[ERROR] Failed to load font" << std::endl;
        }

        FT_Set_Pixel_Sizes(face, 0, 248);

        for (unsigned char c = 0; c < 128; c++)
        {
            // load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "[ERROR] Failed to load Glyph" << std::endl;
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
                c,
                new Texture(face->glyph->bitmap.buffer, 1, face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            );

            _renderer = new Renderer(_quadIndices, _quadVertices, {MatrixBufferObject::getDefault()});
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
    
    Text::~Text()
    {
        delete _texture;
    }

    void Text::Render(PipelineConfig& config, Camera* camera, std::string& text)
    {
        int xOffset = 0,yOffset = 0;

        glm::mat4 model = glm::scale(glm::mat4(1.f), { size,size,size });
        for (size_t i = 0; i < text.length(); i++)
        {
            xOffset++;

            if (_characters.find(text[i]) == _characters.end()) continue;
            if (text[i] == '/' && text[i + 1] == 'n')
            {
                xOffset = 0;
                yOffset++;
                i += 1;
                continue;
            }

            MatrixData data;
            data.model = glm::translate(model, { static_cast<float>(1 * xOffset), yOffset,0 });
            _matrixBuffer.SetData((void*)&data);

            _characters[text[i]].texture->Bind();
            _renderer->Render(config, { &_matrixBuffer });
        }
    }

}