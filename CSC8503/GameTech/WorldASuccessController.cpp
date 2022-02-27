#include "WorldASuccessController.h"
#include "TutorialGame.h"
#include "../../Common/Window.h"
#include "GameWorld_Menu.h"
#include "UICanvasMainMenu.h"

void WorldASuccessController::Update(float dt)
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E)) {
		TutorialGame::GetInstance()->SetWorld(new GameWorld_Menu());
		TutorialGame::GetInstance()->SetCanvas(new UICanvasMainMenu());
	}
}
