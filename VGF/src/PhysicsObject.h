#pragma once

#include "Object.h"
#include "Physics.h"

namespace VGF
{
    class PhysicsObject : public Object
    {
    public:
        PhysicsObject(Model* model, Physics* physics, btVector3 origin = { 0,0,0 }, btVector3 size = { 2,2,2 }, btScalar mass = 0.f);

        ~PhysicsObject();

        using Object::SetPosition;
        using Object::SetRotation;
        using Object::SetScale;

        void SetPosition(const float x, const float y, const float z) override;
        void SetPosition(const btVector3 position) { SetPosition(position.x(), position.y(), position.z()); }

        void SetRotation(const glm::quat rotation) override;
        void SetRotation(const btVector3 rotation) { SetRotation(rotation.x(), rotation.y(), rotation.z()); }

        void SetScale(const float x, const float y, const float z) override;
        void SetScale(const btVector3 scale) { SetScale(scale.x(), scale.y(), scale.z()); }

        btVector3 GetPositionBt();
        glm::vec3 GetPositionGlm() { btVector3 position = GetPositionBt(); return { position.x(), position.y(), position.z() }; }

        btRigidBody* body;
    private:
        Physics* _physics;
        btCollisionShape* collisionShape;

        glm::mat4 CreateModelMatrix() const override;
    };

}
