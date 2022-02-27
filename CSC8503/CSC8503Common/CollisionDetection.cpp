#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "../../Common/Vector2.h"
#include "../../Common/Window.h"
#include "GameWorld.h"
#include "../../Common/Maths.h"
#include <array>
#include "Debug.h"
#include "../Common/Exceptions.h"

#include "QuadTree.h"
#include "OcTree.h"

#include <list>

using namespace NCL;


bool CollisionDetection::RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions) {
	float ln = Vector3::Dot(p.GetNormal(), r.GetDirection());

	if (ln == 0.0f) {
		return false; //direction vectors are perpendicular!
	}
	
	Vector3 planePoint = p.GetPointOnPlane();

	Vector3 pointDir = planePoint - r.GetPosition();

	float d = Vector3::Dot(pointDir, p.GetNormal()) / ln;

	collisions.collidedAt = r.GetPosition() + (r.GetDirection() * d);

	return true;
}

bool NCL::CollisionDetection::RayTreeIntersection(const Ray& r, OcTree<GameObject*>& tree, vector<GameObject*>& gosOut)
{
	tree.OperateOnNodeHierarchical(
		[&](OcTreeNode<GameObject*>& node)->bool {
			if (!node.children && node.contents.size() == 0) {
				return false;
			}
			bool rslt = RayAABBTest(r, node.GetPosition(), node.GetHalfSize());
			if (rslt && node.contents.size() != 0) {
				for (auto& entry : node.contents) {
					gosOut.emplace_back(entry.object);
				}
			}
			return rslt;
		}
	);
	return gosOut.size() > 0;
}

//Return the leaf nodes intersecting
bool NCL::CollisionDetection::RayTreeIntersection(const Ray& r, OcTree<GameObject*>& tree, vector<OcTreeNode<GameObject*>>& nodesOut)
{
	tree.OperateOnNodeHierarchical(
		[&](OcTreeNode<GameObject*>& node)->bool {
			bool rslt = RayAABBTest(r, node.GetPosition(), node.GetHalfSize());
			if (rslt && !node.children) {
				nodesOut.emplace_back(node);
			}
			return rslt;
		}
	);
	return nodesOut.size() > 0;
}

bool NCL::CollisionDetection::AABBTreeIntersection(const Vector3& pos, const Vector3& halfSize, OcTree<GameObject*>& tree, vector<GameObject*>& gosOut)
{
	tree.OperateOnNodeHierarchical(
		[&](OcTreeNode<GameObject*>& node)->bool {
			if (!node.children && node.contents.size() == 0) {
				return false;
			}
			bool rslt = AABBTest(pos, node.GetPosition(), halfSize, node.GetHalfSize());
			if (rslt && node.contents.size() != 0) {
				for (auto& entry : node.contents) {
					gosOut.emplace_back(entry.object);
				}
			}
			return rslt;
		}
	);
	return gosOut.size() > 0;
}

bool NCL::CollisionDetection::AABBTreeIntersection(const Vector3& pos, const Vector3& halfSize, OcTree<GameObject*>& tree, vector<OcTreeNode<GameObject*>>& nodesOut)
{
	tree.OperateOnNodeHierarchical(
		[&](OcTreeNode<GameObject*>& node)->bool {
			bool rslt = AABBTest(pos, node.GetPosition(), halfSize, node.GetHalfSize());
			if (rslt && !node.children) {
				nodesOut.emplace_back(node);
			}
			return rslt;
		}
	);
	return nodesOut.size() > 0;
}

bool CollisionDetection::RayIntersection(const Ray& r,GameObject& object, RayCollision& collision) {
	bool hasCollided = false;

	const Transform& worldTransform = object.GetTransform();
	const CollisionVolume* volume	= object.GetBoundingVolume();

	if (!volume) {
		return false;
	}

	switch (volume->type) {
		case VolumeType::AABB:		hasCollided = RayAABBIntersection(r, worldTransform, (const AABBVolume&)*volume	, collision); break;
		case VolumeType::OBB:		hasCollided = RayOBBIntersection(r, worldTransform, (const OBBVolume&)*volume	, collision); break;
		case VolumeType::Sphere:	hasCollided = RaySphereIntersection(r, worldTransform, (const SphereVolume&)*volume	, collision); break;
		case VolumeType::Capsule:	hasCollided = RayCapsuleIntersection(r, worldTransform, (const CapsuleVolume&)*volume, collision); break;
	}

	return hasCollided;
}

