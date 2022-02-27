#include "SceneCreator.h"
#include "../CSC8503Common/GameWorld.h"
#include "../CSC8503Common/GameObject.h"
#include "EnemyFollow.h"
#include "SharedMacros.h"

SceneCreator SceneCreator::instance;

bool SceneCreator::LoadSceneFromJson(const string& pathJson)
{
	std::ifstream ifs(pathJson);
	//ifstream ifs("testjson.json");

	//float invMass = 10.0f;

	if (!ifs.is_open())
	{
		std::cout << "Error opening file\n";
		return false;
	}

	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(ifs, root, false))
	{
		std::cerr << "parse failed \n";
		return false;
	}

	Json::Value goJSONs = root["scene"];
	for (unsigned int i = 0; i < goJSONs.size(); ++i) {
		auto goJSON = goJSONs[i];

		string name = goJSON["name"].asString();
		Vector3 position = Vector3::FromUnityString(goJSON["position"].asString());
		Vector4 rot = Vector4::FromUnityString(goJSON["rotation"].asString());
		Quaternion rotation = Quaternion(rot.x, rot.y, rot.z, rot.w);
		Vector3 scale = Vector3::FromUnityString(goJSON["scale"].asString());

		string goType = goJSON["type"].asString();
		bool isPhysical = goJSON["isPhysical"].asBool();
		float invMass = goJSON["inverseMass"].asDouble();

		string layerName = goJSON["layerName"].asString();

		GameObject* go = nullptr;
		if (goType == "Cube") {
			go = AddCubeToWorld(position, scale / 2.0f, isPhysical, invMass);
		}
		else if (goType == "Sphere")
		{
			go = AddSphereToWorld(position, scale.x / 2.0f, isPhysical, invMass);
		}
		else if (goType == "Capsule") {
			go = AddCapsuleToWorld(position, scale.x, scale.x / 2.0f, isPhysical, invMass);
		}
		else if (goType == "AutoPusher") {
			Json::Value info = goJSON["info"];
			go = AddAutoPusherToWorld(position, scale / 2.0f, rotation, info);
		}
		else if (goType == "ManualPusher")
		{
			Json::Value info = goJSON["info"];
			go = AddManualPusherToWorld(position, scale / 2.0f, rotation, info);
		}
		else if (goType == "RotatablePlatform")
		{
			Json::Value info = goJSON["info"];
			go = AddRotatablePlatform(position, scale / 2.0f, rotation, info);
		}
		else if (goType == "ControllableCharacter")
		{
			Json::Value info = goJSON["info"];
			go = AddControllableCharacter(position, scale.x, scale.x / 2.0f, rotation, info, invMass);
		}
		else if (goType == "MainCharacterB") 
		{
			Json::Value info = goJSON["info"];
			go = AddControllableCharacter(position, scale.x, scale.x / 2.0f, rotation, info, invMass);
			go->SetName("MainCharacter");
			go->tint = GO_COLOUR_MAIN;
			go->GetRenderObject()->SetColour(go->tint);
			go->GetComponent<ControllableCharacter>()->OnSelected();
		}
		else if (goType == "Enemy") {
			Json::Value info = goJSON["info"];
			go = AddEnemy0ToWorld(position, scale.x, scale.x / 2.0f, rotation, info);
		}
		else if (goType == "MainBall")
		{
			go = AddMainBallToWorld(position, scale.x / 2.0f, invMass);
		}
		else if (goType == "KillingPlane")
		{
			go = AddKillPlaneToWorld(position, scale / 2.0f);
		}
		else if (goType == "Coin")
		{
			go = AddCoinToWorld(position, scale.x / 2.0f);
		}
		else if (goType == "Sign")
		{
			go = AddCubeToWorld(position, scale / 2.0f, false, 0);
			go->tint = GO_COLOUR_MAIN;
			go->GetRenderObject()->SetColour(GO_COLOUR_MAIN);
		}
		else if (goType == "PartASuccess")
		{
			go = AddPartASuccessToWorld(position, scale / 2.0f);
		}
		else
		{
			//throw NotImplementedException();
			std::cout << "UNKNOWN TYPE SCENE OBJECT" << std::endl;
		}

		if (go) {
			go->SetName(name);
			go->GetTransform().SetOrientation(rotation);
			go->SetObjectLayer(layerName);
		}
	}

	ifs.close();
	return true;
}

