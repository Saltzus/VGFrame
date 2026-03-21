#pragma once

#include <utility>

#include "TextManager.h"
#include "Texture.h"

namespace VGF
{
    class Text
    {

    public:
        Text();
        ~Text();

        void Render(Camera* camera, std::string text) { Render(*TextManager::GetDefaultConfig(), camera, text); };
        void Render(PipelineConfig& config, Camera* camera, std::string& text);

        float size = 0.1f;
        Font* font;

    private:

        PipelineConfig _config;
        Renderer* _renderer;

        MatrixBufferObject _matrixBuffer;

        FT_Library ft;

        static inline std::vector<float> _quadVertices =
        {
            // BL (x, y, z)               (nx, ny, nz)        (r,g,b)             (u, v)
            -0.5000f, -0.5000f, 0,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // 0
             0.5000f, -0.5000f, 0,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // 1
             0.5000f,  0.5000f, 0,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // 2
            -0.5000f,  0.5000f, 0,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // 3
        };

        static inline std::vector<unsigned int> _quadIndices =
        {
            0, 1, 2,
            2, 3, 0
        };
    };

} 
