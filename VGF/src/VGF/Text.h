#pragma once

#include <utility>

#include "TextManager.h"
#include "Texture.h"
#include "TextEffect.h"

#include "UniformBuffers/TextBuffer.h"

namespace VGF
{
    class Text
    {

    public:
        Text();
        Text(TextEffect effect) : Text() { _effect = effect; }
        ~Text();

        void Render(const Window& window, const std::string text) { Render(*TextManager::GetDefaultConfig(), window, text); };
        void Render(PipelineConfig& config, const Window& window, const std::string text);

        void SetEffect(TextEffect effect) { _effect = effect; }

        float size = 0.1f;
        glm::vec3 position = { 10,-150,0 };
        glm::vec4 color = { 1,1,1,1 };


        Font* font;

    private:
        Renderer* _renderer;

        TextEffect _effect;
        bool _ownEffect = true;

        TextBufferObject _textBuffer;

        FT_Library ft;
    };
}
