#pragma once
#include <string>

#include "State.h"

namespace NCL {
	namespace CSC8503 {
		enum PushdownResult {
			PushdownResult_Push, PushdownResult_Pop, PushdownResult_NoChange
		};

		class PushdownState :
			public State_
		{
		public:
			PushdownState();
			~PushdownState();

			std::string name;

			virtual void Update(float dt) override {};
			virtual PushdownResult PushdownUpdate(float dt, PushdownState** pushResult) = 0;
			virtual void PushdownReset() {}

			virtual void OnAwake() {} //By default do nothing
			virtual void OnSleep() {} //By default do nothing
		};
	}
}

