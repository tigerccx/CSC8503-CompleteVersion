#include "GameWorld_Menu.h"
#include "MainMenuController.h"

void GameWorld_Menu::InitWorld()
{
	auto goControl = new GameObject();
	goControl->AddComponent(new MainMenuController());

	AddGameObject(goControl);
}
