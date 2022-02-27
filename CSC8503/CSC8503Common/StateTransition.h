#pragma once
#include <functional>
namespace NCL {
	namespace CSC8503 {

		class State_;

		class StateTransition_
		{
		public:
			virtual bool CanTransition() const = 0;

			State_* GetDestinationState()  const {
				return destinationState;
			}

			State_* GetSourceState() const {
				return sourceState;
			}

		protected:
			State_ * sourceState;
			State_ * destinationState;
		};

		class State;

		class StateTransition : public StateTransition_
		{
		public:
			typedef std::function<bool(void*, void*)> TransitionFunc;
			typedef std::function<bool(float)> TransitionUpdateFunc; //If return true, end transition. If return false, keep updating till time's up.
			StateTransition(TransitionFunc f, void* testData, void* otherData, State_* srcState, State_* destState,
							float time = 0.0f, TransitionUpdateFunc fUpdate = nullptr) :
				dataA(testData), dataB(otherData)
			{
				funcJudgement				= f;
				sourceState			= srcState;		//
				destinationState	= destState;

				transitionTime = time;
				funcUpdate = fUpdate;
			}
			~StateTransition() {}

			State* GetDestinationState()  const {
				return (State*)destinationState;
			}

			State* GetSourceState() const {
				return (State*)sourceState;
			}

			virtual bool CanTransition() const override{
				if (funcJudgement) {
					return funcJudgement(dataA, dataB);
				}
				return false;
			}

			bool RequireUpdate() const {
				return transitionTime > 0.0f;
			}

			virtual void InitUpdate() {
				_transitionTimeCur = 0.0f;
			}

			virtual bool Update(float dt) { //If return true, end transition. If return false, keep updating till time's up.
				_transitionTimeCur += dt;
				float deltaTime = dt;
				bool endTransition = false;
				if (_transitionTimeCur < transitionTime) {
					deltaTime = dt;
				}
				else
				{
					endTransition = true;
					deltaTime = transitionTime - _transitionTimeCur + dt;
				}

				if (funcUpdate) {
					endTransition = endTransition || funcUpdate(deltaTime);
				}

				return endTransition;
			}

			virtual void EndUpdate() {
				//Dunno what to do but... leave the API here I guess
			}


			template<class T>
			static bool GreaterThanTransition(void* dataA, void* dataB) {
				return *((T*)dataA) > *((T*)dataB);
			}
			template<class T>
			static bool LessThanTransition(void* dataA, void* dataB) {
				return *((T*)dataA) < *((T*)dataB);
			}
			template<class T>
			static bool EqualsTransition(void* dataA, void* dataB) {
				return *((T*)dataA) == *((T*)dataB);
			}
			template<class T>
			static bool NotEqualsTransition(void* dataA, void* dataB) {
				return *((T*)dataA) != *((T*)dataB);
			}

		protected:
			TransitionFunc  funcJudgement;
			void* dataA;
			void* dataB;

			float transitionTime;
			TransitionUpdateFunc funcUpdate;

			float _transitionTimeCur = 0.0f;
		};
	}
}

