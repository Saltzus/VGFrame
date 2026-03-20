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

    void Text::Render(PipelineConfig& config, Camera* camera)
    {
        glm::mat4 model = glm::scale(glm::mat4(1.f), { 0.5,0.5,0.5 });

        MatrixData data;
        data.model = model;
        data.proj = camera->projection;
        data.view = camera->view;
        _matrixBuffer.SetData((void*)&data);

        _characters['u'].texture->Bind();
        //Texture::GetWhiteTexture()->Bind();
        _renderer->Render(config, { &_matrixBuffer });

        data.model = glm::translate(model, { 1,0,0 });
        _matrixBuffer.SetData((void*)&data);
        _characters['m'].texture->Bind();
        _renderer->Render(config, { &_matrixBuffer });

        data.model = glm::translate(model, { -1,0,0 });
        _matrixBuffer.SetData((void*)&data);
        _characters['c'].texture->Bind();
        _renderer->Render(config, { &_matrixBuffer });
    }

}