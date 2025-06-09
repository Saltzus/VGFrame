#pragma once

#include "Object.h"
#include "Physics.h"

namespace VGF
{
    class PhysicsObject : private Object
    {
    public:
        PhysicsObject(Physics* physics, btVector3 origin = { 0,0,0 }, btVector3 size = { 2,2,2 }, btScalar mass = 0.f);
        ~PhysicsObject();

        virtual void Render(PipelineConfig& config, Camera* camera, glm::mat4 model = glm::mat4(1.0f)) override;

        btRigidBody* body;
    private:
        btCollisionShape* collisionShape;

        using Object::model;
        using Object::translation;
        using Object::rotation;
        using Object::scale;
    };

}
