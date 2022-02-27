#include "MainMenuController.h"
#include "TutorialGame.h"
#include "../../Common/UICanvasBase.h"
#include "UIElementDebugWord.h"

#include "UICanvas_A.h"
#include "GameWorld_A.h"

#include "UICanvas_B.h"
#include "GameWorld_B.h"

#include <string>


void MainMenuController::Update(float dt)
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1)) {
		TutorialGame::GetInstance()->SetCanvas(new UICanvas_A());
		TutorialGame::GetInstance()->SetWorld(new GameWorld_A());
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2)) {
		TutorialGame::GetInstance()->SetCanvas(new UICanvas_B());
		TutorialGame::GetInstance()->SetWorld(new GameWorld_B());
	}
}
