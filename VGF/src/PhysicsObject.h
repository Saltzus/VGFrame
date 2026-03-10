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

        virtual void Render(PipelineConfig& config, Camera* camera, std::vector<UniformBufferObject*> additionalUniformBuffers = {}) override;

        void SetPosition(float x, float y, float z);
        void SetPosition(btVector3 position) { SetPosition(position.x(), position.y(), position.z()); }
        void SetPosition(glm::vec3 position) { SetPosition(position.x  , position.y  , position.z  ); }

        btVector3 GetPositionBt();
        glm::vec3 GetPositionGlm() { btVector3 position = GetPositionBt(); return { position.x(), position.y(), position.z() }; }

        btRigidBody* body;
    private:
        btCollisionShape* collisionShape;

        using Object::model;
        using Object::translation;
        using Object::rotation;
        using Object::scale;
    };

}
