#include "CameraControllerFollow.h"
#include "../../Common/Window.h"
#include "../CSC8503Common/GameObject.h"

void CameraControllerFollow::ResetFollow(GameObject* goFollow, Vector3 offset)
{
	this->goFollow = goFollow;
	this->offset = offset;
}

void CameraControllerFollow::Update(float dt)
{
	if (camera) {
		camera->position = goFollow->GetTransform().GetPosition() + offset;
	}
}
