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
        glm::vec3 position = { 10,15,0 };


        Font* font;

    private:


        PipelineConfig _config;
        Renderer* _renderer;

        TextEffect _effect;
        bool _ownEffect = true;

        TextBufferObject _textBuffer;

        FT_Library ft;

        static inline std::vector<float> _quadVertices =
        {
            // BL (x, y, z)               (nx, ny, nz)        (r,g,b)             (u, v)
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // 0
            1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // 1
            1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // 2
            0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // 3
        };

        static inline std::vector<unsigned int> _quadIndices =
        {
            0, 1, 2,
            2, 3, 0
        };
    };
}
