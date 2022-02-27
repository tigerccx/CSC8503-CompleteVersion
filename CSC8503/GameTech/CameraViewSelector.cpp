#include "CameraViewSelector.h"
#include "../../Common/Window.h"
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/CollisionDetection.h"
#include "TutorialGame.h"
#include "../CSC8503Common/Debug.h"
#include "TransformDisplayer.h"
#include <exception>

void CameraViewSelector::Update(float dt)
{
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT)) {
		if (goSelected) {
			UnselectObject();
		}

		Ray ray = CollisionDetection::BuildRayFromMouse(*camera);

		RayCollision closestCollision;
		if (CollisionDetection::Raycast(ray, closestCollision, TutorialGame::GetInstance()->world, true)) {
			SelectObject((GameObject*)closestCollision.node);
			Debug::DrawLine(ray.GetPosition(), ray.GetPosition() + ray.GetDirection() * closestCollision.rayDistance, Vector4(1, 0, 0, 1), 5.0f);
		}
	}
}

void CameraViewSelector::OnDeactivated()
{
	if (goSelected) {
		UnselectObject();
	}
}

void CameraViewSelector::SelectObject(GameObject* go)
{
	goSelected = go;
	goSelected->OnSelected();
	goSelected->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	goSelected->AddComponent(new TransformDisplayer());
}

void CameraViewSelector::UnselectObject()
{
	auto td = goSelected->RemoveComponent<TransformDisplayer>();
	if (td) {
		delete td;
	}
	goSelected->GetRenderObject()->SetColour(goSelected->tint);
	goSelected->OnUnSelected();
	goSelected = nullptr;
}
