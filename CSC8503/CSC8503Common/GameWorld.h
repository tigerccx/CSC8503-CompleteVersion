#pragma once
#include <vector>
#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"
#include "LayerMask.h"
#include "WorldHierarchy.h"

namespace NCL {
		class Camera;
		using Maths::Ray;
	namespace CSC8503 {
		class GameObject;
		class Constraint;

		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

		class GameWorld	{
		public:
			GameWorld();
			virtual ~GameWorld();

			virtual void InitWorld() = 0;
			inline bool IsInit() { return isInit; }

			void UpdateHierarchy();
			virtual void UpdateWorld(float dt);

			void Clear();

			void AddGameObject(GameObject* o);
			GameObject* FindGameObject(const std::string& name);
			void FindGameObjects(const std::string& name, std::vector<GameObject*>& gos);
			void RemoveGameObject(GameObject* o, bool andDelete = false);

			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c, bool andDelete = false);

			inline WorldHierarchy* GetHierarchy() { return &hierarchy; };

			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void GetConstraintIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;

			const WorldHierarchy* GetHeirarchy() const {
				return &hierarchy;
			}

		protected:
			WorldHierarchy hierarchy;
			
			std::vector<GameObject*> gameObjects;
			std::vector<Constraint*> constraints;
			
			Camera* mainCamera;

			bool	shuffleConstraints;
			bool	shuffleObjects;
			int		worldIDCounter;

			bool	isInit = false;
		};
	}
}