bool SceneCreator::InitLayerFromJson(const string& pathJson)
{
    return false;
}



/*
* Adding things
*/
/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* SceneCreator::AddFloorToWorld(const Vector3& position) {
	Vector3 floorSize = Vector3(100, 2, 100);
	GameObject* go = AddCubeToWorld(position, floorSize, true, 0, true);
	go->SetName("Floor");

	return go;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple'
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* SceneCreator::AddSphereToWorld(const Vector3& position, float radius, bool isPhysical, float inverseMass) {
	GameObject* sphere = new GameObject();
	sphere->SetName("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), 
												assetManager->GetMesh("sphere"), 
												assetManager->GetTexture("basic"), 
												assetManager->GetShader("basic")));
	if (isPhysical) {
		sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

		sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
		sphere->GetPhysicsObject()->InitSphereInertia();
	}

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* SceneCreator::AddHollowSphereToWorld(const Vector3& position, float radius, bool isPhysical, float inverseMass)
{
	GameObject* sphere = new GameObject();
	sphere->SetName("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), 
												assetManager->GetMesh("sphere"), 
												assetManager->GetTexture("basic"), 
												assetManager->GetShader("basic")));
	if (isPhysical) {
		sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

		sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
		sphere->GetPhysicsObject()->InitSphereShellInertia();
	}

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* SceneCreator::AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool isPhysical, float inverseMass,
	bool useAABB) {
	GameObject* cube = new GameObject();
	cube->SetName("Cube");


	if (useAABB) {
		AABBVolume* volume = new AABBVolume(dimensions);
		cube->SetBoundingVolume((CollisionVolume*)volume);
	}
	else
	{
		OBBVolume* volume = new OBBVolume(dimensions);
		cube->SetBoundingVolume((CollisionVolume*)volume);
	}

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), 
											assetManager->GetMesh("cube"), 
											assetManager->GetTexture("basic"),	
											assetManager->GetShader("basic")));
	if (isPhysical) {
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

		cube->GetPhysicsObject()->SetInverseMass(inverseMass);
		cube->GetPhysicsObject()->InitCubeInertia();
	}


	world->AddGameObject(cube);

	return cube;
}

GameObject* SceneCreator::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, bool isPhysical, float inverseMass) {
	GameObject* capsule = new GameObject();
	capsule->SetName("Capsule");

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), 
												assetManager->GetMesh("capsule"), 
												assetManager->GetTexture("basic"), 
												assetManager->GetShader("basic")));

	if (isPhysical) {
		capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

		capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
		capsule->GetPhysicsObject()->InitCubeInertia();
		world->AddConstraint(new RotationConstraint(capsule, capsule->GetTransform().GetOrientation()));
	}

	world->AddGameObject(capsule);

	return capsule;

}

GameObject* SceneCreator::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();
	character->SetName("Player");

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), 
													assetManager->GetMesh("charA"),
													nullptr, 
													assetManager->GetShader("basic")));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), 
													assetManager->GetMesh("charB"), 
													nullptr, 
													assetManager->GetShader("basic")));
	}
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	//lockedObject = character;

	return character;
}

GameObject* SceneCreator::AddEnemyToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();
	character->SetName("Enemy");

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), 
												assetManager->GetMesh("charC"),
												nullptr,
												assetManager->GetShader("basic")));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* SceneCreator::AddBonusToWorld(const Vector3& position) {
	GameObject* bonus = new GameObject();
	bonus->SetName("Bonus");

	SphereVolume* volume = new SphereVolume(0.25f);
	bonus->SetBoundingVolume((CollisionVolume*)volume);
	bonus->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	bonus->SetRenderObject(new RenderObject(&bonus->GetTransform(), 
											assetManager->GetMesh("coin"),
											nullptr,
											assetManager->GetShader("basic")));
	bonus->SetPhysicsObject(new PhysicsObject(&bonus->GetTransform(), bonus->GetBoundingVolume()));

	bonus->GetPhysicsObject()->SetInverseMass(1.0f);
	bonus->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(bonus);

	return bonus;
}

