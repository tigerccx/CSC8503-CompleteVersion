#pragma once
#include "../CSC8503Common/Component.h"
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../../Common/Window.h"
#include "IImportable.h"

using namespace NCL;
using namespace NCL::CSC8503;

class ManualPusher:
	public Component, public IImportable {
public:
	ManualPusher() = default;
	~ManualPusher();

	float tPushout = 0.3f;
	float tPushoutWait = 0.2f;
	float tDrawback = 1.4f;
	float pushoutDis = 10.0f;

	void ResetState();
	void WaitForInteract(float dt);
	void PushOut(float dt);
	void DrawBack(float dt);

	virtual void Update(float dt) override;
	virtual void OnSelected() override;
	virtual void OnUnSelected() override;

	void ImportFromJson(Json::Value json);

protected:
	float tCur = 0.0f;
	Vector3 posDefault;
	Vector3 posEnd;

	bool selected = false;
	bool triggered = false;

	StateMachine* stateMachine;
};