#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <iostream>
#include <cstdarg>
#include <thread>

#include "PipelineConfig.h"
#include "Log.h"
#include "VertexBuffers/DefaultInstanceBuffer.h"
#include "VertexBuffers/DebugVertexBuffer.h"

#include "PhysicsDebugRenderer.h"

namespace VGF
{
	namespace PhysicsLayers
	{
		static constexpr JPH::ObjectLayer NON_MOVING = 0;
		static constexpr JPH::ObjectLayer MOVING = 1;
		static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
	};

	namespace BroadPhaseLayers
	{
		static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
		static constexpr JPH::BroadPhaseLayer MOVING(1);
		static constexpr unsigned int NUM_LAYERS(2);
	};

	class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
	{
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
		{
			switch (inObject1)
			{
			case PhysicsLayers::NON_MOVING:
				return inObject2 == PhysicsLayers::MOVING; // Non moving only collides with moving
			case PhysicsLayers::MOVING:
				return true; // Moving collides with everything
			default:
				JPH_ASSERT(false);
				return false;
			}
		}
	};

	class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
	{
	public:
		BPLayerInterfaceImpl()
		{
			// Create a mapping table from object to broad phase layer
			mObjectToBroadPhase[PhysicsLayers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
			mObjectToBroadPhase[PhysicsLayers::MOVING] = BroadPhaseLayers::MOVING;
		}

		virtual unsigned int GetNumBroadPhaseLayers() const override{
			return BroadPhaseLayers::NUM_LAYERS;
		}

		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
		{
			if (inLayer > PhysicsLayers::NUM_LAYERS) Log::Error("inLayer < PhysicsLayers::NUM_LAYERS");
			return mObjectToBroadPhase[inLayer];
		}

		virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
		{
			switch ((JPH::BroadPhaseLayer::Type)inLayer)
			{
			case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	
				return "NON_MOVING";
			case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		
				return "MOVING";
			default:													
				JPH_ASSERT(false); return "INVALID";
			}
		}

	private:
		JPH::BroadPhaseLayer mObjectToBroadPhase[PhysicsLayers::NUM_LAYERS];
	};

	class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
	{
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
		{
			switch (inLayer1)
			{
			case PhysicsLayers::NON_MOVING:
				return inLayer2 == BroadPhaseLayers::MOVING;
			case PhysicsLayers::MOVING:
				return true;
			default:
				JPH_ASSERT(false);
				return false;
			}
		}
	};

	class VGFContactListener : public JPH::ContactListener
	{
	public:
		// See: ContactListener
		virtual JPH::ValidateResult	OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override
		{
			Log::Info("Physics Contact validate callback");

			// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
			return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
		}

		virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
		{
			Log::Info("Physics A contact was added");
		}

		virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
		{
			Log::Info("Physics A contact was persisted");
		}

		virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
		{
			Log::Info("Physics A contact was removed");
		}
	};

	class VGFBodyActivationListener : public JPH::BodyActivationListener
	{
	public:
		virtual void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
		{
			Log::Info("A body got activated");
		}

		virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
		{
			Log::Info("Physics A body went to sleep");
		}
	};

	class Camera;

	class Physics
	{
	public:
		Physics();
		~Physics();

		static Physics* GetInstance() { return physics; }

		void Update(float deltaTime);
		void DebugRender(const Camera& camera);

		const unsigned int maxBodies = 65536;
		const unsigned int numBodyMutexes = 0;
		const unsigned int maxBodyPairs = 65536;
		const unsigned int maxContactConstraints = 10240;

		JPH::TempAllocatorImpl* tempAllocator;
		JPH::JobSystemThreadPool jobSystem;
		JPH::PhysicsSystem physicsSystem;
		JPH::BodyInterface* bodyInterface;

		VGFBodyActivationListener bodyActivationListener;
		VGFContactListener contactListener;

		BPLayerInterfaceImpl broadPhaseLayerInterface;
		ObjectVsBroadPhaseLayerFilterImpl objectVsBroadphaseLayerFilter;
		ObjectLayerPairFilterImpl objectVsObjectLayerFilter;

		JPH::BodyManager::DrawSettings drawSettings;
		PhysicsDebugRenderer* debugRenderer;

		bool debugRenderOn = false;

	private:

		inline static Physics* physics = nullptr;

		static inline DebugVertexBuffer debugVertexBuffer;
		static inline DefaultInstanceBuffer instanceBuffer;

		static const VGF::PipelineConfig& GetDefaultConfig(bool wireframe)
		{
			if (wireframe)
			{
				static const VGF::PipelineConfig config
				(
					VGF::Resource::Get("Shaders/debug.vert"),
					VGF::Resource::Get("Shaders/debug.frag"),
					VGF::Topology::LINE_LIST,
					false,
					debugVertexBuffer,
					instanceBuffer
				);
				return config;
			}
			else
			{
				static const VGF::PipelineConfig config
				(
					VGF::Resource::Get("Shaders/debug.vert"),
					VGF::Resource::Get("Shaders/debug.frag"),
					VGF::Topology::TRIANGLE_LIST,
					false,
					debugVertexBuffer,
					instanceBuffer
				);
				return config;
			}
		}
	};
}