bool NCL::CollisionDetection::Raycast(Ray& r, RayCollision& closestCollision, const GameWorld* world, bool closestObject, LayerMask collisionLayerMask, bool includeLayerMaskObjects)
{
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	//BroadPhase or not
	vector<GameObject*> gos;

	CollisionDetection::RayTreeIntersection(r, *(world->GetHeirarchy()->treeDynamic), gos);
	CollisionDetection::RayTreeIntersection(r, *(world->GetHeirarchy()->treeStatic), gos);

	for (auto& i : gos) {
		if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
			continue;
		}
		Layer layer = i->GetObjectLayer();
		if (includeLayerMaskObjects ? ((Layer::IsIncludedIn(collisionLayerMask, layer)) > 0) : ((Layer::IsExcludedFrom(collisionLayerMask, layer)))) { //If collisionMask
			RayCollision thisCollision;
			if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {

				if (!closestObject) {
					closestCollision = thisCollision;
					closestCollision.node = i;
					return true;
				}
				else {
					if (thisCollision.rayDistance < collision.rayDistance) {
						thisCollision.node = i;
						collision = thisCollision;
					}
				}
			}
		}
	}
	if (collision.node) {
		closestCollision = collision;
		closestCollision.node = collision.node;
		return true;
	}
	return false;
}

bool CollisionDetection::RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision) {
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;

	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals(-1, -1, -1);

	for (int i = 0; i < 3; ++i) { //Only check half the faces (because it's AAed)
		if (rayDir[i] > 0) {
			tVals[i] = (boxMin[i] - rayPos[i]) / rayDir[i];
		}
		else if(rayDir[i]<0)
		{
			tVals[i] = (boxMax[i] - rayPos[i]) / rayDir[i];
		}
	}
	float bestT = tVals.GetMaxElement();
	if (bestT < 0.0f) {
		return false;
	}

	Vector3 intersection = rayPos + (rayDir * bestT);
	const float epsilon = 0.0001f;
	for (int i = 0; i < 3; ++i) {
		if (intersection[i] + epsilon < boxMin[i] || intersection[i] - epsilon > boxMax[i]) {
			return false;
		}
	}
	collision.collidedAt = intersection;
	collision.rayDistance = bestT;
	return true;
}

bool CollisionDetection::RayAABBTest(const Ray&r, const Vector3& boxPos, const Vector3& boxSize) {
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;

	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals(-1, -1, -1);

	for (int i = 0; i < 3; ++i) { //Only check half the faces (because it's AAed)
		if (rayDir[i] > 0) {
			tVals[i] = (boxMin[i] - rayPos[i]) / rayDir[i];
		}
		else if(rayDir[i]<0)
		{
			tVals[i] = (boxMax[i] - rayPos[i]) / rayDir[i];
		}
	}
	float bestT = tVals.GetMaxElement();

	Vector3 intersection = rayPos + (rayDir * bestT);
	const float epsilon = 0.0001f;
	for (int i = 0; i < 3; ++i) {
		if (intersection[i] + epsilon < boxMin[i] || intersection[i] - epsilon > boxMax[i]) {
			return false;
		}
	}
	return true;
}

bool CollisionDetection::RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision) {
	Vector3 boxPos = worldTransform.GetPosition();
	Vector3 boxSize = volume.GetHalfDimensions();
	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

bool CollisionDetection::RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision) {
	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 position = worldTransform.GetPosition();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	Vector3 localRayPos = r.GetPosition() - position;

	Ray tempRay(invTransform * localRayPos, invTransform * r.GetDirection());

	bool collided = RayBoxIntersection(tempRay, Vector3(), volume.GetHalfDimensions(), collision);
	if (collided) {
		collision.collidedAt = transform * collision.collidedAt + position;
	}
	return collided;
}

