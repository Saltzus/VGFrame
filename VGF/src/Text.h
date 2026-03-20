#pragma once

#include "TextManager.h"
#include "Texture.h"

namespace VGF
{
    class Text
    {

    public:
        Text();
        ~Text();

        void Render(Camera* camera) { Render(*TextManager::defaultTextConfig, camera); };
        void Render(PipelineConfig& config, Camera* camera);

    private:

        struct Character 
        {
            Character(Texture* tex = nullptr, glm::ivec2 s = {0,0}, glm::ivec2 b = { 0,0 }, unsigned int adv = 0)
                : texture(tex), size(s), bearing(b), advance(adv) {}
            ~Character() { delete texture; }

            Texture* texture;
            glm::ivec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
        };

        std::map<char, Character> _characters;

        PipelineConfig _config;
        Texture* _texture;
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
