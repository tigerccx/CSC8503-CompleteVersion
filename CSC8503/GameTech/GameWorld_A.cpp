#include "GameWorld_A.h"
#include "SceneCreator.h"
#include "CameraController.h"
#include "CameraController1.h"
#include "CameraViewSelector.h"
#include "CameraTransformDisplayer.h"
#include "WorldAController.h"

GameWorld_A::~GameWorld_A()
{
	gameController->CleanUp();
}

void GameWorld_A::InitWorld()
{
	std::string path = "../../Scenes/partA.json";
	std::string pathLayers = "../../Scenes/partALayer.json";

	SceneCreator::instance.SetGameWorld(this);
	SceneCreator::instance.LoadSceneFromJson(path);
	Layer::InitLayerFromJson(pathLayers);

	// Init Cam
	auto goCam = new GameObject();
	auto controller = new CameraController1(mainCamera);
	auto selector = new CameraViewSelector();
	selector->camera = mainCamera;
	controller->selector = selector;
	controller->camera = mainCamera;
	goCam->AddComponent(controller);
	goCam->AddComponent(selector);
	mainCamera->SetNearPlane(0.1f);
	mainCamera->SetFarPlane(500.0f);
	mainCamera->SetPitch(-40.0f);
	mainCamera->SetYaw(329.83f);
	mainCamera->SetPosition(Vector3(6.46, 61.46, 11.51));
	goCam->AddComponent(new CameraTransformDisplayer(mainCamera));

	AddGameObject(goCam);


	auto goGameController = new GameObject();
	gameController = new WorldAController();
	gameController->goCam = goCam;
	goGameController->AddComponent(gameController);

	AddGameObject(goGameController);

	isInit = true;
}