bool CollisionDetection::RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision) {
	
	Vector3 centA = worldTransform.GetPosition();
	Matrix4 modelA = worldTransform.GetMatrix();
	float halfHeightA = volume.GetHalfHeight();
	float radA = volume.GetRadius();
	Vector3 tipA = Vector3(0, halfHeightA, 0);
	tipA = modelA * Vector4(tipA, 1.0f);
	Vector3 baseA = -Vector3(0, halfHeightA, 0);
	baseA = modelA * Vector4(baseA, 1.0f);
	Vector3 normalA = (tipA - baseA).Normalised();
	Vector3 lineEndOffsetA = normalA * radA;
	Vector3 aA = baseA + lineEndOffsetA;
	Vector3 bA = tipA - lineEndOffsetA;

	Vector3 rayPoint0 = r.GetPosition();
	float maxDis = 100000.0f;
	Vector3 rayPoint1 = rayPoint0 + r.GetDirection() * maxDis;

	Vector3 closestOnCapsuleAxis, closestOnRay;
	assert(ClosestPointsBetweenLineSegments(aA, bA, rayPoint0, rayPoint1, closestOnCapsuleAxis, closestOnRay));

	Vector3 spherePos = closestOnCapsuleAxis;
	float sphereRadius = volume.GetRadius();

	Vector3 dir = spherePos - r.GetPosition();
	float sphereProj = Vector3::Dot(dir, r.GetDirection());

	if (sphereProj < 0.0f) {
		return false;
	}

	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);

	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius) {
		return false;
	}

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

	collision.rayDistance = sphereProj - offset;
	collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);
	return true;
}

bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision) {
	Vector3 spherePos = worldTransform.GetPosition();
	float sphereRadius = volume.GetRadius();

	Vector3 dir = spherePos - r.GetPosition();
	float sphereProj = Vector3::Dot(dir, r.GetDirection());

	if (sphereProj < 0.0f) {
		return false;
	}

	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);

	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius) {
		return false;
	}

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

	collision.rayDistance = sphereProj - offset;
	collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);
	return true;
}

Matrix4 GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam) {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	//std::cout << "Ray Direction:" << c << std::endl;

	return Ray(cam.GetPosition(), c);
}

//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0]  = aspect / h;
	m.array[5]  = tan(fov*PI_OVER_360);

	m.array[10] = 0.0f;
	m.array[11] = 1.0f / d;

	m.array[14] = 1.0f / e;

	m.array[15] = -c / (d*e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
Matrix4::Translation(position) *
Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
Matrix4::Rotation(pitch, Vector3(1, 0, 0));

return iview;
}

void NCL::CollisionDetection::SATtest(const Vector3& axis, const vector<Vector3>& ptSet, float& minAlong, float& maxAlong)
{
	minAlong = HUGE, maxAlong = -HUGE;
	for (int i = 0; i < ptSet.size(); i++)
	{
		// just dot it to get the min/max along this axis.
		float dotVal = Vector3::Dot(ptSet[i], axis);
		if (dotVal < minAlong)  minAlong = dotVal;
		if (dotVal > maxAlong)  maxAlong = dotVal;
	}
}

