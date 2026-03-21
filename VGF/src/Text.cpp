#include "Text.h"

namespace VGF
{
    Text::Text()
    {
        if (!TextManager::Initialized()){
            Log::Error("TextManager not initialized before Text initialization!");
        }

        font = TextManager::GetDefaultFont();
        _renderer = new Renderer(_quadIndices, _quadVertices, {MatrixBufferObject::getDefault()});
    }
    
    Text::~Text()
    {
        delete _renderer;
    }

    void Text::Render(PipelineConfig& config, Camera* camera, std::string& text)
    {
        int xOffset = 0,yOffset = 0;

        glm::mat4 model = glm::scale(glm::mat4(1.f), { size,size,size });

        for (size_t i = 0; i < text.length(); i++)
        {
            xOffset++;

            if (!font->HasCharacter(text[i])) continue;
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

            font->GetCharacter(text[i])->texture->Bind();
            _renderer->Render(config, { &_matrixBuffer });
        }
    }

}