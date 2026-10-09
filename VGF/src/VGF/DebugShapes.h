#pragma once

#include <glm/glm.hpp>
#include <box3d/box3d.h>

#include <variant>

struct MeshData
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

namespace DebugShapes
{
	class Shape
	{
	public:
		Shape(const b3DebugShape* shape);
		MeshData data;
	};
}