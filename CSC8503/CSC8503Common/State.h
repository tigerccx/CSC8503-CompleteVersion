#pragma once
#include <functional>
#include <string>
namespace NCL {
	namespace CSC8503 {
		class StateMachine;

		class State_		{
		public:
			State_() {}
			virtual ~State_() {}
			virtual void Update(float dt) = 0; //Pure virtual base class
		};

		typedef std::function<void(float, void*)> StateFunc;
		typedef std::function<void(void*)> StateEnterLeaveFunc;

		class State : public State_		{
		public:
			State(StateFunc updateFunc, void* someData, 
				StateEnterLeaveFunc enterFunc=nullptr, StateEnterLeaveFunc leaveFunc=nullptr) {
				funcUpdate		= updateFunc;
				funcData	= someData;
				funcEnter = enterFunc;
				funcLeave = leaveFunc;
			}

			std::string name;

			virtual void Enter() {
				if (funcEnter) {
					funcEnter(funcData);
				}
			}
			virtual void Update(float dt) override {
				funcUpdate(dt, funcData);
			}
			virtual void Leave() {
				if (funcLeave) {
					funcLeave(funcData);
				}
			}
		protected:
			StateFunc funcUpdate;
			StateEnterLeaveFunc funcEnter;
			StateEnterLeaveFunc funcLeave;
			void* funcData;
		};

		class StateGroup : public State {
		public:
			StateGroup(StateMachine* stateMachine);
			StateMachine* stateMachine;
		};
	}
}