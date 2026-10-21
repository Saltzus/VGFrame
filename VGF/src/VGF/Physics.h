#pragma once

#include <box3d/box3d.h>

#include <iostream>
#include <cstdarg>
#include <thread>
#include <unordered_set>

#include "PipelineConfig.h"
#include "Log.h"
#include "VertexBuffers/DefaultInstanceBuffer.h"
#include "VertexBuffers/DebugVertexBuffer.h"

#include "PhysicsDebugRenderer.h"

namespace VGF
{
	struct RaycastContext
	{
		b3BodyId ignoreBody;
		b3BodyId hitBody;
		b3ShapeId shapeId;
		b3Vec3 point;
		b3Vec3 normal;
		float fraction;
		bool hit = false;
	};

	static float RayCastCallback(b3ShapeId shapeId, b3Pos point, b3Vec3 normal, float fraction, uint64_t usermaterialId, int triangleIndex, int childIndex, void* context)
	{
		RaycastContext* myContext = static_cast<RaycastContext*>(context);
		b3BodyId bodyId = b3Shape_GetBody(shapeId);

		if (bodyId.index1 == myContext->ignoreBody.index1 && bodyId.world0 == myContext->ignoreBody.world0)
		{
			return -1.f;
		}

		myContext->hitBody = bodyId;
		myContext->shapeId = shapeId;
		myContext->point = point;
		myContext->normal = normal;
		myContext->fraction = fraction;
		myContext->hit = true;

		return fraction;
	};

	class Camera;
	class Physics
	{
	public:
		Physics();
		~Physics();

		static Physics* GetInstance() { return physics; }

		void Step(float deltaTime);
		void Update(float deltaTime);
		void DebugRender(const Camera& camera);
	
		b3WorldId worldId;
		PhysicsDebugRenderer debugRenderer;

		bool debugRenderOn = false;

	private:
		static inline Physics* physics = nullptr;

		static inline DebugVertexBuffer debugVertexBuffer;
		static inline DefaultInstanceBuffer instanceBuffer;

		static const VGF::PipelineConfig& GetDefaultConfig(bool wireframe)
		{
			if (wireframe)
			{
				static const VGF::PipelineConfig config
				(
					VGF::Resource::Get("Shaders/debug_vert.slang"),
					VGF::Resource::Get("Shaders/debug_frag.slang"),
					VGF::Topology::LINE_LIST,
					false,
					debugVertexBuffer,
					instanceBuffer
				);
				return config;
			}
			else
			{
				static const VGF::PipelineConfig config
				(
					VGF::Resource::Get("Shaders/debug_vert.slang"),
					VGF::Resource::Get("Shaders/debug_frag.slang"),
					VGF::Topology::TRIANGLE_LIST,
					false,
					debugVertexBuffer,
					instanceBuffer
				);
				return config;
			}
		}
	};
}

