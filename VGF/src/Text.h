#pragma once

#include <utility>

#include "TextManager.h"
#include "Texture.h"

#include "UniformBuffers/TextBuffer.h"

namespace VGF
{
    class Text
    {

    public:
        Text();
        ~Text();

        void Render(const Window& window, std::string text) { Render(*TextManager::GetDefaultConfig(), window, text); };
        void Render(PipelineConfig& config, const Window& window, std::string& text);

        float size = 0.1f;
        Font* font;

    private:

        glm::vec3 _position = {10,15,0};

        PipelineConfig _config;
        Renderer* _renderer;

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
