#include "ControllableCharacter.h"
#include "TutorialGame.h"
#include "../../Common/UICanvasBase.h"
#include "SharedMacros.h"
#include "UIElementDebugWord.h"

void ControllableCharacter::Update(float dt)
{
	if (selected) {
		auto physics = GetGO()->GetPhysicsObject();
		float helpImpulseScale = 0.5;
		Vector3 v = physics->GetLinearVelocity();
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			//float scale = (2 - Vector3::Dot(Vector3(0, 0, -1), v))/2.0f * helpImpulseScale;
			//physics->ApplyLinearImpulse(Vector3(0, 0, -1)* scale);
			physics->AddForce(Vector3(0, 0, -10) * speedScale);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			//float scale = (2 - Vector3::Dot(Vector3(0, 0, 1), v)) / 2.0f * helpImpulseScale;
			//physics->ApplyLinearImpulse(Vector3(0, 0, 1) * scale);
			physics->AddForce(Vector3(0, 0, 10) * speedScale);
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			//float scale = (2 - Vector3::Dot(Vector3(-1, 0, 0), v)) / 2.0f * helpImpulseScale;
			//physics->ApplyLinearImpulse(Vector3(-1, 0, 0) * scale);
			physics->AddForce(Vector3(-10, 0, 0) * speedScale);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			//float scale = (2 - Vector3::Dot(Vector3(1, 0, 0), v)) / 2.0f * helpImpulseScale;
			//physics->ApplyLinearImpulse(Vector3(1, 0, 0) * scale);
			physics->AddForce(Vector3(10, 0, 0) * speedScale);
		}
	}
}

void ControllableCharacter::OnSelected()
{
	selected = true;
	TutorialGame::GetInstance()->uiCanvas->AddUIElement("Tip_2", new UIElementDebugWord("Use (Up, Down, Left, Right) to move the character.", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET+ UI_ELEMENT_BOTTOM_TIP_Y_OFFSET), UI_ELEMENT_TIP_COLOUR, 15.0f));
}

void ControllableCharacter::OnUnSelected()
{
	selected = false;
	TutorialGame::GetInstance()->uiCanvas->RemoveUIElement("Tip_2");
}

void ControllableCharacter::ImportFromJson(Json::Value json)
{
	speedScale = json["speedScale"].asDouble();
}
