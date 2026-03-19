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
        if (FT_New_Face(ft, "Game/Resources/Fonts/Testi.ttf", 0, &face))
        {
            std::cout << "[ERROR] Failed to load font" << std::endl;
        }

        FT_Set_Pixel_Sizes(face, 0, 48);
        if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        {
            std::cout << "[ERROR] Failed to load Glyph" << std::endl;
        }

        for (unsigned char c = 0; c < 128; c++)
        {
            // load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "[ERROR] Failed to load Glyph" << std::endl;
                continue;
            }

            // generate texture
            texture = new Texture(face->glyph->bitmap.buffer, 1, face->glyph->bitmap.width, face->glyph->bitmap.rows);
            renderer = new Renderer(quadIndices,quadVertices, {MatrixBufferObject::getDefault()});

            MatrixData data;
            data.model = glm::mat4(1.f);
            data.proj = camera->projection;
            data.view = camera->view;
            Model::matrixBuffer.SetData((void*)&data);

            debug->indices.clear();
            debug->vertices.clear();

            dynamicsWorld->debugDrawWorld();

            lines = new VGF::Renderer(debug->indices, debug->vertices, );
            lines->Render(config, { &matrixBuffer });
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
    
    Text::~Text()
    {
        delete texture;
    }
}