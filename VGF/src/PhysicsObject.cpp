#include "PhysicsObject.h"

namespace VGF
{
    using namespace JPH::literals;

    PhysicsObject::PhysicsObject(Model* model, Physics* physics, JPH::Vec3 origin, JPH::Vec3 size, float mass) : Object(model), _physics(physics)
    {
        SetScale(size);

        JPH::ObjectLayer layer = PhysicsLayers::NON_MOVING;
        JPH::EMotionType motionType = JPH::EMotionType::Static;

        if (mass > 0)
        {
            layer = PhysicsLayers::MOVING;
            motionType = JPH::EMotionType::Dynamic;
        }

        JPH::BoxShapeSettings bodyShapeSettings(size / 2);
        bodyShapeSettings.SetEmbedded();

        JPH::ShapeSettings::ShapeResult bodyShapeResult = bodyShapeSettings.Create();
        JPH::ShapeRefC bodyShape = bodyShapeResult.Get();

        JPH::BodyCreationSettings bodySettings(bodyShape, origin, JPH::Quat::sIdentity(), motionType, layer);

        JPH::MassProperties msp;
        msp.ScaleToMass(mass);

        bodySettings.mMassPropertiesOverride = msp;
        bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;

        body = physics->bodyInterface->CreateBody(bodySettings);
        physics->bodyInterface->AddBody(body->GetID(), JPH::EActivation::DontActivate);
    }

    PhysicsObject::~PhysicsObject()
    {
        _physics->bodyInterface->RemoveBody(body->GetID());
        _physics->bodyInterface->DestroyBody(body->GetID());
    }

    glm::mat4 PhysicsObject::CreateModelMatrix() const
    {
        JPH::RMat44 joltMat = _physics->bodyInterface->GetWorldTransform(body->GetID());

        glm::mat4 model
        (
            joltMat.GetColumn4(0).GetX(), joltMat.GetColumn4(0).GetY(), joltMat.GetColumn4(0).GetZ(), joltMat.GetColumn4(0).GetW(),
            joltMat.GetColumn4(1).GetX(), joltMat.GetColumn4(1).GetY(), joltMat.GetColumn4(1).GetZ(), joltMat.GetColumn4(1).GetW(),
            joltMat.GetColumn4(2).GetX(), joltMat.GetColumn4(2).GetY(), joltMat.GetColumn4(2).GetZ(), joltMat.GetColumn4(2).GetW(),
            joltMat.GetColumn4(3).GetX(), joltMat.GetColumn4(3).GetY(), joltMat.GetColumn4(3).GetZ(), joltMat.GetColumn4(3).GetW()
        );

        return glm::scale(model, _scale);
    }

    void PhysicsObject::SetPosition(const JPH::Vec3 position)
    {
        _physics->bodyInterface->SetPosition(body->GetID(), JPH::Vec3(position.GetX(), position.GetY(), position.GetZ()), JPH::EActivation::Activate);
        _position = glm::vec3(position.GetX(), position.GetY(), position.GetZ());
    }

    void PhysicsObject::SetRotation(const JPH::Quat rotation) {
        _physics->bodyInterface->SetRotation(body->GetID(), rotation, JPH::EActivation::Activate);
        _rotation = glm::quat(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW());
    }

    void PhysicsObject::SetScale(const float x, const float y, const float z)
    {
        JPH::Vec3 newScale(x, y, z);
        
        
       //_physics->bodyInterface->SetShape(
       //    body->GetID(),
       //    scaledShape,
       //    true,
       //    JPH::EActivation::Activate
       //);
        
        _scale = glm::vec3(x, y, z);
    }

}

