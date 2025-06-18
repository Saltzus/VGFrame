#include "PhysicsObject.h"

namespace VGF
{
    
    PhysicsObject::PhysicsObject(Physics* physics, btVector3 origin, btVector3 size, btScalar mass) : Object()
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

    void PhysicsObject::Render(PipelineConfig& config, Camera* camera, glm::mat4 model)
    {   
        texture->Bind();

        btMatrix3x3 basis = body->getWorldTransform().getBasis();
        btVector3 origin = body->getWorldTransform().getOrigin();

        btTransform transform(basis, origin);
        btScalar bulletMat[16];
        transform.getOpenGLMatrix(bulletMat);
        model = glm::make_mat4(bulletMat);

        model = glm::scale(model, scale);

        for (const auto nodeIdx : this->model->model.scenes[this->model->model.defaultScene].nodes)
            this->model->drawNodes(0, model, config, camera);

        //this->renderer->Render(config, camera, model);
    }

    PhysicsObject::~PhysicsObject()
    {
        delete body->getMotionState();
        delete body;

        delete collisionShape;
    }

    
}

