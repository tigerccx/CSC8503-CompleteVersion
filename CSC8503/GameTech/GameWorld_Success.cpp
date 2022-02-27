#include "GameWorld_Success.h"
#include "WorldASuccessController.h"

void GameWorld_Success::InitWorld()
{
	auto goControl = new GameObject();
	goControl->AddComponent(new WorldASuccessController());
	AddGameObject(goControl);
}
