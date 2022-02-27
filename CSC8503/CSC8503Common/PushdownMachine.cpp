#include "PushdownMachine.h"
#include "PushdownState.h"
using namespace NCL::CSC8503;


bool PushdownMachine::Update(float dt) {
	if (!activeState){ //Initialisation
		stateStack.push(initialState);
		activeState = initialState;
		activeState->OnAwake();
	}
	else {
		PushdownState* newState = nullptr;
		PushdownResult result = activeState->PushdownUpdate(dt, &newState);

		switch (result) {
			case PushdownResult_Pop: {
				activeState->OnSleep();
				delete activeState;
				stateStack.pop();
				if (stateStack.empty()) {
					activeState = nullptr;
					return false;
				}
				else {
					activeState = stateStack.top();
					activeState->OnAwake();
				}
			}break;
			case PushdownResult_Push: {
				activeState->OnSleep();
				stateStack.push(newState);
				activeState = newState;
				activeState->OnAwake();
			}break;
		}
	}
	return true;
}