#include "PhysicsObject.h"

namespace VGF
{
    
    PhysicsObject::PhysicsObject(Model* model, Physics* physics, btVector3 origin, btVector3 size, btScalar mass) : Object(model)
    {
        collisionShape = new btBoxShape(btVector3(size / 2));

        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(origin));

        scale.x = size.x();
        scale.y = size.y();
        scale.z = size.z();
        
        bool isDynamic = (mass != 0.f);

        btVector3 localInertia(0, 0, 0);
        if (isDynamic)
            collisionShape->calculateLocalInertia(mass, localInertia);

        //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
        btMotionState* motionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, localInertia);
        body = new btRigidBody(rbInfo);

        //add the body to the dynamics world
        physics->dynamicsWorld->addRigidBody(body);

        body->setUserPointer(this);
    }

    void PhysicsObject::Render(PipelineConfig& config, Camera* camera, std::vector<UniformBufferObject*> additionalUniformBuffers)
    {   
        btMatrix3x3 basis = body->getWorldTransform().getBasis();
        btVector3 origin = body->getWorldTransform().getOrigin();

        btTransform transform(basis, origin);
        btScalar bulletMat[16];
        transform.getOpenGLMatrix(bulletMat);
        glm::mat4 model = glm::make_mat4(bulletMat);

        model = glm::scale(model, scale);

        this->model->renderModel(model, config, camera, additionalUniformBuffers);
    }

    PhysicsObject::~PhysicsObject()
    {
        delete body->getMotionState();
        delete body;

        delete collisionShape;
    }

    

    void PhysicsObject::SetPosition(float x, float y, float z)
    {
        body->activate(true);

        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(x,y,z));

        btMotionState* motionState;
        if ((motionState = body->getMotionState()) != nullptr)
            motionState->setWorldTransform(transform);

        body->setWorldTransform(transform);
    }

    btVector3 PhysicsObject::GetPositionBt()
    {
        btTransform transform;
        body->getMotionState()->getWorldTransform(transform);

        return transform.getOrigin();
    }
}

