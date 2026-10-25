#include "Text.h"
#include "Shapes.h"
#include "Log.h"

namespace VGF
{
    Text::Text()
    {
        if (!TextManager::Initialized()){
            Log::Error("TextManager not initialized before Text initialization!");
        }

        font = TextManager::GetDefaultFont();
        _renderer = new Renderer(Shader::GetDefault(), VGF::Spapes::Quad::quadIndices, VGF::Spapes::Quad::quadVertices);
    }
    
    Text::~Text()
    {
        delete _renderer;
    }

    RenderData remderData(Shader::GetDefault());

    void Text::Render(Shader& shader, const Window& window, const std::string tex)
    {
        std::string text = _effect.runEffect(tex);

        float xCursor = position.x;
        float yCursor = position.y;

        for (size_t ch = 0; ch < text.size(); ch++)
        {
            if (!font->HasCharacter(text[ch])) continue;

            if (text[ch] == '\n')
            {
                xCursor = position.x;
                yCursor -= (font->GetLineHeight() >> 6) * size;
                continue;
            }

            const Character chr = _effect.runPerCharacter(font->GetCharacter(text[ch]));;

            float xPos = xCursor + chr.bearing.x * size;
            float yPos = yCursor - (chr.size.y - chr.bearing.y) * size;

            float width = chr.size.x * size;
            float height = chr.size.y * size;

            glm::mat4 model = glm::translate(glm::mat4(1.f), { xPos, yPos, position.z });
            model = glm::scale(model, { width, height, 1.f });
  
            if (font->GetCharacter(text[ch])->texture != nullptr)
            {
                TextData data;
                data.model = model;
                data.proj = glm::ortho(0.f, (float)window.width, 0.f, (float)window.height, -1.f, 1.f);
                data.color = color;
                data.uvScale = glm::vec2(chr.size.x / chr.texture->GetWidth(), chr.size.y / chr.texture->GetHeight());

                _textBuffer.SetData((void*)&data);

                font->GetCharacter(text[ch])->texture->Bind();
                _renderer->Render(remderData, shader);
            }

            xCursor += (chr.advance >> 6) * size;
        }
    }

}