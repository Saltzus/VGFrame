#pragma once

#include <btBulletDynamicsCommon.h>

namespace VGF
{
	class Physics
	{
	public:
		Physics();
		~Physics();

		void Update(double delta_time);

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

