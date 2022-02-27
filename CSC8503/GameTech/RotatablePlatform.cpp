#include "RotatablePlatform.h"
#include "TutorialGame.h"
#include "../../Common/UICanvasBase.h"
#include "UIElementDebugWord.h"
#include "SharedMacros.h"

void RotatablePlatform::Update(float dt)
{
	if (selected) {
		Quaternion quad = GetGO()->GetTransform().GetOrientation();
		Vector3 euler = quad.ToEuler();
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			euler.z = Clamp(euler.z + speed * dt, -angleLimit, angleLimit);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			euler.z = Clamp(euler.z - speed * dt, -angleLimit, angleLimit);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			euler.x = Clamp(euler.x + speed * dt, -angleLimit, angleLimit);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			euler.x = Clamp(euler.x - speed * dt, -angleLimit, angleLimit);
		}
		GetGO()->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(euler.x, euler.y, euler.z));
	}
}

void RotatablePlatform::OnSelected()
{
	selected = true;
	TutorialGame::GetInstance()->uiCanvas->AddUIElement("Tip_2", new UIElementDebugWord("Use (Up, Down, Left, Right) to rotate the platform.", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET + UI_ELEMENT_BOTTOM_TIP_Y_OFFSET), UI_ELEMENT_TIP_COLOUR, 15.0f));
}

void RotatablePlatform::OnUnSelected()
{
	selected = false;
	TutorialGame::GetInstance()->uiCanvas->RemoveUIElement("Tip_2");
}

void RotatablePlatform::ImportFromJson(Json::Value json)
{
	angleLimit = json["angleLimit"].asDouble();
	speed = json["speed"].asDouble();
}
