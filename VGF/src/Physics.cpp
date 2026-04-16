#include "Physics.h"

#include "Input.h"

namespace VGF
{
	Physics::Physics()
	{
		///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		overlappingPairCache = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		solver = new btSequentialImpulseConstraintSolver;

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

		debug = new PhysicsDebugDraw();
		dynamicsWorld->setDebugDrawer(debug);
	}

	void Physics::Update(double delta_time)
	{
		dynamicsWorld->stepSimulation(static_cast<btScalar>(delta_time), 10, 1.0f / 60.0f);

		for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; --j)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;

			if (body && body->getMotionState())
				body->getMotionState()->getWorldTransform(trans);
			else
				trans = obj->getWorldTransform();
		}
	}

	Physics::~Physics()
	{
		delete collisionConfiguration;
		delete dispatcher;
		delete overlappingPairCache;
		delete solver;
		delete dynamicsWorld;

		delete debug;
	}

	VGF::Renderer* lines;
	MatrixBufferObject matrixBuffer;

	void Physics::debugRender(const Camera& camera)
	{
		if (Input::getDebugDrawerOn())
		{
			delete lines;

			MatrixData data;
			data.model = glm::mat4(1.f);
			data.proj = camera.projection;
			data.view = camera.view;
			matrixBuffer.SetData((void*)&data);

			debug->indices.clear();
			debug->vertices.clear();

			dynamicsWorld->debugDrawWorld();

			lines = new VGF::Renderer(debug->indices, debug->vertices, {MatrixBufferObject::getDefault()});
			lines->Render(GetDefaultConfig(), {&matrixBuffer});
		}
	}
}
