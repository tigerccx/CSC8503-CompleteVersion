#pragma once
#include "../CSC8503Common/GameWorld.h"
#include "WorldHierarchy.h"
#include <set>

namespace NCL {
	namespace CSC8503 {
		class PhysicsSystem	{
		public:
			PhysicsSystem(GameWorld* g = nullptr);
			~PhysicsSystem();

			void Clear();

			void Update(float dt);

			void UseGravity(bool state) {
				applyGravity = state;
			}

			void RemoveAllCollisionsInCurrentWorld();
			inline void SetWorld(GameWorld* world) { gameWorld = world; }
			inline GameWorld* GetWorld() const { return gameWorld; }

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g);

			void SetLinearDamping(const float damping);
			float GetLinearDamping() const;
			void SetAngularDamping(const float damping);
			float GetAngularDamping() const;

		protected:
			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);
			void UpdateCollisionList();
			//void UpdateDynamicObjectAABBs();

			void ImpulseResolveCollision(GameObject& a , GameObject&b, CollisionDetection::ContactPoint& p) const;
			void PenaltyResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const;

			GameWorld* gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;
			float linearDamping = 0.4f;
			float angularDamping = 0.4f;

			std::set<CollisionDetection::CollisionInfo> broadphaseCollisions;

			std::set<CollisionDetection::CollisionInfo> allCollisions;

			bool useBroadPhase		= true;
			int numCollisionFrames	= 5;
		};
	}
}

