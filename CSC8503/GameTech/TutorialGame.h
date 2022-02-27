#pragma once


#include <string>
#include <fstream>
#include <iostream>

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"


class UIRendererBase;
class UICanvasBase;

namespace NCL {
	namespace CSC8503 {
		class GameObject;
		class GameWorld;
		class PhysicsSystem;
		class AssetManager;
		class GameTechRenderer;

		class TutorialGame		{
		public:
			~TutorialGame();

			static TutorialGame* GetInstance();

			void PreUpdate();
			void UpdateKeys();
			bool UpdateGame(float dt);

			void SetWorld(GameWorld* world);
			void SetCanvas(UICanvasBase* canvas);
	
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();


			GameTechRenderer*	renderer;
			GameWorld*			world;
			
			UIRendererBase*		uiRenderer;
			UICanvasBase*		uiCanvas;
			
			PhysicsSystem*		physics;
			AssetManager*		assetManager;

			

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Maths::Vector3 lockedOffset		= Maths::Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}
		
		protected:
			TutorialGame();

			GameWorld* __tempWorld = nullptr;
			void __DelayedSetWorld();
			UICanvasBase* __tempCanvas = nullptr;
			void __DelayedSetCanvas();
		};
	}
}

