#pragma once

#include <box3d/box3d.h>
#include <vector>

class PhysicsDebugRenderer
{
public:
	PhysicsDebugRenderer();

	void Debug(b3WorldId worldId);

	std::vector<float>wireframeVertices;                                
	std::vector<unsigned int>wireframeIndices;

	std::vector<float> vertices;
	std::vector<unsigned int>indices;

private:
	//static void DebugDrawSegment(b3Pos p1, b3Pos p2, b3HexColor color, void* context);
	//static bool DebugDrawShape(void* userShape, b3WorldTransform transform, b3HexColor color, void* context);
	//static void DebugDrawBox(b3Vec3 extents, b3WorldTransform transform, b3HexColor color, void* context);
};
