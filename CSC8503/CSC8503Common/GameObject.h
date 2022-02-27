#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include "Component.h"

#include "PhysicsObject.h"
#include "RenderObject.h"

#include "LayerMask.h"

#include <vector>
#include <string>

using std::vector;

namespace NCL {
	namespace CSC8503 {
		class GameObject	{
		public:
			GameObject(string name = "GameObject");
			~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool IsActive() const {
				return isActive;
			}

			Vector4 tint = Vector4(1,1,1,1);

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			void SetName(const string& name) {
				this->name = name;
			}

			const string& GetName() const {
				return name;
			}

			inline void SetObjectLayer(const string& name) { this->layerName = name; }
			inline string GetLayerName() const { return layerName; }
			inline Layer GetObjectLayer() const { Layer layer; assert(Layer::GetLayer(layerName, layer)); return layer; }

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();

			inline void SetStatic(bool val){
				isStatic = val;
			}

			inline bool GetStatic() const {
				return isStatic;
			}

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int		GetWorldID() const {
				return worldID;
			}

			//
			//Component
			//
			inline void AddComponent(Component* component, bool toActivate = true) {
				component->SetGO(this);
				componentList.push_back(component);
				component->Activate();
			};
			template<class T> T* RemoveComponent();
			template<class T> T* RemoveComponent(T* component);

			template<class T> T* GetComponent();
			template<class T> bool GetComponents(vector<T*>& components);

			void Update(float dt);
			void OnSelected();
			void OnUnSelected();
			void OnCollisionBegin(GameObject* otherObject);
			void OnCollisionEnd(GameObject* otherObject);

		protected:
			Transform			transform;

			//long long int objectLayer = 1;
			string layerName = "Default";
			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;

			bool	isActive;
			bool	isStatic = false;
			int		worldID;
			string	name = "GameObject";

			Vector3 broadphaseAABB;

			//
			//Component
			//
			vector<Component*> componentList;
		};
		template<class T>
		inline T* GameObject::RemoveComponent()
		{
			size_t idTypeGet = typeid(T).hash_code();
			for (auto itr = componentList.begin(); itr != componentList.end(); ++itr) {
				if ((*itr)->GetTypeID() == idTypeGet) {
					T* ret = (T*)(*itr);
					(*itr)->gameobject = nullptr;
					itr = componentList.erase(itr);
					return ret;
				}
			}
			return nullptr;
		}
		template<class T>
		inline T* GameObject::RemoveComponent(T* component)
		{
			size_t idTypeGet = typeid(T).hash_code();
			for (auto itr = componentList.begin(); itr != componentList.end(); ++itr) {
				if ((*itr)->GetTypeID() == idTypeGet) {
					T* ret = (T*)(*itr);
					if (ret != component)
						continue;
					(*itr)->gameobject = nullptr;
					itr = componentList.erase(itr);
					return ret;
				}
			}
			return nullptr;
		}
		template<class T>
		inline T* GameObject::GetComponent()
		{
			size_t idTypeGet = typeid(T).hash_code();
			for (Component* itr : componentList) {
				if (itr->GetTypeID() == idTypeGet) {
					return (T*)itr;
				}
			}
			return nullptr;
		}
		template<class T>
		inline bool GameObject::GetComponents(vector<T*>& componentsOut)
		{
			size_t size0 = componentsOut.size();
			size_t idTypeGet = typeid(T).hash_code();
			for (Component* itr : componentList) {
				if (itr->GetTypeID() == idTypeGet) {
					componentsOut.emplace_back((T*)itr);
				}
			}
			return componentsOut.size() != size0;
		}
	}
}

