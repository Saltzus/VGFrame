#include "PhysicsDebugRenderer.h"

#include "Physics.h"

PhysicsDebugRenderer::PhysicsDebugRenderer()
{
	Initialize();
}

JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount)
{
	BatchImpl* batch = new BatchImpl;
	if (inTriangles == nullptr || inTriangleCount == 0)
		return batch;

	batch->mTriangles.assign(inTriangles, inTriangles + inTriangleCount);
	return batch;
}

JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount)
{
	BatchImpl* batch = new BatchImpl;
	if (inVertices == nullptr || inVertexCount == 0 || inIndices == nullptr || inIndexCount == 0)
		return batch;

	// Convert indexed triangle list to triangle list
	batch->mTriangles.resize(inIndexCount / 3);
	for (size_t t = 0; t < batch->mTriangles.size(); ++t)
	{
		Triangle& triangle = batch->mTriangles[t];
		triangle.mV[0] = inVertices[inIndices[t * 3 + 0]];
		triangle.mV[1] = inVertices[inIndices[t * 3 + 1]];
		triangle.mV[2] = inVertices[inIndices[t * 3 + 2]];
	}

	return batch;
}

void PhysicsDebugRenderer::DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode)
{
	// Figure out which LOD to use
	const LOD* lod = inGeometry->mLODs.data();
	if (mCameraPosSet)
		lod = &inGeometry->GetLOD(JPH::Vec3(mCameraPos), inWorldSpaceBounds, inLODScaleSq);

	// Draw the batch
	const BatchImpl* batch = static_cast<const BatchImpl*>(lod->mTriangleBatch.GetPtr());
	
	size_t triangleCount = batch->mTriangles.size();

	float* vertPointr = nullptr;
	unsigned int* indxPointr = nullptr;

	size_t oldVertexSize = 0;
	size_t oldIndexSize = 0;

	if (inDrawMode == EDrawMode::Wireframe)
	{
		oldVertexSize = wireframeVertices.size();
		oldIndexSize = wireframeIndices.size();

		wireframeVertices.resize(wireframeVertices.size() + triangleCount * 3 * 6);
		wireframeIndices.resize(wireframeIndices.size() + triangleCount * 6);

		vertPointr = &wireframeVertices[oldVertexSize];
		indxPointr = &wireframeIndices[oldIndexSize];
	}
	else
	{
		oldVertexSize = vertices.size();
		oldIndexSize = indices.size();

		vertices.resize(vertices.size() + triangleCount * 3 * 6);
		indices.resize(indices.size() + triangleCount * 3);

		vertPointr = &vertices[oldVertexSize];
		indxPointr = &indices[oldIndexSize];
	}

	unsigned int base = static_cast<unsigned int>(oldVertexSize) / 6;

	for (const Triangle& triangle : batch->mTriangles)
	{
		JPH::RVec3 v0 = inModelMatrix * JPH::Vec3(triangle.mV[0].mPosition);
		JPH::RVec3 v1 = inModelMatrix * JPH::Vec3(triangle.mV[1].mPosition);
		JPH::RVec3 v2 = inModelMatrix * JPH::Vec3(triangle.mV[2].mPosition);
		
		JPH::Color color = inModelColor * triangle.mV[0].mColor;
		float r = color.r / 255.0f;
		float g = color.g / 255.0f;
		float b = color.b / 255.0f;

		*vertPointr++ = (float)v0.GetX(); *vertPointr++ = (float)v0.GetY(); *vertPointr++ = (float)v0.GetZ(); *vertPointr++ = r; *vertPointr++ = g; *vertPointr++ = b;
		*vertPointr++ = (float)v1.GetX(); *vertPointr++ = (float)v1.GetY(); *vertPointr++ = (float)v1.GetZ(); *vertPointr++ = r; *vertPointr++ = g; *vertPointr++ = b;
		*vertPointr++ = (float)v2.GetX(); *vertPointr++ = (float)v2.GetY(); *vertPointr++ = (float)v2.GetZ(); *vertPointr++ = r; *vertPointr++ = g; *vertPointr++ = b;
		
		if (inDrawMode == EDrawMode::Wireframe)
		{
			*indxPointr++ = base;
			*indxPointr++ = base + 1;
			*indxPointr++ = base + 1;
			*indxPointr++ = base + 2;
			*indxPointr++ = base + 2;
			*indxPointr++ = base;
		}
		else
		{
			*indxPointr++ = base;
			*indxPointr++ = base + 1;
			*indxPointr++ = base + 2;
			
		}

		base += 3;
	}
}