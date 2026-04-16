#include "PhysicsObject.h"

namespace VGF
{
    
    PhysicsObject::PhysicsObject(Model* model, Physics* physics, btVector3 origin, btVector3 size, btScalar mass) : Object(model), _physics(physics)
    {
        collisionShape = new btBoxShape(btVector3(size / 2));

        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(origin));

        _scale.x = size.x();
        _scale.y = size.y();
        _scale.z = size.z();
        
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

    glm::mat4 PhysicsObject::CreateModelMatrix() const
    {
        btMatrix3x3 basis = body->getWorldTransform().getBasis();
        btVector3 origin = body->getWorldTransform().getOrigin();

        btTransform transform(basis, origin);
        btScalar bulletMat[16];
        transform.getOpenGLMatrix(bulletMat);
        glm::mat4 model = glm::make_mat4(bulletMat);

        return glm::scale(model, _scale);
    }

    PhysicsObject::~PhysicsObject()
    {
        delete body->getMotionState();
        delete body;

        delete collisionShape;
    }

    void PhysicsObject::SetPosition(const float x, const float y, const float z)
    {
        _position = { x,y,z };

        body->activate(true);

        btMotionState* motionState;
        btTransform transform;

        if ((motionState = body->getMotionState()) != nullptr)
            motionState->getWorldTransform(transform);
        else transform = body->getWorldTransform();

        transform.setOrigin(btVector3(x,y,z));

        if ((motionState = body->getMotionState()) != nullptr)
            motionState->setWorldTransform(transform);
        body->setWorldTransform(transform);
    }

    void PhysicsObject::SetRotation(const glm::quat rotation) {
        _rotation = rotation;
        body->activate(true);
        
        btQuaternion quat(rotation.x, rotation.y, rotation.z, rotation.w);
        btTransform transform;
        btMotionState* motionState;

        if ((motionState = body->getMotionState()) != nullptr)
            motionState->getWorldTransform(transform);
        else transform = body->getWorldTransform();

        transform.setRotation(quat);

        if ((motionState = body->getMotionState()) != nullptr)
            motionState->setWorldTransform(transform);
        body->setWorldTransform(transform);
    }

    void PhysicsObject::SetScale(const float x, const float y, const float z)
    {
        body->activate(true);
        collisionShape->setLocalScaling(btVector3(x, y, z));
        _physics->dynamicsWorld->updateSingleAabb(body);

        _scale = glm::vec3(x, y, z);
    }

    btVector3 PhysicsObject::GetPositionBt()
    {
        btTransform transform;
        body->getMotionState()->getWorldTransform(transform);

        return transform.getOrigin();
    }
}

