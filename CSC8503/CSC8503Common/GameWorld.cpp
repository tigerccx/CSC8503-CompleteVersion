#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "../../Common/Camera.h"
#include "WorldHierarchy.h"
#include <algorithm>

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld()	{
	mainCamera = new Camera();

	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;

	hierarchy.gameWorld = this;
}

GameWorld::~GameWorld()	{
	Clear();
	delete mainCamera;
}

void GameWorld::Clear() {
	for (auto& i : gameObjects) {
		delete i;
	}
	for (auto& i : constraints) {
		delete i;
	}
	gameObjects.clear();
	constraints.clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
}

GameObject* NCL::CSC8503::GameWorld::FindGameObject(const std::string& name)
{
	for (auto& itr : gameObjects) {
		if (itr->GetName() == name) {
			return itr;
		}
	}
	return nullptr;
}

void NCL::CSC8503::GameWorld::FindGameObjects(const std::string& name, std::vector<GameObject*>& gos)
{
	for (auto& itr : gameObjects) {
		if (itr->GetName() == name) {
			gos.emplace_back(itr);
		}
	}
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete) {
		delete o;
	}
}

void GameWorld::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void NCL::CSC8503::GameWorld::UpdateHierarchy()
{
	if (!hierarchy.IsStaticTreeInited()) {
		hierarchy.UpdateStaticObjectAABBs();
		hierarchy.InitStaticTree();
	}
	hierarchy.UpdateDynamicObjectAABBs();
	hierarchy.BuildDynamicTree();
	hierarchy.UpdateStaticTree();
}

void GameWorld::UpdateWorld(float dt) {
	if (shuffleObjects) {
		std::random_shuffle(gameObjects.begin(), gameObjects.end());
	}

	if (shuffleConstraints) {
		std::random_shuffle(constraints.begin(), constraints.end());
	}

	//Add auto update
	for (auto itr = gameObjects.begin(); itr != gameObjects.end(); ++itr) {
		(*itr)->Update(dt);
	}
}


/*
Constraint Tutorial Stuff
*/

void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
	constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
	if (andDelete) {
		delete c;
	}
}

void GameWorld::GetConstraintIterators(
	std::vector<Constraint*>::const_iterator& first,
	std::vector<Constraint*>::const_iterator& last) const {
	first	= constraints.begin();
	last	= constraints.end();
}