#include "State.h"
#include "StateMachine.h"

NCL::CSC8503::StateGroup::StateGroup(StateMachine* stateMachine) :
	State(
		[&](float dt, void* v) {
			stateMachine->Update(dt);
		},
		nullptr,
			[&](void* v) {
			stateMachine->Enter();
		},
			[&](void* v) {
			stateMachine->Leave();
		}
			)
{

}