void NCL::CollisionDetection::SATtest(const Vector3& axis, const vector<Vector3>& ptSet, float& minAlong, float& maxAlong, Vector3& minPtAlong, Vector3& maxPtAlong)
{
	minAlong = HUGE, maxAlong = -HUGE;
	for (int i = 0; i < ptSet.size(); i++)
	{
		// just dot it to get the min/max along this axis.
		float dotVal = Vector3::Dot(ptSet[i], axis);
		if (dotVal < minAlong) { minAlong = dotVal; minPtAlong = ptSet[i]; }
		if (dotVal > maxAlong) { maxAlong = dotVal; maxPtAlong = ptSet[i]; }
	}
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo) {
	const CollisionVolume* volA = a->GetBoundingVolume();
	const CollisionVolume* volB = b->GetBoundingVolume();

	if (!volA || !volB) {
		return false;
	}

	collisionInfo.a = a;
	collisionInfo.b = b;

	Transform& transformA = a->GetTransform();
	Transform& transformB = b->GetTransform();

	VolumeType pairType = (VolumeType)((int)volA->type | (int)volB->type);

	if (pairType == VolumeType::AABB) {
		return AABBIntersection((AABBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::Sphere) {
		return SphereIntersection((SphereVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::OBB) {
		return OBBIntersection((OBBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::Capsule) {
		return CapsuleIntersection((CapsuleVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}

	//
	//Different volumes
	//
	//AABB<->Sphere
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Sphere) {
		return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}
	//AABB<->OBB
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::OBB) {
		return AABBOBBIntersection((AABBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::OBB && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBOBBIntersection((AABBVolume&)*volB, transformB, (OBBVolume&)*volA, transformA, collisionInfo);
	}
	//AABB<->Capsule
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Capsule) {
		return AABBCapsuleIntersection((AABBVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBCapsuleIntersection((AABBVolume&)*volB, transformB, (CapsuleVolume&)*volA, transformA, collisionInfo);
	}
	//OBB<->Capsule
	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Capsule) {
		return OBBCapsuleIntersection((OBBVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBCapsuleIntersection((OBBVolume&)*volB, transformB, (CapsuleVolume&)*volA, transformA, collisionInfo);
	}
	//Sphere<->OBB
	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Sphere) {
		return OBBSphereIntersection((OBBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBSphereIntersection((OBBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}
	//Sphere<->Capsule
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::Capsule) {
		return SphereCapsuleIntersection((SphereVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Sphere) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return SphereCapsuleIntersection((SphereVolume&)*volB, transformB, (CapsuleVolume&)*volA, transformA, collisionInfo);
	}

	return false;
}

bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB) {
	Vector3 delta = posB - posA;
	Vector3 totalSize = halfSizeA + halfSizeB;

	if (abs(delta.x) < totalSize.x && abs(delta.y) < totalSize.y && abs(delta.z) < totalSize.z) {
		return true;
	}
	return false;
}

//AABB/AABB Collisions
bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();

	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();

	bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);

	if (overlap) {
		static const Vector3 faces[6] = {
			Vector3(-1 , 0 , 0) , Vector3(1 , 0 , 0) ,
			Vector3(0 , -1 , 0) , Vector3(0 , 1 , 0) ,
			Vector3(0 , 0 , -1) , Vector3(0 , 0 , 1) ,
		};

		Vector3 maxA = boxAPos + boxASize;
		Vector3 minA = boxAPos - boxASize;
		
		Vector3 maxB = boxBPos + boxBSize;
		Vector3 minB = boxBPos - boxBSize;

		float distances[6] = {
			(maxB.x - minA.x) ,// distance of box ’b?to ’left ?of ’a ?
			(maxA.x - minB.x) ,// distance of box ’b?to ’right ?of ’a ?
			(maxB.y - minA.y) ,// distance of box ’b?to ’bottom ?of ’a ?
			(maxA.y - minB.y) ,// distance of box ’b?to ’top ?of ’a ?
			(maxB.z - minA.z) ,// distance of box ’b?to ’far ?of ’a ?
			(maxA.z - minB.z) // distance of box ’b?to ’near ?of ’a ?
		};
		float penetration = FLT_MAX;
		Vector3 bestAxis;

		for (int i = 0; i < 6; ++i) {
			if (distances[i] < penetration) {
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}
		collisionInfo.AddContactPoint(Vector3(), Vector3(), bestAxis, penetration); //Contact point doesn't matter?????
		return true;
	}
	return false;
}

//Sphere / Sphere Collision
bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	float radii = volumeA.GetRadius() + volumeB.GetRadius();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	float deltaLength = delta.Length();

	if (deltaLength < radii) {
		float penetration = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;
	}
	return false;
}

//AABB - Sphere Collision
bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxSize = volumeA.GetHalfDimensions();

	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();
	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);
	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < volumeB.GetRadius()) {
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (volumeB.GetRadius() - distance);

		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	return false;
}

////Return projection point along normal of line0
//bool LineLineNormalProjection(const Vector3& dir0, const Vector3& pos0, const Vector3& dir1, const Vector3& pos1, const Vector3& normal,
//								 Vector3& ptIntersect) {
//	Vector3 normPlane = Vector3::Cross(dir1, normal);
//	Vector3 planePt = pos1;
//	Vector3 dirLine = dir0;
//	Vector3 linePt = pos0;
//	float dotLinePlane = (Vector3::Dot(dirLine, planePt));
//	if (dotLinePlane < 1e-6) {
//		return false;
//	}
//	float t = (Vector3::Dot((planePt - linePt), normPlane)) / dotLinePlane; //Check Edge Parallel
//	ptIntersect = linePt + dirLine * t;
//	return true;
//}

bool CollisionDetection::OBBIntersection(//NOTE: works only with tiny penetration
	const OBBVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	
	//throw NotImplementedException();

	//A
	vector<Vector3> ptsA;
	volumeA.GetPointSet(ptsA, worldTransformA);
	Vector3 forwardA = worldTransformA.GetForward();
	forwardA.Normalise();
	Vector3 upA = worldTransformA.GetUp();
	upA.Normalise();
	Vector3 rightA = Vector3::Cross(forwardA, upA);
	rightA.Normalise();

	//B
	vector<Vector3> ptsB;
	volumeB.GetPointSet(ptsB, worldTransformB);
	Vector3 forwardB = worldTransformB.GetForward();
	forwardB.Normalise();
	Vector3 upB = worldTransformB.GetUp();
	upB.Normalise();
	Vector3 rightB = Vector3::Cross(forwardB, upB);
	rightB.Normalise();

	std::array<Vector3, 15> axises = { forwardA , upA , rightA , forwardB , upB , rightB,
									Vector3::Cross(forwardA, forwardB).Normalised(), Vector3::Cross(forwardA, upB).Normalised(), Vector3::Cross(forwardA, rightB).Normalised(),
									Vector3::Cross(upA, forwardB).Normalised(), Vector3::Cross(upA, upB).Normalised(), Vector3::Cross(upA, rightB).Normalised(),
									Vector3::Cross(rightA, forwardB).Normalised(), Vector3::Cross(rightA, upB).Normalised(), Vector3::Cross(rightA, rightB).Normalised() };


	//std::array<Vector3, 6> axises = { forwardA , upA , rightA , forwardB , upB , rightB };

	float minA, maxA, minB, maxB;
	Vector3 minPtA, maxPtA, minPtB, maxPtB;
	
	Vector3 collisionNormal = Vector3(0, 0, 0);
	float penetration = FLT_MAX;
	Vector3 bestA, bestB;
	Vector3 bestEdgeDirA, bestEdgeDirB;
	CollisionType collisionType;

	vector<Plane> planeSetA;
	vector<Plane> planeSetB;
	volumeA.GetPlaneSet(planeSetA, worldTransformA);
	volumeB.GetPlaneSet(planeSetB, worldTransformB);

	bool isARightBest = false;
	int iBest = -1;
	int idxABest = -1;
	int idxBBest = -1;

	for (int i = 0; i < axises.size(); ++i) {
		Vector3 axis = axises[i];
		SATtest(axis, ptsA, minA, maxA, minPtA, maxPtA);
		SATtest(axis, ptsB, minB, maxB, minPtB, maxPtB);
		if (SegmentOverlap(minA, maxA, minB, maxB)) {
			float curPenetration;
			bool isARight = false;

			if (maxA > maxB) {
				curPenetration = maxB - minA;
				isARight = true;
			}
			else
			{
				curPenetration = maxA - minB;
				isARight = false;
			}
			if (curPenetration < penetration) {
				penetration = curPenetration;
				isARightBest = isARight;
				iBest = i;

				if (isARight) { //A on the right
					bestA = minPtA;
					bestB = maxPtB;
				}
				else { //B on the right
					bestA = maxPtA;
					bestB = minPtB;
				}

				if (i < 6) {
					collisionType = CollisionType_VF;
				}
				else {
					collisionType = CollisionType_EE;
					int idxTmp = i - 6;
					idxABest = idxTmp / 3;
					idxBBest = idxTmp % 3;
					bestEdgeDirA = axises[idxABest];
					bestEdgeDirB = axises[3+ idxBBest];
				}
				
				collisionNormal = axis;
				continue;
			}
		}
		else
		{
			return false;
		}
	}

	Vector3 posA = worldTransformA.GetPosition();
	Vector3 posB = worldTransformB.GetPosition();

	Vector3 localA = Vector3();
	Vector3 localB = Vector3();

	if (collisionType == CollisionType_VF) { //Vertex Face Collision
		Plane planeBest;
		Vector3 vertexBest;
		bool vertFromA = false;
		if (isARightBest) {
			switch (iBest)
			{
			case 0: {
				planeBest = planeSetA[OBBVolumePlaneSet_Back];
				vertexBest = bestB;
				vertFromA = false;
			}break;
			case 1: {
				planeBest = planeSetA[OBBVolumePlaneSet_Bottom];
				vertexBest = bestB;
				vertFromA = false;
			}break;
			case 2: {
				planeBest = planeSetA[OBBVolumePlaneSet_Left];
				vertexBest = bestB;
				vertFromA = false;
			}break;
			case 3: {
				planeBest = planeSetB[OBBVolumePlaneSet_Front];
				vertexBest = bestA;
				vertFromA = true;
			}break;
			case 4: {
				planeBest = planeSetB[OBBVolumePlaneSet_Top];
				vertexBest = bestA;
				vertFromA = true;
			}break;
			case 5: {
				planeBest = planeSetB[OBBVolumePlaneSet_Right];
				vertexBest = bestA;
				vertFromA = true;
			}break;
			default:
				break;
			}
		}
		else
		{
			switch (iBest)
			{
			case 0: {
				planeBest = planeSetB[OBBVolumePlaneSet_Back];
				vertexBest = bestA;
				vertFromA = true;
			}break;
			case 1: {
				planeBest = planeSetB[OBBVolumePlaneSet_Bottom];
				vertexBest = bestA;
				vertFromA = true;
			}break;
			case 2: {
				planeBest = planeSetB[OBBVolumePlaneSet_Left];
				vertexBest = bestA;
				vertFromA = true;
			}break;
			case 3: {
				planeBest = planeSetA[OBBVolumePlaneSet_Front];
				vertexBest = bestB;
				vertFromA = false;
			}break;
			case 4: {
				planeBest = planeSetA[OBBVolumePlaneSet_Top];
				vertexBest = bestB;
				vertFromA = false;
			}break;
			case 5: {
				planeBest = planeSetA[OBBVolumePlaneSet_Right];
				vertexBest = bestB;
				vertFromA = false;
			}break;
			default:
				break;
			}
		}

		Vector3 vertexBest1 = planeBest.ProjectPointOntoPlane(vertexBest); //TODO: Clamp
		if (vertFromA) {
			localA = vertexBest - posA;
			localB = vertexBest1 - posB;
		}
		else
		{
			localA = vertexBest1 - posA;
			localB = vertexBest - posB;
		}
	}
	else
	{//EdgeEdge
		Vector3 ptA, ptB;
		bool isParallel = LineLineNormalProjection(bestEdgeDirA, bestA, bestEdgeDirB, bestB, collisionNormal, ptA);
		LineLineNormalProjection(bestEdgeDirB, bestB, bestEdgeDirA, bestA, collisionNormal, ptB);
		localA = ptA - posA;
		localB = ptB - posB;
		//if (!isParallel) {
		//	LineLineNormalProjection(bestEdgeDirB, bestB, bestEdgeDirA, bestA, collisionNormal, ptB);
		//	localA = ptA - posA;
		//	localB = ptB - posB;
		//}
		//else
		//{
		//	Vector3 toCentA = posA - bestA;
		//	if (Vector3::Dot(bestEdgeDirA, toCentA)<0) {
		//		bestEdgeDirA = -bestEdgeDirA;
		//	}
		//	Vector3 toCentB = posB - bestB;
		//	if (Vector3::Dot(bestEdgeDirB, toCentB) < 0) {
		//		bestEdgeDirB = -bestEdgeDirB;
		//	}
		//	Vector3 halfDimsA = volumeA.GetHalfDimensions();
		//	Vector3 halfDimsB = volumeB.GetHalfDimensions();
		//	Vector3 edgeCentA = bestA + bestEdgeDirA * halfDimsA[2 - idxABest];
		//	Vector3 edgeCentB = bestB + bestEdgeDirB * halfDimsB[2 - idxBBest];
		//	Vector3 edgeCent = (edgeCentA + edgeCentB) / 2.0f;
		//	localA = edgeCent - posA;
		//	localB = edgeCent - posB;
		//}
	}


	Vector3 dirA2B = (worldTransformB.GetPosition() - worldTransformA.GetPosition());
	dirA2B.Normalise();
	if (Vector3::Dot(collisionNormal, dirA2B) < 0) {
		collisionNormal = -collisionNormal;
	}

	collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
	return true;
}

bool NCL::CollisionDetection::AABBOBBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	OBBVolume volumeOBBA(volumeA.GetHalfDimensions());
	Transform worldTransformA0 = worldTransformA;
	worldTransformA0.SetOrientation(Quaternion(0, 0, 0, 0));
	bool isColliding = OBBIntersection(volumeOBBA, worldTransformA0, volumeB, worldTransformB, collisionInfo);
	if (isColliding) {
		collisionInfo.point.localA = Vector3(0, 0, 0);
	}
	return isColliding;
}

bool NCL::CollisionDetection::OBBSphereIntersection(const OBBVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Vector3 posBw = worldTransformB.GetPosition();
	Quaternion rotA = worldTransformA.GetOrientation();
	Quaternion rotAInv = rotA.Conjugate();
	Quaternion rotBInv = worldTransformB.GetOrientation().Conjugate();
	Vector3 translateA = worldTransformA.GetPosition();

	Vector3 delta = rotAInv * (posBw - translateA);
	Vector3 boxSize = volumeA.GetHalfDimensions();
	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);
	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < volumeB.GetRadius()) {
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (volumeB.GetRadius() - distance);

		Vector3 localA = closestPointOnBox;
		collisionNormal = rotA * collisionNormal;
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	return false;

}


bool NCL::CollisionDetection::CapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA, 
	const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	//Capsule A
	Vector3 centA = worldTransformA.GetPosition();
	Matrix4 modelA = worldTransformA.GetMatrix();
	float halfHeightA = volumeA.GetHalfHeight();
	float radA = volumeA.GetRadius();
	Vector3 tipA = modelA * Vector4(Vector3(0, halfHeightA, 0), 1.0f);
	Vector3 baseA = modelA * Vector4(-Vector3(0, halfHeightA, 0), 1.0f);
	Vector3 normalA = (tipA - baseA).Normalised();
	Vector3 lineEndOffsetA = normalA * radA;
	Vector3 aA = baseA + lineEndOffsetA;
	Vector3 bA = tipA - lineEndOffsetA;
	
	//Capsule B
	Vector3 centB = worldTransformB.GetPosition();
	Matrix4 modelB = worldTransformB.GetMatrix();
	float halfHeightB = volumeB.GetHalfHeight();
	float radB = volumeB.GetRadius();
	Vector3 tipB = modelB * Vector4(Vector3(0, halfHeightB, 0), 1.0f);
	Vector3 baseB = modelB * Vector4(-Vector3(0, halfHeightB, 0), 1.0f);
	Vector3 normalB = (tipB - baseB).Normalised();
	Vector3 lineEndOffsetB = normalB * radB;
	Vector3 aB = baseB + lineEndOffsetB;
	Vector3 bB = tipB - lineEndOffsetB;

	//Vector between line endpoints:
	Vector3 v0 = aB - aA;
	Vector3 v1 = bB - aA;
	Vector3 v2 = aB - bA;
	Vector3 v3 = bB - bA;

	//Sqaured distances:
	float d0 = Vector3::Dot(v0, v0);
	float d1 = Vector3::Dot(v1, v1);
	float d2 = Vector3::Dot(v2, v2);
	float d3 = Vector3::Dot(v3, v3);

	//Select bset potential endpoint on capsule A:
	Vector3 bestA;
	if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1) {
		bestA = bA;
	}
	else
	{
		bestA = aA;
	}

	//select point on capsule B nearest to best potential end point on A
	Vector3 bestB = ClosestPointOnLineSegment(aB, bB, bestA);

	//same for capsule A
	bestA = ClosestPointOnLineSegment(aA, bA, bestB);

	float radii = radA + radB;
	Vector3 delta = bestB - bestA;

	float deltaLength = delta.Length();

	if (deltaLength < radii) {
		float penetration = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * radA;
		Vector3 localB = -normal * radB;

		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;
	}
	return false;
}

bool NCL::CollisionDetection::SphereCapsuleIntersection(const SphereVolume& volumeA, const Transform& worldTransformA, 
	const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	//Sphere A
	Vector3 centA = worldTransformA.GetPosition();
	float radA = volumeA.GetRadius();

	//Capsule B
	Vector3 centB = worldTransformB.GetPosition();
	Matrix4 modelB = worldTransformB.GetMatrix();
	float halfHeightB = volumeB.GetHalfHeight();
	float radB = volumeB.GetRadius();
	Vector3 tipB = modelB * Vector4(Vector3(0, halfHeightB, 0), 1.0f);
	Vector3 baseB = modelB * Vector4(-Vector3(0, halfHeightB, 0), 1.0f);
	Vector3 normalB = (tipB - baseB).Normalised();
	Vector3 lineEndOffsetB = normalB * radB;
	Vector3 aB = baseB + lineEndOffsetB;
	Vector3 bB = tipB - lineEndOffsetB;

	Vector3 sphereCentB = ClosestPointOnLineSegment(aB, bB, centA);

	float radii = radA + radB;
	Vector3 delta = sphereCentB - centA;

	float deltaLength = delta.Length();

	if (deltaLength < radii) {
		float penetration = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * radA;
		Vector3 localB = -normal * radB + sphereCentB - centB;

		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;
	}

	return false;
}

bool NCL::CollisionDetection::AABBCapsuleIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, 
	const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	//Capsule
	Vector3 centB = worldTransformB.GetPosition();
	Matrix4 modelB = worldTransformB.GetMatrix();
	float halfHeightB = volumeB.GetHalfHeight();
	float radB = volumeB.GetRadius();
	Vector3 tipB = modelB * Vector4(Vector3(0, halfHeightB, 0), 1.0f);
	Vector3 baseB = modelB * Vector4(-Vector3(0, halfHeightB, 0), 1.0f);
	Vector3 normalB = (tipB - baseB).Normalised();
	Vector3 lineEndOffsetB = normalB * radB;
	Vector3 aB = baseB + lineEndOffsetB;
	Vector3 bB = tipB - lineEndOffsetB;

	/*Ray r(aB, (bB - aB).Normalised());*/

	//Box
	Vector3 boxPos = worldTransformA.GetPosition();
	Vector3 boxSize = volumeA.GetHalfDimensions();

	Vector3 sphereCent = ClosestPointOnLineSegment(aB, bB, boxPos);
	
	Vector3 delta = sphereCent - worldTransformA.GetPosition();
	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);
	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < radB) {
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (radB - distance);

		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * radB;

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	return false;
}

bool NCL::CollisionDetection::OBBCapsuleIntersection(const OBBVolume& volumeA, const Transform& worldTransformA, const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	//Capsule
	Vector3 centB = worldTransformB.GetPosition();
	Matrix4 modelB = worldTransformB.GetMatrix();
	float halfHeightB = volumeB.GetHalfHeight();
	float radB = volumeB.GetRadius();
	Vector3 tipB = modelB * Vector4(Vector3(0, halfHeightB, 0), 1.0f);
	Vector3 baseB = modelB * Vector4(-Vector3(0, halfHeightB, 0), 1.0f);
	Vector3 normalB = (tipB - baseB).Normalised();
	Vector3 lineEndOffsetB = normalB * radB;
	Vector3 aB = baseB + lineEndOffsetB;
	Vector3 bB = tipB - lineEndOffsetB;

	Vector3 aBw = aB;
	Vector3 bBw = bB;

	Quaternion rotA = worldTransformA.GetOrientation();
	Quaternion rotAInv = rotA.Conjugate();
	Quaternion rotBInv = worldTransformB.GetOrientation().Conjugate();
	Vector3 translateA = worldTransformA.GetPosition();

	Vector3 aB0 = rotAInv * (aBw - translateA);
	Vector3 bB0 = rotAInv * (bBw - translateA);

	Vector3 boxSize = volumeA.GetHalfDimensions();
	Vector3 boxPos = worldTransformA.GetPosition();

	Vector3 sphereCent = ClosestPointOnLineSegment(aB0, bB0, boxPos);

	
	Vector3 closestPointOnBox = Maths::Clamp(sphereCent, -boxSize, boxSize);
	Vector3 localPoint = sphereCent - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < volumeB.GetRadius()) {
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (volumeB.GetRadius() - distance);

		Vector3 localA = closestPointOnBox;
		collisionNormal = rotA * collisionNormal;
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	return false;
}
