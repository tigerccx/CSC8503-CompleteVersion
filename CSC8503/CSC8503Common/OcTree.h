#pragma once
#include "../../Common/Vector2.h"
#include "../CSC8503Common/CollisionDetection.h"
#include "Debug.h"
#include <list>
#include <functional>
#include <string>

#include "../../Common/Pool.h"
#include "../../Common/Test.h"

namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		enum OcTreeChildNode {
			OcTreeChildNode_NX_Y_Z,
			OcTreeChildNode_X_Y_Z,
			OcTreeChildNode_NX_NY_Z,
			OcTreeChildNode_X_NY_Z,
			OcTreeChildNode_NX_Y_NZ,
			OcTreeChildNode_X_Y_NZ,
			OcTreeChildNode_NX_NY_NZ,
			OcTreeChildNode_X_NY_NZ
		};

		template<class T>
		class OcTree;

		template<class T>
		struct OcTreeEntry {
			Vector3 pos;
			Vector3 size;
			T object;

			OcTreeEntry() {}

			OcTreeEntry(T obj, Vector3 pos, Vector3 size) {
				object		= obj;
				this->pos	= pos;
				this->size	= size;
			}
		};

		template<class T>
		class OcTreeNode	{
		public:
			typedef std::function<void(std::list<OcTreeEntry<T>>&)> OcTreeFunc;
			typedef std::function<void(OcTreeNode&)> OcTreeNodeFunc;
			typedef std::function<bool(OcTreeNode&)> OcTreeNodeFuncHierarchical;

			OcTreeNode() { children = nullptr; }
			OcTreeNode(Vector3 pos, Vector3 size) {
				children = nullptr;
				this->position = pos;
				this->size = size;
			}
			~OcTreeNode() {
				//cout << "DELETE OcTreeNode" << endl;
				ClearChildren();
			}

		protected:
			friend class OcTree<T>;

			void Insert(T& object, const Vector3& objectPos, const Vector3& objectSize, int depthLeft, int maxSize) {
				if (!CollisionDetection::AABBTest(objectPos, Vector3(position.x, position.y, position.z), objectSize, Vector3(size.x, size.y, size.z))) { //To insert into this node?
					return;
				}
				if (children) { //not a leaf node
					for (int i = 0; i < 8; ++i) {
						children[i]->Insert(object, objectPos, objectSize, depthLeft - 1, maxSize);
					}
				}
				else { //currently a leaf node
					contents.push_back(OcTreeEntry<T>(object, objectPos, objectSize));
					if ((int)contents.size() > maxSize && depthLeft > 0) {
						if (!children) {
							Split();
							//we need to reinsert the contents so far!
							for (const auto& i : contents) {
								for (int j = 0; j < 8; ++j) {
									auto entry = i;
									children[j]->Insert(entry.object, entry.pos, entry.size, depthLeft - 1, maxSize);
								}
							}
							contents.clear();
						}
					}
				}
			}

			void Split() {
				Vector3 halfSize = size / 2.0f;
				children = new OcTreeNode<T>*[8];
				for (int i = 0; i < 8; ++i) {
					children[i] = OcTree<T>::nodePool.GetResource();
				}
				children[0]->Init(position + Vector3(-halfSize.x, halfSize.y, halfSize.z), halfSize);
				children[1]->Init(position + Vector3(halfSize.x, halfSize.y, halfSize.z), halfSize);
				children[2]->Init(position + Vector3(-halfSize.x, -halfSize.y, halfSize.z), halfSize);
				children[3]->Init(position + Vector3(halfSize.x, -halfSize.y, halfSize.z), halfSize);
				children[4]->Init(position + Vector3(-halfSize.x, halfSize.y, -halfSize.z), halfSize);
				children[5]->Init(position + Vector3(halfSize.x, halfSize.y, -halfSize.z), halfSize);
				children[6]->Init(position + Vector3(-halfSize.x, -halfSize.y, -halfSize.z), halfSize);
				children[7]->Init(position + Vector3(halfSize.x, -halfSize.y, -halfSize.z), halfSize);
				/*children[0] = OcTreeNode<T>(position + Vector3(-halfSize.x, halfSize.y, halfSize.z), halfSize); 
				children[1] = OcTreeNode<T>(position + Vector3(halfSize.x, halfSize.y, halfSize.z), halfSize); 
				children[2] = OcTreeNode<T>(position + Vector3(-halfSize.x, -halfSize.y, halfSize.z), halfSize); 
				children[3] = OcTreeNode<T>(position + Vector3(halfSize.x, -halfSize.y, halfSize.z), halfSize); 
				children[4] = OcTreeNode<T>(position + Vector3(-halfSize.x, halfSize.y, -halfSize.z), halfSize); 
				children[5] = OcTreeNode<T>(position + Vector3(halfSize.x, halfSize.y, -halfSize.z), halfSize);
				children[6] = OcTreeNode<T>(position + Vector3(-halfSize.x, -halfSize.y, -halfSize.z), halfSize); 
				children[7] = OcTreeNode<T>(position + Vector3(halfSize.x, -halfSize.y, -halfSize.z), halfSize);*/
			}

			void DebugDraw(Vector4 colour = Vector4(0, 0, 1, 1)) {
				Debug::DrawAABB(position, size, colour, 10.0f);
				if (children) {
					for (int i = 0; i < 8; ++i) {
						children[i]->DebugDraw(colour);
					}
				}
			}

			void DebugPrint(vector<int>& branches) {
				if (children) {
					for (int i = 0; i < 8; ++i) {
						branches.push_back(i);
						children[i]->DebugPrint(branches);
						branches.pop_back();
					}
				}
				else
				{
					if (!contents.empty()) {
						string str = "";
						for (int i = 0; i < branches.size(); ++i) {
							str += std::to_string(branches[i]) + " ";
						}
						str += ": ";
						if (typeid(T) == typeid(GameObject)) {
							for (auto& itr:contents) {
								str += ((GameObject*)(&itr.object))->GetName();
							}
						}
						else
						{
							str += std::to_string(contents.size());
						}
						std::cout<<str<<std::endl;
					}
				}
			}

			void OperateOnContents(OcTreeFunc& funcUpdate) {
				if (children) {
					for (int i = 0; i < 8; ++i) {
						children[i]->OperateOnContents(funcUpdate);
					}
				}
				else
				{
					if (!contents.empty()) {
						funcUpdate(contents);
					}
				}
			}

			void OperateOnNode(OcTreeNodeFunc& funcUpdate) {
				funcUpdate(*this);
				if (children) {
					for (int i = 0; i < 8; ++i) {
						children[i]->OperateOnNode(funcUpdate);
					}
				}
			}

			void OperateOnNodeHierarchical(OcTreeNodeFuncHierarchical& funcUpdate) {
				if(funcUpdate(*this) && children) {
					for (int i = 0; i < 8; ++i) {
						children[i]->OperateOnNodeHierarchical(funcUpdate);
					}
				}
			}

			void ClearChildren() {
				if (children) {
					for (int i = 0; i < 8; ++i) {
						children[i]->ClearChildren();
						children[i]->Init();
						OcTree<T>::nodePool.PutResource(children[i]);
					}
					delete[] children;
				}
				children = nullptr;
			}

			void ClearContents() {
				contents.clear();
			}

		public:
			void Init(Vector3 pos=Vector3(0,0,0), Vector3 size= Vector3(0, 0, 0)) {
				children = nullptr;
				this->position = pos;
				this->size = size;
				ClearContents();
			}
			Vector3 GetPosition() const { return position; }
			Vector3 GetHalfSize() const { return size; }
			//AABBVolume GetAABB() const { return AABBVolume(size); }
			std::list< OcTreeEntry<T> >	contents;
			OcTreeNode<T>** children;

		protected:
			Vector3 position;
			Vector3 size; //Note! It is actually half size.
		};
	}
}


namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		//class WorldHierarchy;

		template<class T>
		class OcTree
		{
		public:
			OcTree(Vector3 size, int maxDepth = 6, int maxSize = 5){
				root = OcTreeNode<T>(Vector3(), size); //Starting from world org
				this->maxDepth	= maxDepth;
				this->maxSize	= maxSize;
			}
			~OcTree() {
			}

			//NOTE: Here we assume the tree is big enough for all objects in the world (or things go wrong because of a lack of insertion status check)
			void Insert(T object, const Vector3& pos, const Vector3& size) {
				root.Insert(object, pos, size, maxDepth, maxSize);
			}

			void DebugDraw(Vector4 colour = Vector4(0, 0, 1, 1)) {
				root.DebugDraw(colour);
			}

			void DebugPrint() {
				vector<int> branches;
				root.DebugPrint(branches);
			}

			void OperateOnContents(typename OcTreeNode<T>::OcTreeFunc  func) {
				root.OperateOnContents(func);
			}

			void OperateOnNode(typename OcTreeNode<T>::OcTreeNodeFunc  func) {
				root.OperateOnNode(func);
			}

			void OperateOnNodeHierarchical(typename OcTreeNode<T>::OcTreeNodeFuncHierarchical  func) {
				root.OperateOnNodeHierarchical(func);
			}

			void ClearTree() {
				root.ClearChildren();
				root.ClearContents();
			}

			static Pool<OcTreeNode<T>> nodePool;

		protected:
			OcTreeNode<T> root;
			int maxDepth;
			int maxSize;
		};
	}
}

template<class T> Pool<OcTreeNode<T>> OcTree<T>::nodePool = Pool<OcTreeNode<T>>();