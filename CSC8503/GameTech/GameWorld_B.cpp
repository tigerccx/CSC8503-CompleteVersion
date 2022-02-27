#include "GameWorld_B.h"
#include "SceneCreator.h"
#include "CameraController.h"
#include "CameraControllerFollow.h"
#include "CameraTransformDisplayer.h"
#include "NavigationController.h"
#include "EnemyFollow.h"
#include "TransformDisplayer.h"
#include "WorldBController.h"

void GameWorld_B::InitWorld()
{
	std::string path = "../../Scenes/partB.json";
	std::string pathLayers = "../../Scenes/partBLayer.json";

	SceneCreator::instance.SetGameWorld(this);
	SceneCreator::instance.LoadSceneFromJson(path);
	Layer::InitLayerFromJson(pathLayers);

	goMainCharacter = FindGameObject("MainCharacterB");

	// Init Cam
	auto goCam = new GameObject();
	auto controller = new CameraControllerFollow(mainCamera);
	controller->camera = mainCamera;
	controller->ResetFollow(goMainCharacter, Vector3(0, 30, 0));
	goCam->AddComponent(controller);
	mainCamera->SetNearPlane(0.1f);
	mainCamera->SetFarPlane(500.0f);
	mainCamera->SetPitch(-90.0f);
	mainCamera->SetYaw(0.0f);
	goCam->AddComponent(new CameraTransformDisplayer(mainCamera));

	AddGameObject(goCam);


	goController = new GameObject();
	//NavMesh
	std::string pathNavMesh = "partB.navmesh";
	navController = new NavigationController();
	assert(navController->LoadNavigationMap(pathNavMesh));
	goController->AddComponent(navController);
	//World Controller
	gameController = new WorldBController();
	goController->AddComponent(gameController);
	AddGameObject(goController);

	std::vector<GameObject*> enemies;
	FindGameObjects("Enemy", enemies);
	enemies[0]->AddComponent(new TransformDisplayer());
	for (auto& enemy : enemies) {
		auto follow = enemy->GetComponent<EnemyFollow>();
		follow->goFollow = goMainCharacter;
	}

	std::vector<GameObject*> coins;
	FindGameObjects("Coin", coins);
	for (auto& coinGO : coins) {
		auto coin = coinGO->GetComponent<Coin>();
		coin->goCollector = goMainCharacter;
	}
	Coin::coinCount = coins.size();

	isInit = true;
}
