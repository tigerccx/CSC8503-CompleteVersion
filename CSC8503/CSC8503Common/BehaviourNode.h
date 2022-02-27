#pragma once
#include <vector>
#include <string>
#include <functional>

namespace NCL {
	namespace CSC8503 {
		enum BehaviourState {
			BehaviourState_Initialise,
			BehaviourState_Failure,
			BehaviourState_Success,
			BehaviourState_Ongoing
		};
		
		class BehaviourNode
		{
		public:
			BehaviourNode(const std::string& nodeName) {
				currentState = BehaviourState_Initialise;
				name = nodeName;
			}
			virtual ~BehaviourNode(){}
			virtual BehaviourState Execute(float dt) = 0;
			virtual void Reset() { currentState = BehaviourState_Initialise; }

		protected:
			BehaviourState currentState;
			std::string name;
		};

		class BehaviourNodeWithChildren : public BehaviourNode {
		public:
			BehaviourNodeWithChildren(const std::string& nodeName) :
				BehaviourNode(nodeName) {};
			virtual ~BehaviourNodeWithChildren() override {
				for (auto& i : childNodes) {
					delete i;
				}
			}

			virtual void AddChild(BehaviourNode* n) {
				childNodes.emplace_back(n);
			}

			virtual void Reset() override {
				BehaviourNode::Reset();
				for (auto& i : childNodes) {
					i->Reset();
				}
			}

		protected:
			std::vector<BehaviourNode*> childNodes;
		};

		class BehaviourSelector : public BehaviourNodeWithChildren {
		public:
			BehaviourSelector(const std::string& nodeName) :
				BehaviourNodeWithChildren(nodeName) { }
			virtual ~BehaviourSelector(){ }
			virtual BehaviourState Execute(float dt) override {
				//std::cout << "Executing selector " << name << "\n";
				for (auto& i : childNodes) {
					BehaviourState nodeState = i->Execute(dt);
					switch (nodeState)
					{
					case BehaviourState_Failure:
						continue;
					case BehaviourState_Success:
					case BehaviourState_Ongoing: {
							currentState = nodeState;
							return currentState;
						}
					}
				}
				return BehaviourState_Failure;
			}
		};

		class BehaviourSequence : public BehaviourNodeWithChildren {
		public:
			BehaviourSequence(const std::string& nodeName): 
				BehaviourNodeWithChildren(nodeName) { }
			virtual ~BehaviourSequence() { }
			virtual BehaviourState Execute(float dt) override {
				//std::cout << "Executing sequence " << name << "\n";
				for (auto& i : childNodes) {
					BehaviourState nodeState = i->Execute(dt);
					switch (nodeState)
					{
					case BehaviourState_Success:
						continue;
					case BehaviourState_Failure:
					case BehaviourState_Ongoing: {
							currentState = nodeState;
							return currentState;
						}
					}
				}
				return BehaviourState_Success;
			}
		};

		class BehaviourParallel : public BehaviourNodeWithChildren {
		public:
			BehaviourParallel(const std::string& nodeName) :
				BehaviourNodeWithChildren(nodeName) { 
			}
			virtual ~BehaviourParallel() { }

			virtual void AddChild(BehaviourNode* n) override {
				BehaviourNodeWithChildren::AddChild(n);
				executionStates.emplace_back(BehaviourState_Initialise);
			}

			virtual BehaviourState Execute(float dt) override {
				//std::cout << "Executing parallel " << name << "\n";
				bool isAllExecuted = true;
				for (int idx = 0; idx < childNodes.size(); ++idx) {
					auto& i = childNodes[idx];
					switch (executionStates[idx])
					{
					case BehaviourState_Success:
					case BehaviourState_Failure:
						continue;
					}

					BehaviourState nodeState = i->Execute(dt);
					switch (nodeState)
					{
					case BehaviourState_Ongoing: {
							isAllExecuted = false;
							continue;
						}
					case BehaviourState_Success:
					case BehaviourState_Failure: {
							executionStates[idx] = nodeState;
							continue;
						}
					}
				}

				if (isAllExecuted) {
					bool isAllSuccess = true;
					for (int idx = 0; idx < executionStates.size(); ++idx) {
						isAllSuccess = isAllSuccess && (executionStates[idx] == BehaviourState_Success);
					}
					return isAllSuccess ? BehaviourState_Success : BehaviourState_Failure;
				}
				return BehaviourState_Ongoing;
			}

			virtual void Reset() override {
				BehaviourNodeWithChildren::Reset();
				for (auto& state: executionStates) {
					state = BehaviourState_Initialise;
				}
			}

		protected:
			std::vector<BehaviourState> executionStates;
		};

		typedef std::function<BehaviourState(float, BehaviourState)> BehaviourActionFunc;
		class BehaviourAction : public BehaviourNode {
		public:
			BehaviourAction(const std::string& nodeName, BehaviourActionFunc f) :
				BehaviourNode(nodeName) {
				function = f;
			}
			virtual ~BehaviourAction() = default;
			virtual BehaviourState Execute(float dt) override {
				//std::cout << "Executing action " << name << "\n";
				currentState = function(dt, currentState);
				return currentState;
			}
		protected:
			BehaviourActionFunc function;
		};

		typedef std::function<BehaviourState(BehaviourState, void*)> BehaviourDecoratorFunc;
		class BehaviourDecorator : public BehaviourNode {
		public:
			BehaviourDecorator(const std::string& nodeName, BehaviourNode* c, 
				BehaviourDecoratorFunc f, void* fParam) :
				BehaviourNode(nodeName) {
				child = c;
				function = f;
				param = fParam;
			}
			virtual ~BehaviourDecorator() = default;
			virtual BehaviourState Execute(float dt) override {
				//std::cout << "Executing action " << name << "\n";
				currentState = function(child->Execute(dt), param);
				return currentState;
			}

			virtual void Reset() override {
				BehaviourNode::Reset();
				child->Reset();
			}

			static BehaviourState Invertor(BehaviourState state, void* param) {
				switch (state)
				{
				case BehaviourState_Failure:
					return BehaviourState_Success;
				case BehaviourState_Success:
					return BehaviourState_Failure;

				}
				return state;
			}

		protected:
			BehaviourNode* child;
			BehaviourDecoratorFunc function;
			void* param;
		};
	}
}


