#include "WorldAController.h"
#include "GameWorld_A.h"
#include "TutorialGame.h"
#include "GameWorld_Success.h"
#include "UICanvas_A_Success.h"
#include "../../Common/Window.h"
#include "CameraViewSelector.h"
#include "SharedMacros.h"
#include "UIElementDebugWord.h"

#define UI_ELEMENT_TIME_COUNTER_NAME "__time_count"

void WorldAController::Update(float dt)
{
	timeCount += dt;

	char buf[128];

	auto elementCount = (UIElementDebugWord*)TutorialGame::GetInstance()->uiCanvas->GetUIElement(UI_ELEMENT_TIME_COUNTER_NAME);
	if (!elementCount) {
		elementCount = new UIElementDebugWord("", Vector2(40, 100), UI_ELEMENT_DEFAULT_COLOUR);
		TutorialGame::GetInstance()->uiCanvas->AddUIElement(UI_ELEMENT_TIME_COUNTER_NAME, elementCount);
	}
	snprintf(buf, 128, "Time Took: %.2fs", timeCount);
	elementCount->words = buf;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::R)) {
		ResetScene();
	}
}

void WorldAController::ResetScene()
{
	timeCount = 0;
	CleanUp();
	TutorialGame::GetInstance()->SetWorld(new GameWorld_A());
}

void WorldAController::LoadSuccessScene()
{
	CleanUp();
	TutorialGame::GetInstance()->SetWorld(new GameWorld_Success());
	TutorialGame::GetInstance()->SetCanvas(new UICanvas_A_Success(timeCount));
}

void WorldAController::CleanUp()
{
	auto selector = goCam->GetComponent<CameraViewSelector>();
	if (selector) {
		selector->Deactivate();
	}
}
