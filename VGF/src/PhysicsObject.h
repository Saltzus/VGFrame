#pragma once

#include "Object.h"
#include "Physics.h"

namespace VGF
{
    class PhysicsObject : public Object
    {
    public:
        PhysicsObject(Model* model, Physics* physics, JPH::Vec3 origin = { 0,0,0 }, JPH::Vec3 size = { 1,1,1 }, float mass = 0.f);

        ~PhysicsObject();

        using Object::SetPosition;
        using Object::SetRotation;
        using Object::SetScale;

        void SetPosition(const float x, const float y, const float z) override { SetPosition(JPH::Vec3(x, y, z)); }
        void SetPosition(const glm::vec3 position) { SetPosition(JPH::Vec3(position.x, position.y, position.z)); }
        void SetPosition(const JPH::Vec3 position);

        void SetRotation(const glm::quat rotation) override { SetRotation(JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w)); }
        void SetRotation(const JPH::Quat rotation);

        void SetScale(const float x, const float y, const float z) override;
        void SetScale(const JPH::Vec3 scale) { SetScale(scale.GetX(), scale.GetY(), scale.GetZ()); }

        JPH::Vec3 GetPositionJPH() { return _physics->bodyInterface->GetPosition(body->GetID()); }
        glm::vec3 GetPositionGlm() { JPH::Vec3 position = GetPositionJPH(); return { position.GetX(), position.GetY(), position.GetZ() }; }

        
        JPH::Body* body;

    private:
        Physics* _physics;
        glm::mat4 CreateModelMatrix() const override;
    };

}
