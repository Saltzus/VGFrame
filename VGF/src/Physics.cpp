#include "Physics.h"

#include "Input.h"
#include "Camera.h"

namespace VGF
{
	using namespace JPH::literals;

	Physics::Physics()
	{
		physics = this;

		JPH::RegisterDefaultAllocator();	
		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();

		tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

		jobSystem.Init(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
		physicsSystem.Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, broadPhaseLayerInterface, objectVsBroadphaseLayerFilter, objectVsObjectLayerFilter);

		bodyInterface = &physicsSystem.GetBodyInterface();

		const float deltaTime = 1.0f / 60.0f;
		physicsSystem.OptimizeBroadPhase();

		debugRenderer = new PhysicsDebugRenderer();
	}

	void Physics::Update()
	{
		physicsSystem.Update(1.0f / 60.0f, 1, tempAllocator, &jobSystem);
	}

	Physics::~Physics()
	{
		JPH::UnregisterTypes();

		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;

		delete tempAllocator;
		delete debugRenderer;

	}

	VGF::Renderer* wireframeTriangles;
	VGF::Renderer* Triangles;
	MatrixBufferObject matrixBuffer;

	void Physics::DebugRender(const Camera& camera)
	{
		if (debugRenderOn)
		{
			if (wireframeTriangles) delete wireframeTriangles; wireframeTriangles = nullptr;
			if (Triangles) delete Triangles; Triangles = nullptr;

			MatrixData data;
			data.model = glm::mat4(1.f);
			data.proj = camera.projection;
			data.view = camera.view;
			matrixBuffer.SetData((void*)&data);


			physicsSystem.DrawBodies(drawSettings, debugRenderer);


			if (!debugRenderer->indices.empty() && !debugRenderer->vertices.empty())
			{
				Triangles = new VGF::Renderer(debugRenderer->indices, debugRenderer->vertices, { MatrixBufferObject::getDefault() });
				Triangles->Render(GetDefaultConfig(false), { &matrixBuffer });
			}

			if (!debugRenderer->wireframeIndices.empty() && !debugRenderer->wireframeVertices.empty())
			{
				wireframeTriangles = new VGF::Renderer(debugRenderer->wireframeIndices, debugRenderer->wireframeVertices, { MatrixBufferObject::getDefault() });
				wireframeTriangles->Render(GetDefaultConfig(true), { &matrixBuffer });
			}
		}

		debugRenderer->wireframeIndices.clear();
		debugRenderer->wireframeVertices.clear();
		debugRenderer->indices.clear();
		debugRenderer->vertices.clear();
	}
}
