#pragma once
#include <string>
#include <fstream>
#include <iostream>

#include "AssetManager.h"
#include "json/json.h"

#include "../CSC8503Common/RotationConstraint.h"
#include "../CSC8503Common/LinearMovementConstraint.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/PositionConstraint1.h"

#include "AutoPusher.h"
#include "ControllableCharacter.h"
#include "KillingPlane.h"
#include "MainBall.h"
#include "ManualPusher.h"
#include "RotatablePlatform.h"
#include "ScoreRecorder.h"
#include "Coin.h"
#include "PartASuccess.h"

using namespace NCL;
using namespace CSC8503;

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
	}
}

class SceneCreator
{
public:
	SceneCreator() = default;
	~SceneCreator() = default;

	inline void SetAssetManager(AssetManager* assetManager) { this->assetManager = assetManager; }
	inline AssetManager* GetAssetManager() const { return assetManager; }
	inline void SetGameWorld(GameWorld* world) { this->world = world; }
	inline GameWorld* GetGameWorld() const { return world; }

	bool LoadSceneFromJson(const string& pathJson);
	bool InitLayerFromJson(const string& pathJson);

protected:
	GameWorld* world;
	AssetManager* assetManager;

	GameObject* AddSphereToWorld(const Vector3& position, float radius, bool isPhysical = true, float inverseMass = 10.0f);
	GameObject* AddHollowSphereToWorld(const Vector3& position, float radius, bool isPhysical = true, float inverseMass = 10.0f);
	GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, bool isPhysical = true, float inverseMass = 10.0f, bool useAABB = false);
	GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, bool isPhysical = true, float inverseMass = 10.0f);


	GameObject* AddAutoPusherToWorld(const Vector3& position, const Vector3& dimensions, const Quaternion& rotation, Json::Value json);
	GameObject* AddManualPusherToWorld(const Vector3& position, const Vector3& dimensions, const Quaternion& rotation, Json::Value json);
	GameObject* AddRotatablePlatform(const Vector3& position, const Vector3& dimensions, const Quaternion& rotation, Json::Value json);
	GameObject* AddControllableCharacter(const Vector3& position, const float& halfHeight, const float& radius, const Quaternion& rotation, Json::Value json, float inverseMass = 10.0f);
	GameObject* AddMainBallToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
	GameObject* AddKillPlaneToWorld(const Vector3& position, Vector3 dimensions);
	GameObject* AddCoinToWorld(const Vector3& position, float radius);
	GameObject* AddFloorToWorld(const Vector3& position);
	GameObject* AddPlayerToWorld(const Vector3& position);
	GameObject* AddEnemyToWorld(const Vector3& position);
	GameObject* AddBonusToWorld(const Vector3& position);
	GameObject* AddPartASuccessToWorld(const Vector3& position, Vector3 dimensions);

	GameObject* AddEnemy0ToWorld(const Vector3& position, const float& halfHeight, const float& radius, const Quaternion& rotation, Json::Value json);
	GameObject* AddEnemy0ToWorld(const vector<Vector2>& pathPoints);

	void InitGameExamples();
	void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
	void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
	void InitMixedGridWorldStatic(int numRows, int numCols, float rowSpacing, float colSpacing);
	void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
	void InitDefaultFloor();
	void BridgeConstraintTest();
	void EngineConstraintTest();

public:
	static SceneCreator instance;
};




