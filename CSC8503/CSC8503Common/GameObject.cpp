#include "GameObject.h"
#include "CollisionDetection.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName)	{
	name			= objectName;
	worldID			= -1;
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	renderObject	= nullptr;
}

GameObject::~GameObject()	{
	for (Component* itr : componentList) {
		if (itr)
			itr->Deactivate();
		delete itr;
	}

	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}

void GameObject::Update(float dt) {
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->Update(dt);
	}
}

void GameObject::OnSelected() {
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->OnSelected();
	}
}

void GameObject::OnUnSelected() {
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->OnUnSelected();
	}
}

void GameObject::OnCollisionBegin(GameObject* otherObject) {
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->OnCollisionBegin(otherObject);
	}
}

void GameObject::OnCollisionEnd(GameObject* otherObject) {
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->OnCollisionEnd(otherObject);
	}
}