#include "WorldHierarchy.h"

#include "GameWorld.h"
#include "GameObject.h"

NCL::CSC8503::WorldHierarchy::WorldHierarchy() :
	treeDynamic(new OcTree<GameObject*>(Vector3(1024, 1024, 1024), 5, 6)),
	treeStatic(new OcTree<GameObject*>(Vector3(1024, 1024, 1024), 5, 6))
{
}

NCL::CSC8503::WorldHierarchy::~WorldHierarchy()
{
	/*cout << "DELETE WorldHierarchy" << endl;*/
	delete treeDynamic;
	delete treeStatic;
}

void NCL::CSC8503::WorldHierarchy::SetGameWorld(GameWorld* g)
{
	gameWorld = g;
}

void NCL::CSC8503::WorldHierarchy::UpdateStaticObjectAABBs()
{
	gameWorld->OperateOnContents(
		[](GameObject* g) {
			if (g->GetStatic())
				g->UpdateBroadphaseAABB();
		}
	);
}

void NCL::CSC8503::WorldHierarchy::UpdateDynamicObjectAABBs()
{
	gameWorld->OperateOnContents(
		[](GameObject* g) {
			if(!g->GetStatic())
				g->UpdateBroadphaseAABB();
		}
	);
}

void NCL::CSC8503::WorldHierarchy::InitStaticTree()
{
	treeStatic->ClearTree();

	vector<GameObject*>::const_iterator first;
	vector<GameObject*>::const_iterator last;
	gameWorld->GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i) {
		if (!(*i)->GetStatic()) { //Exclude dynamic objects
			continue;
		}

		Vector3 halfSize;
		if (!(*i)->GetBroadphaseAABB(halfSize)) {
			continue;
		}
		Vector3 pos = (*i)->GetTransform().GetPosition();
		treeStatic->Insert(*i, pos, halfSize);
		staticObjects.emplace_back(*i);
	}
	isStaticTreeInited = true;
}

void NCL::CSC8503::WorldHierarchy::UpdateStaticTree()
{
	//TODO: Add efficient-ish support for static tree update (efficient when there is no change)
	if (toAllowStaticTreeUpdate) {
		throw NotImplementedException();

		//vector<GameObject*>::const_iterator first;
		//vector<GameObject*>::const_iterator last;
		//gameWorld->GetObjectIterators(first, last);


		//for (auto i = first; i != last; ++i) {
		//	if (!(*i)->GetStatic()) {
		//		if (staticObjects.find(*i) != staticObjects.end()) {
		//			//treeStatic.RemoveObject
		//		}
		//	}
		//	else
		//	{
		//		if (staticObjects.find(*i) == staticObjects.end()) {
		//			//treeStatic.AddObject
		//		}
		//	}
		//}
	}
}



void NCL::CSC8503::WorldHierarchy::BuildDynamicTree()
{
	treeDynamic->ClearTree();
	
	vector<GameObject*>::const_iterator first;
	vector<GameObject*>::const_iterator last;
	gameWorld->GetObjectIterators(first, last);
	for (auto i = first; i != last; ++i) {
		if ((*i)->GetStatic()) { //Exclude static objects
			continue;
		}
		
		Vector3 halfSize;
		if (!(*i)->GetBroadphaseAABB(halfSize)) {
			continue;
		}
		Vector3 pos = (*i)->GetTransform().GetPosition();
		treeDynamic->Insert(*i, pos, halfSize);
	}
}

void NCL::CSC8503::WorldHierarchy::Delete()
{
	treeDynamic->ClearTree();
	treeStatic->ClearTree();
}


WorldHierarchy SingletonWorldHierarchy::hierarchy = WorldHierarchy();