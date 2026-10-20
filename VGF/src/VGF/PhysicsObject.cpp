#include "PhysicsObject.h"

namespace VGF
{
    PhysicsObject::PhysicsObject(std::string modelPath, Physics* physics, b3Vec3 origin, b3Vec3 size, b3BodyType type, float density) : Object(modelPath), _physics(physics)
    {
        InitObject(physics, origin, size, type, density);
    }

    PhysicsObject::PhysicsObject(Model& model, Physics* physics, b3Vec3 origin, b3Vec3 size, b3BodyType type, float density) : Object(model), _physics(physics)
    {
        InitObject(physics, origin, size, type, density);
    }

    void PhysicsObject::InitObject(Physics* physics, b3Vec3 origin, b3Vec3 size, b3BodyType type, float density)
    {
        SetScale(size);

        b3BodyDef bodyDef = b3DefaultBodyDef();
        bodyDef.type = type;
        bodyDef.position = origin;
        b3BodyId bodyId = b3CreateBody(physics->worldId, &bodyDef);

        b3BoxHull dynamicBox = b3MakeBoxHull(size.x/2,size.y/2,size.z/2);

        b3ShapeDef shapeDef = b3DefaultShapeDef();
        shapeDef.density = density;
        shapeDef.baseMaterial.friction = 0.3f;

        b3CreateHullShape(bodyId, &shapeDef, &dynamicBox.base);
    }

    PhysicsObject::~PhysicsObject()
    {
        if (b3Body_IsValid(body)) b3DestroyBody(body);
    }

    glm::mat4 PhysicsObject::CreateModelMatrix(glm::mat4 cameraViewMatrix) const
    {
        const b3Vec3& b3Position = b3Body_GetPosition(body);
        const b3Quat& b3Rotation = b3Body_GetRotation(body);

        glm::vec3 glmPosition = { b3Position.x, b3Position.y, b3Position.z };
        glm::quat glmRotation = { b3Rotation.s, b3Rotation.v.x, b3Rotation.v.y, b3Rotation.v.z };

        glm::mat4 transform = glm::mat4_cast(glmRotation);
        return glm::translate(glm::mat4(1.f), glmPosition) * transform;
    }

    void PhysicsObject::SetPosition(const b3Vec3 position)
    {
        b3Body_SetTransform(body, position, b3Body_GetRotation(body));
        _position = glm::vec3(position.x, position.y, position.z);
    }

    void PhysicsObject::SetRotation(const b3Quat rotation) 
    {
        b3Body_SetTransform(body, b3Body_GetPosition(body), rotation);
        _rotation = glm::quat(rotation.s, rotation.v.x, rotation.v.y, rotation.v.z);
    }

    void PhysicsObject::SetScale(const float x, const float y, const float z)
    {
        b3Vec3 newScale = { x, y, z };
        _scale = { x, y, z };
    }

}

