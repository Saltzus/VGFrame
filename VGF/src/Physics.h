#pragma once

#include <btBulletDynamicsCommon.h>
#include "PhysicsDebugDraw.h"

namespace VGF
{
	class Physics
	{
	public:
		Physics();
		~Physics();

		void Update(double delta_time);
		void debugRender(const Camera& camera);

		PhysicsDebugDraw* debug;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
	private:
		int i;

		static const VGF::PipelineConfig& GetDefaultConfig()
		{
			static const VGF::PipelineConfig config
			(
				VGF::Resource::Get("Shaders/debug.vert"),
				VGF::Resource::Get("Shaders/debug.frag"),
				VGF::Topology::LINE_LIST
			);
			return config;
		}

		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
	};
}

