#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <map>

#include "Model.h"
#include "Texture.h"

namespace VGF
{
    class Text
    {

    public:
        Text();
        ~Text();
    private:
        Texture* texture;
        Renderer* renderer;

        FT_Library ft;

        const static inline std::vector<float> quadVertices =
        {
            // BL (x, y, z)               (nx, ny, nz)        (r,g,b)             (u, v)
            -0.5000f, 0, -0.5000f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // 0
             0.5000f, 0, -0.5000f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // 1
             0.5000f, 0,  0.5000f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // 2
            -0.5000f, 0,  0.5000f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // 3
         };

        const static inline std::vector<unsigned int> quadIndices =
        {
            0, 1, 2,
            2, 3, 0
        };
    };

} 