GameObject* SceneCreator::AddPartASuccessToWorld(const Vector3& position, Vector3 dimensions)
{
	GameObject* go = AddCubeToWorld(position, dimensions, true, 0, false);

	PartASuccess* partA = new PartASuccess();
	go->AddComponent(partA);

	go->tint = GO_COLOUR_MAIN;
	go->GetRenderObject()->SetColour(go->tint);

	return go;
}

GameObject* SceneCreator::AddAutoPusherToWorld(const Vector3& position, const Vector3& dimensions, const Quaternion& rotation, Json::Value json)
{
	GameObject* go = AddCubeToWorld(position, dimensions, true, 0, false);
	go->SetName("Pusher");

	AutoPusher* pusher = new AutoPusher();
	go->AddComponent(pusher);
	pusher->ImportFromJson(json);
	pusher->ResetState();

	world->AddConstraint(new RotationConstraint(go, go->GetTransform().GetOrientation()));

	return go;
}

GameObject* SceneCreator::AddManualPusherToWorld(const Vector3& position, const Vector3& dimensions, const Quaternion& rotation, Json::Value json)
{
	GameObject* go = AddCubeToWorld(position, dimensions, true, 0, false);
	go->SetName("ManualPusher");

	ManualPusher* pusher = new ManualPusher();
	go->AddComponent(pusher);
	pusher->ImportFromJson(json);
	pusher->ResetState();

	go->tint = GO_COLOUR_INTERACTIBALE;
	go->GetRenderObject()->SetColour(go->tint);

	return go;
}

GameObject* SceneCreator::AddRotatablePlatform(const Vector3& position, const Vector3& dimensions, const Quaternion& rotation, Json::Value json)
{
	GameObject* go = AddCubeToWorld(position, dimensions, true, 0, false);

	RotatablePlatform* platform = new RotatablePlatform();
	go->AddComponent(platform);
	platform->ImportFromJson(json);

	go->tint = GO_COLOUR_INTERACTIBALE;
	go->GetRenderObject()->SetColour(go->tint);

	return go;
}

GameObject* SceneCreator::AddControllableCharacter(const Vector3& position, const float& halfHeight, const float& radius, const Quaternion& rotation, Json::Value json, float inverseMass)
{
	GameObject* go = AddCapsuleToWorld(position, halfHeight, radius, true, inverseMass);
	
	ControllableCharacter* controllable = new ControllableCharacter();
	go->AddComponent(controllable);
	controllable->ImportFromJson(json);

	world->AddConstraint(new RotationConstraint(go, go->GetTransform().GetOrientation()));
	world->AddConstraint(new LinearMovementConstraint(go, Vector3(0, 1, 0)));

	go->tint = GO_COLOUR_INTERACTIBALE;
	go->GetRenderObject()->SetColour(go->tint);

	return go;
}

GameObject* SceneCreator::AddMainBallToWorld(const Vector3& position, float radius, float inverseMass)
{
	GameObject* go = AddSphereToWorld(position, radius, true, inverseMass);
	go->SetName("MainBall");

	MainBall* mainBall = new MainBall();
	go->AddComponent(mainBall);

	go->tint = GO_COLOUR_MAIN;
	go->GetRenderObject()->SetColour(go->tint);

	return go;
}

GameObject* SceneCreator::AddKillPlaneToWorld(const Vector3& position, Vector3 dimensions)
{
	GameObject* go = AddCubeToWorld(position, dimensions, true, 0, false);
	go->SetName("KillPlane");

	KillingPlane* killingPlane = new KillingPlane();
	go->AddComponent(killingPlane);

	go->tint = Vector4(0.1, 0.1, 0.1, 0);
	go->GetRenderObject()->SetColour(go->tint);

	return go;
}

GameObject* SceneCreator::AddCoinToWorld(const Vector3& position, float radius)
{
	GameObject* go = AddSphereToWorld(position, radius, false, 0);
	go->SetName("Coin");
	
	Coin* coin = new Coin();
	go->AddComponent(coin);

	go->tint = Vector4(1.0, 1.0, 0.2, 0);
	go->GetRenderObject()->SetColour(go->tint);

	return go;
}

