#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"

using namespace NCL::CSC8503;

StateMachine::StateMachine()
{
	activeState = nullptr;
}

StateMachine::~StateMachine()
{
	for (auto& i : allStates) {
		delete i;
	}
	for (auto& i : allTransitions) {
		delete i.second;
	}
}

void StateMachine::AddState(State* s) {
	allStates.emplace_back(s);
	if (activeState == nullptr) {
		activeState = s;
		activeState->Enter();
	}
}

void StateMachine::AddTransition(StateTransition* t) {
	allTransitions.insert(std::make_pair(t->GetSourceState(), t));
}

void StateMachine::Enter() {
	activeState->Enter();
}

void StateMachine::Update(float dt) {
	if (activeTransition) { //If transition requires time, do this
		if (activeTransition->Update(dt)) { //If end update. Switch to state update mode.
			activeTransition->EndUpdate();
			State* newState = activeTransition->GetDestinationState();
			activeState = newState;
			activeState->Enter();
			activeTransition = nullptr;
		}
	}
	if (activeState) { //Update active state
		activeState->Update(dt);
	
		//Get the transition set starting from this state node;
		std::pair<TransitionIterator, TransitionIterator> range = allTransitions.equal_range(activeState);

		for (auto& i = range.first; i != range.second; ++i) {
			if (i->second->CanTransition()) {
				if (i->second->RequireUpdate()) {
					//Has transition time->switch to transition update mode. Leave current state. 
					activeTransition = i->second;
					activeTransition->InitUpdate();
					activeState->Leave(); 
					activeState = nullptr; 
				}
				else
				{
					//No transition time->just do the transition
					activeTransition = nullptr;
					State* newState = i->second->GetDestinationState();
					activeState->Leave();
					activeState = newState;
					activeState->Enter();
				}
			}
		}
	}
}

void StateMachine::Leave() {
	if(activeState)
		activeState->Leave();
}