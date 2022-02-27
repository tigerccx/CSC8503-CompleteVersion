#pragma once
#include <vector>
#include <map>

namespace NCL {
	namespace CSC8503 {

		class State;
		class StateTransition;

		typedef std::multimap<State*, StateTransition*> TransitionContainer;
		typedef TransitionContainer::iterator TransitionIterator;

		class StateMachine	{
		public:
			StateMachine();
			~StateMachine();

			void AddState(State* s);
			void AddTransition(StateTransition* t);

			void Enter();
			void Update(float dt);
			void Leave();

		protected:
			State* activeState;
			StateTransition* activeTransition;

			std::vector<State*> allStates;

			TransitionContainer allTransitions;
		};
	}
}