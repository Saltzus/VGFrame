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
		void debugRender(PipelineConfig& config, Camera* camera);

		PhysicsDebugDraw* debug;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
	private:
		int i;

		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
	};
}

