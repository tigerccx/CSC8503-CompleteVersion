#pragma once

#include <typeinfo>

namespace NCL {
	namespace CSC8503 {
		class GameObject;

		class Component
		{
		public:
			Component() { Init(); } //TODO: determine the pipeline order
			Component(const Component& other) { Init(); }
			Component& operator=(const Component& other) { Init(); }
			virtual ~Component() { OnTerminated(); }

			virtual void Start() {}; //When starting to function
			virtual void Update(float dt) {};   //Called each frame

			virtual void OnDeactivated() {};
			virtual void OnActivated() {}; //When activated (attached to GO or activated manually)
			virtual void OnTerminated() {};
			virtual void OnSelected() {};
			virtual void OnUnSelected() {};
			virtual void OnCollisionBegin(GameObject* otherObject) {};
			virtual void OnCollisionEnd(GameObject* otherObject) {};


			inline void Activate() { if (!isActive) { isActive = true; OnActivated(); } }
			inline void Deactivate() { if (isActive) { isActive = false; OnDeactivated(); } }
			inline bool IsActive() const { return isActive; }
			inline bool IsManaged() const { return isManaged; }

			inline GameObject* GetGO() const { return gameobject; }
			friend class GameObject;

			virtual const size_t GetTypeID() const { return typeid(*this).hash_code(); }

		protected:
			GameObject* gameobject;
			bool isActive = false;
			bool isStarted = false;
			bool isManaged = false;

			virtual void Init() { gameobject = nullptr; }

			inline void SetGO(GameObject* go) { gameobject = go; }
		};
	}
}
