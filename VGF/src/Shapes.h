#pragma once
#include <vector>

namespace VGF::Spapes
{
	struct Quad
	{
		inline static std::vector<float> quadVertices =
		{
	    //  (x, y, z)           (nx, ny, nz)        (r,g,b)             (u, v)
			0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // 0
			1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // 1
			1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // 2
			0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // 3
		};

		inline static std::vector<unsigned int> quadIndices =
		{
			0, 1, 2,
			2, 3, 0
		};
	};
}