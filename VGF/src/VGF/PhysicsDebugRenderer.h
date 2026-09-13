#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>

class PhysicsDebugRenderer : public JPH::DebugRenderer
{
public:
	PhysicsDebugRenderer();

	void SetCameraPos(JPH::RVec3Arg inCameraPos)
	{
		mCameraPos = inCameraPos;
		mCameraPosSet = true;
	}
	virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override 
	{
		float r = inColor.r / 255.0f;
		float g = inColor.g / 255.0f;
		float b = inColor.b / 255.0f;

		vertices.push_back((float)inV1.GetX()); vertices.push_back((float)inV1.GetY()); vertices.push_back((float)inV1.GetZ());
		vertices.push_back(r); vertices.push_back(g); vertices.push_back(b);

		vertices.push_back((float)inV2.GetX()); vertices.push_back((float)inV2.GetY()); vertices.push_back((float)inV2.GetZ());
		vertices.push_back(r); vertices.push_back(g); vertices.push_back(b);

		vertices.push_back((float)inV3.GetX()); vertices.push_back((float)inV3.GetY()); vertices.push_back((float)inV3.GetZ());
		vertices.push_back(r); vertices.push_back(g); vertices.push_back(b);

		unsigned int base = static_cast<unsigned int>(vertices.size()) / 6 - 3;
		indices.push_back(base);
		indices.push_back(base + 1);
		indices.push_back(base + 2);
	}
	virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override
	{
		float r = inColor.r / 255.0f;
		float g = inColor.g / 255.0f;
		float b = inColor.b / 255.0f;

		wireframeVertices.push_back((float)inFrom.GetX()); wireframeVertices.push_back((float)inFrom.GetY()); wireframeVertices.push_back((float)inFrom.GetZ());
		wireframeVertices.push_back(r); wireframeVertices.push_back(g); wireframeVertices.push_back(b);

		wireframeVertices.push_back((float)inTo.GetX()); wireframeVertices.push_back((float)inTo.GetY()); wireframeVertices.push_back((float)inTo.GetZ());
		wireframeVertices.push_back(r); wireframeVertices.push_back(g); wireframeVertices.push_back(b);

		unsigned int base = static_cast<unsigned int>(wireframeVertices.size()) / 6;
		wireframeIndices.push_back(base);
		wireframeIndices.push_back(base + 1);
	}
	virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor = JPH::Color::sWhite, float inHeight = 0.5f) override {}

	std::vector<float> wireframeVertices;
	std::vector<unsigned int>wireframeIndices;

	std::vector<float> vertices;
	std::vector<unsigned int>indices;

protected:
	virtual Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;
	virtual Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override;
	virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override;

private:
	class BatchImpl : public JPH::RefTargetVirtual
	{
	public:
		virtual void AddRef() override { ++mRefCount; }
		virtual void Release() override { if (--mRefCount == 0) delete this; }

		JPH::Array<Triangle> mTriangles;

	private:
		std::atomic<JPH::uint32> mRefCount = 0;
	};

	JPH::RVec3 mCameraPos;
	bool mCameraPosSet = false;
};
