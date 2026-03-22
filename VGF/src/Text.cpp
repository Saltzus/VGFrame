#include "Text.h"

namespace VGF
{
    Text::Text()
    {
        if (!TextManager::Initialized()){
            Log::Error("TextManager not initialized before Text initialization!");
        }

        font = TextManager::GetDefaultFont();
        _renderer = new Renderer(_quadIndices, _quadVertices, {TextBufferObject::getDefault()});
    }
    
    Text::~Text()
    {
        delete _renderer;
    }

    void Text::Render(PipelineConfig& config, const Window& window, std::string& text)
    {
        float xCursor = _position.x;
        float yCursor = _position.y;

        for (rsize_t ch = 0; ch < text.size(); ch++)
        {
            if (!font->HasCharacter(text[ch])){
                continue;
            }

            const Character* chr = font->GetCharacter(text[ch]);

            float xPos = xCursor + chr->bearing.x * size;
            float yPos = yCursor - (chr->size.y - chr->bearing.y) * size;

            float width = chr->size.x * size;
            float height = chr->size.y * size;

            glm::mat4 model = glm::translate(glm::mat4(1.f), { xPos, yPos,0 });
            model = glm::scale(model, { width, height, 1.f });
  
            if (font->GetCharacter(text[ch])->texture != nullptr)
            {
                TextData data;
                data.model = model;
                data.proj = glm::ortho(0.f, (float)window.width, 0.f, (float)window.height, -1.f, 1.f);
                data.uvScale = glm::vec2(chr->size.x / chr->texture->GetWidth(), chr->size.y / chr->texture->GetHeight());

                _textBuffer.SetData((void*)&data);

                font->GetCharacter(text[ch])->texture->Bind();
                _renderer->Render(config, { &_textBuffer });
            }

            xCursor += (chr->advance >> 6) * size;
        }
    }

}