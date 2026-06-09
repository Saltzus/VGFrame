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

		std::vector<float> outVertices =
		{
			(float)inV1.GetX(), (float)inV1.GetY(), (float)inV1.GetZ(),   0,0,1,   r,g,b,  0.5f, 0.0f,
			(float)inV2.GetX(), (float)inV2.GetY(), (float)inV2.GetZ(),   0,0,1,   r,g,b,  1.0f, 1.0f,
			(float)inV3.GetX(), (float)inV3.GetY(), (float)inV3.GetZ(),   0,0,1,   r,g,b,  0.0f, 1.0f,
		};

		unsigned int base = static_cast<unsigned int>(vertices.size()) / 11;
		vertices.insert(vertices.end(), outVertices.begin(), outVertices.end());
		indices.push_back(base);
		indices.push_back(base + 1);
		indices.push_back(base + 2);
	}
	virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override
	{
		float r = inColor.r / 255.0f;
		float g = inColor.g / 255.0f;
		float b = inColor.b / 255.0f;

		std::vector<float> outVertices =
		{
			(float)inFrom.GetX(), (float)inFrom.GetY(), (float)inFrom.GetZ(),   0,0,1,   r,g,b,  0.5f, 0.0f,
			(float)inTo.GetX()  , (float)inTo.GetY()  , (float)inTo.GetZ()  ,   0,0,1,   r,g,b,  1.0f, 1.0f,
		};

		unsigned int base = static_cast<unsigned int>(wireframeVertices.size()) / 11;
		wireframeVertices.insert(wireframeVertices.end(), outVertices.begin(), outVertices.end());
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
