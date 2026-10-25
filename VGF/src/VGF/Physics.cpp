#include "Physics.h"

#include "Input.h"
#include "Camera.h"

#include "DebugShapes.h"
#include "RenderData.h"

namespace VGF
{
	static void* CreateDebugShape(const b3DebugShape* shape, void* context)
	{
		return static_cast<void*>(new DebugShapes::Shape(shape));
	}

	static void DestroyDebugShape(void* userShape, void* context)
	{
		if (userShape != nullptr) delete static_cast<DebugShapes::Shape*>(userShape);
	}

	Physics::Physics()
	{
		physics = this;

		b3WorldDef worldDef = b3DefaultWorldDef();
		worldDef.createDebugShape = CreateDebugShape;
		worldDef.destroyDebugShape = DestroyDebugShape;
		worldDef.userDebugShapeContext = nullptr;

		worldId = b3CreateWorld(&worldDef);

		//JPH::RegisterDefaultAllocator();	
		//JPH::Factory::sInstance = new JPH::Factory();
		//JPH::RegisterTypes();
		//
		//tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
		//
		//jobSystem.Init(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
		//physicsSystem.Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, broadPhaseLayerInterface, objectVsBroadphaseLayerFilter, objectVsObjectLayerFilter);
		//
		//bodyInterface = &physicsSystem.GetBodyInterface();
		//
		//const float deltaTime = 1.0f / 60.0f;
		//physicsSystem.OptimizeBroadPhase();
		//
		//debugRenderer = new PhysicsDebugRenderer();
	}

	float accumulator = 0;
	float alpha = 0;

	void Physics::Step(float deltaTime)
	{
		b3World_Step(worldId, deltaTime, 4);
	}

	void Physics::Update(float deltaTime)
	{
		accumulator += deltaTime;
		float fixedDeltaTime = 1.f / 60.f;

		while (accumulator >= fixedDeltaTime)
		{
			Step(fixedDeltaTime);
			accumulator -= fixedDeltaTime;
		}

		alpha = accumulator / fixedDeltaTime;
	}

	Physics::~Physics()
	{
		b3DestroyWorld(worldId);
	}

	VGF::Renderer* wireframeTriangles;
	VGF::Renderer* Triangles;
	MatrixBufferObject matrixBuffer;

	

	void Physics::DebugRender(const Camera& camera)
	{
		RenderData renderData = RenderData(GetDefaultShader(true));

		if (debugRenderOn)
		{
			if (wireframeTriangles) delete wireframeTriangles; wireframeTriangles = nullptr;
			if (Triangles) delete Triangles; Triangles = nullptr;

			renderData.Set("MatrixBuffer.model", glm::mat4(1.f));
			renderData.Set("MatrixBuffer.proj", camera.projection);
			renderData.Set("MatrixBuffer.view", camera.view);

			debugRenderer.Debug(worldId);

			if (!debugRenderer.indices.empty() && !debugRenderer.vertices.empty())
			{
				Triangles = new VGF::Renderer(GetDefaultShader(false), debugRenderer.indices, debugRenderer.vertices);
				Triangles->Render(renderData, GetDefaultShader(false));
			}
			
			if (!debugRenderer.wireframeIndices.empty() && !debugRenderer.wireframeVertices.empty())
			{
				wireframeTriangles = new VGF::Renderer(GetDefaultShader(true), debugRenderer.wireframeIndices, debugRenderer.wireframeVertices);
				wireframeTriangles->Render(renderData, GetDefaultShader(true));
			}
		}

		debugRenderer.wireframeIndices.clear();
		debugRenderer.wireframeVertices.clear();
		debugRenderer.indices.clear();
		debugRenderer.vertices.clear();
	}
}