GameObject* SceneCreator::AddEnemy0ToWorld(const Vector3& position, const float& halfHeight, const float& radius, const Quaternion& rotation, Json::Value json)
{
	GameObject* go = AddCapsuleToWorld(position, halfHeight, radius, false, 0);
	go->SetName("Enemy");

	EnemyFollow* enemyFollow = new EnemyFollow();
	enemyFollow->ImportFromJson(json);
	go->AddComponent(enemyFollow);

	go->tint = GO_COLOUR_INTERACTIBALE;
	go->GetRenderObject()->SetColour(go->tint);

	EnemyFollow::posY = position.y;

	return go;
}

GameObject* SceneCreator::AddEnemy0ToWorld(const vector<Vector2>& pathPoints)
{
	throw NotImplementedException();
	return nullptr;
	//float meshSize = 1.0f;
	//float inverseMass = 0.5f;

	//GO_MoveAround* character = new GO_MoveAround(pathPoints, 0.1);
	//character->SetName("Enemy");

	//OBBVolume* volume = new OBBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	//character->SetBoundingVolume((CollisionVolume*)volume);

	//character->GetTransform()
	//	.SetScale(Vector3(meshSize, meshSize, meshSize));

	//character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	//character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	//character->GetPhysicsObject()->SetInverseMass(inverseMass);
	//character->GetPhysicsObject()->InitSphereInertia();
	//world->AddConstraint(new LinearMovementConstraint(character, Vector3(0, 1, 0)));

	//world->AddGameObject(character);

	//character->transChaseTarget = &chara->GetTransform();


	//return character;
}




void SceneCreator::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 5; //how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 22;
	float cubeDistance = 20;

	Vector3 startPos = Vector3(50, 50, -50);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, true, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 1) * cubeDistance, 0, 0), cubeSize, true, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, true, invCubeMass);
		PositionConstraint* constraintPosition = new PositionConstraint(previous, block, maxDistance);
		RotationConstraint* constraintRotation = new RotationConstraint(block, block->GetTransform().GetOrientation());
		world->AddConstraint(constraintPosition);
		world->AddConstraint(constraintRotation);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void SceneCreator::EngineConstraintTest()
{
	GameObject* center = AddCubeToWorld(Vector3(0, 40, 0), Vector3(1, 1, 1), true, 0);
	GameObject* outter = AddCubeToWorld(Vector3(0, 30, 0), Vector3(1, 1, 1), true, 10.0f);
	PositionConstraint1* c1 = new PositionConstraint1(center, outter, 10, 1);
	world->AddConstraint(c1);
}

void SceneCreator::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, true, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void SceneCreator::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);
	GameObject* go;
	int count = 0;

	int offset = 0;

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3((x - numCols / 2 + offset) * colSpacing, 20.0f, (z - numRows / 2 + offset) * rowSpacing);

			if (rand() % 2) {
				go = AddCubeToWorld(position, cubeDims);
			}
			else {
				go = AddSphereToWorld(position, sphereRadius);
			}
			go->SetName(go->GetName() + "_" + std::to_string(count));
			//AddCapsuleToWorld(position, 1, sphereRadius);
			count += 1;
		}
	}
}

void SceneCreator::InitMixedGridWorldStatic(int numRows, int numCols, float rowSpacing, float colSpacing)
{
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);
	GameObject* go;
	int count = 0;

	int offset = 0;

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3((x - numCols / 2 - offset) * colSpacing, 10.0f, (z - numRows / 2 - offset) * rowSpacing);

			if (rand() % 2) {
				go = AddCubeToWorld(position, cubeDims, true, 0.0f);
			}
			else {
				go = AddSphereToWorld(position, sphereRadius, true, 0.0f);
			}
			go->SetName(go->GetName() + "_" + std::to_string(count));
			go->SetStatic(true);
			count += 1;
		}
	}
}

void SceneCreator::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols + 1; ++x) {
		for (int z = 1; z < numRows + 1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, true, 1.0f);
		}
	}
}

void SceneCreator::InitDefaultFloor() {
	auto floor = AddFloorToWorld(Vector3(0, -1, 0));
	//auto floor = AddFloorToWorld(Vector3(0, -20, 0));
	floor->SetStatic(true);
}

void SceneCreator::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}