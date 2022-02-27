#pragma once
#include "Settings.h"

#include "../../Common/Camera.h"
#include "../../Common/Plane.h"

#include "Transform.h"
#include "GameObject.h"

#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "CapsuleVolume.h"
#include "Ray.h"

#include "../../Common/Exceptions.h"

using NCL::Camera;
using namespace NCL::Maths;
using namespace NCL::CSC8503;
namespace NCL {
	namespace CSC8503 {
		class GameWorld;

		template<class T>
		class QuadTree;
		template<class T>
		class OcTree;
		template<class T>
		class OcTreeNode;
	}

	enum CollisionType {
		CollisionType_VV,
		CollisionType_VF,
		CollisionType_VE,
		CollisionType_FF,
		CollisionType_FE,
		CollisionType_EE,
	};

	class CollisionDetection
	{
	public:
		struct ContactPoint {
			Vector3 localA;
			Vector3 localB;
			Vector3 normal;
			float	penetration;
		};
		struct CollisionInfo {
			GameObject* a;
			GameObject* b;		
			mutable int		framesLeft;

			ContactPoint point;

			void AddContactPoint(const Vector3& localA, const Vector3& localB, const Vector3& normal, float p) {
				point.localA		= localA;
				point.localB		= localB;
				point.normal		= normal;
				point.penetration	= p;
			}

			//Advanced collision detection / resolution
			bool operator < (const CollisionInfo& other) const {
				unsigned long long otherHash = (unsigned long long)other.a->GetWorldID() + ((unsigned long long)other.b->GetWorldID() << 32);
				unsigned long long thisHash  = (unsigned long long)a->GetWorldID()		 + ((unsigned long long)b-> GetWorldID() << 32);

				if (thisHash < otherHash) {
					return true;
				}
				return false;
			}

			bool operator ==(const CollisionInfo& other) const {
				if (other.a == a && other.b == b) {
					std::cout << "EQUAL" << std::endl;
					return true;
				}
				return false;
			}
		};

		static bool Raycast(Ray& r, RayCollision& closestCollision, const GameWorld* world, bool closestObject = false, LayerMask collisionLayerMask = LayerMask::all, bool includeLayerMaskObjects = true);

		//TODO ADD THIS PROPERLY
		static bool RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision);

		static Ray BuildRayFromMouse(const Camera& c);

		//Tree
		static bool RayTreeIntersection(const Ray& r, OcTree<GameObject*>& tree, vector<GameObject*>& gosOut);
		static bool RayTreeIntersection(const Ray& r, OcTree<GameObject*>& tree, vector<OcTreeNode<GameObject*>>& nodesOut);
		static bool AABBTreeIntersection(const Vector3& pos, const Vector3& halfSize, OcTree<GameObject*>& tree, vector<GameObject*>& gosOut);
		static bool AABBTreeIntersection(const Vector3& pos, const Vector3& halfSize, OcTree<GameObject*>& tree, vector<OcTreeNode<GameObject*>>& nodesOut);

		//Ray
		static bool RayIntersection(const Ray&r, GameObject& object, RayCollision &collisions);

		static bool RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume&	volume, RayCollision& collision);
		static bool RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume&	volume, RayCollision& collision);
		static bool RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision);
		static bool RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision);
		

		static bool RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions);

		static bool	AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB);
		static bool RayAABBTest(const Ray& r, const Vector3& boxPos, const Vector3& boxSize);

		static bool ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo);


		static bool AABBIntersection(	const AABBVolume& volumeA, const Transform& worldTransformA,
										const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool SphereIntersection(	const SphereVolume& volumeA, const Transform& worldTransformA,
										const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool AABBSphereIntersection(	const AABBVolume& volumeA	 , const Transform& worldTransformA,
										const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool OBBIntersection(	const OBBVolume& volumeA, const Transform& worldTransformA,
										const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool AABBOBBIntersection(	const AABBVolume& volumeA, const Transform& worldTransformA,
											const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool OBBSphereIntersection(	const OBBVolume& volumeA, const Transform& worldTransformA,
											const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool CapsuleIntersection(	const CapsuleVolume& volumeA, const Transform& worldTransformA,
												const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool SphereCapsuleIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
			const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool AABBCapsuleIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
			const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool OBBCapsuleIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
			const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static Vector3 Unproject(const Vector3& screenPos, const Camera& cam);

		static Vector3		UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c);
		static Matrix4		GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane);
		static Matrix4		GenerateInverseView(const Camera &c);

		static void SATtest(const Vector3& axis, const vector<Vector3>& ptSet, float& minAlong, float& maxAlong);
		static void SATtest(const Vector3& axis, const vector<Vector3>& ptSet, float& minAlong, float& maxAlong,
							Vector3& minPtAlong, Vector3& maxPtAlong);


	protected:
	
	private:
		CollisionDetection()	{}
		~CollisionDetection()	{}
	};
}

