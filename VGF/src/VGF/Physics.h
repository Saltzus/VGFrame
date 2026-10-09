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

