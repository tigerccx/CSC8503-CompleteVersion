#pragma once

#include <vector>
#include <set>

#include "../../Common/Pool.h"

#include "OcTree.h"
#include "GameObject.h"

using namespace std;

namespace NCL {
	namespace CSC8503 {
		
		
		class GameWorld;
		class GameObject;
		template<class T> class OcTree;

		class WorldHierarchy
		{
		public:
			WorldHierarchy();
			~WorldHierarchy();

			OcTree<GameObject*>* treeDynamic;
			OcTree<GameObject*>* treeStatic;
			vector<GameObject*> staticObjects;
			GameWorld* gameWorld;

			bool toAllowStaticTreeUpdate = false;

			void SetGameWorld(GameWorld* g);
			void UpdateStaticObjectAABBs();
			void UpdateDynamicObjectAABBs();

			void InitStaticTree();
			inline bool IsStaticTreeInited() const { return isStaticTreeInited; };
			void UpdateStaticTree();

			void BuildDynamicTree();
			void Delete();

		protected:
			bool isStaticTreeInited = false;
		};

		class SingletonWorldHierarchy
		{
		public:
			static WorldHierarchy hierarchy;
		};
	}
}



