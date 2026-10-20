#pragma once

#include "Object.h"
#include "Physics.h"

namespace VGF
{
    class PhysicsObject : public Object
    {
    public:
        PhysicsObject(Model& model, Physics* physics, b3Vec3 origin = { 0,0,0 }, b3Vec3 size = { 1,1,1 }, b3BodyType type = b3_dynamicBody, float density = 1.f);
        PhysicsObject(std::string modelPath, Physics* physics, b3Vec3 origin = { 0,0,0 }, b3Vec3 size = { 1,1,1 }, b3BodyType type = b3_dynamicBody, float density = 1.f);

        ~PhysicsObject();

        using Object::SetPosition;
        using Object::SetRotation;
        using Object::SetScale;

        void SetPosition(const float x, const float y, const float z) override { SetPosition(b3Vec3(x, y, z)); }
        void SetPosition(const glm::vec3 position) { SetPosition(b3Vec3(position.x, position.y, position.z)); }
        void SetPosition(const b3Vec3 position);

        void SetRotation(const glm::quat rotation) override { SetRotation(b3Quat{ rotation.x, rotation.y, rotation.z, rotation.w }); }
        void SetRotation(const b3Quat rotation);

        void SetScale(const float x, const float y, const float z) override;
        void SetScale(const b3Vec3 scale) { SetScale(scale.x, scale.y, scale.z); }

        b3Vec3 GetPositionb3() { return { 1,1,1 }; }
        glm::vec3 GetPositionGlm() { b3Vec3 position = GetPositionb3(); return { position.x, position.y, position.z }; }

        b3BodyId body;

    private:
        Physics* _physics;
        glm::mat4 CreateModelMatrix(glm::mat4 cameraViewMatrix) const override;
        void InitObject(Physics* physics, b3Vec3 origin, b3Vec3 size, b3BodyType type, float density);
    };

}
