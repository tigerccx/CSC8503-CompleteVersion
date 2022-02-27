#include "WorldBController.h"
#include "GameWorld_B.h"
#include "UICanvas_B.h"
#include "TutorialGame.h"
#include "GameWorld_Success.h"
#include "UICanvas_A_Success.h"
#include "../../Common/Window.h"
#include "CameraViewSelector.h"
#include "SharedMacros.h"
#include "UIElementDebugWord.h"
#include "Coin.h"

#define UI_ELEMENT_TIME_COUNTER_NAME "__time_count"
#define UI_ELEMENT_COIN_COUNTER_NAME "__coin_count"

void WorldBController::Update(float dt)
{
	timeCount += dt;

	char buf[128];

	auto elementCoinCount = (UIElementDebugWord*)TutorialGame::GetInstance()->uiCanvas->GetUIElement(UI_ELEMENT_COIN_COUNTER_NAME);
	if (!elementCoinCount) {
		elementCoinCount = new UIElementDebugWord("", Vector2(60, 100), UI_ELEMENT_DEFAULT_COLOUR);
		TutorialGame::GetInstance()->uiCanvas->AddUIElement(UI_ELEMENT_COIN_COUNTER_NAME, elementCoinCount);
	}
	snprintf(buf, 128, "Coins Left: %d", Coin::coinCount);
	elementCoinCount->words = buf;

	if (Coin::coinCount == 0) {
		LoadSuccessScene();
	}

	auto elementCount = (UIElementDebugWord*)TutorialGame::GetInstance()->uiCanvas->GetUIElement(UI_ELEMENT_TIME_COUNTER_NAME);
	if (!elementCount) {
		elementCount = new UIElementDebugWord("", Vector2(20, 100), UI_ELEMENT_DEFAULT_COLOUR);
		TutorialGame::GetInstance()->uiCanvas->AddUIElement(UI_ELEMENT_TIME_COUNTER_NAME, elementCount);
	}
	snprintf(buf, 128, "Time Took: %.2fs", timeCount);
	elementCount->words = buf;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::R)) {
		ResetScene();
	}
}

void WorldBController::ResetScene()
{
	timeCount = 0;
	CleanUp();
	TutorialGame::GetInstance()->SetWorld(new GameWorld_B());
}

void WorldBController::LoadSuccessScene()
{
	//throw NotImplementedException();
	CleanUp();
	TutorialGame::GetInstance()->SetWorld(new GameWorld_Success());
	TutorialGame::GetInstance()->SetCanvas(new UICanvas_A_Success(timeCount));
}

void WorldBController::CleanUp()
{
	
}
