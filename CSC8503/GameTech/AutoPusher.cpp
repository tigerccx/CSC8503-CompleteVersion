#include "AutoPusher.h"

AutoPusher::~AutoPusher()
{
	stateMachine->Leave();
	delete stateMachine;
}

void AutoPusher::ResetState()
{
	posDefault = GetGO()->GetTransform().GetPosition();
	stateMachine = new StateMachine();

	State* stateInitialWait = new State(
		[&](float dt, void* v) { this->Wait(dt); },
		nullptr,
		[&](void* v) { tCur = 0.0f; GetGO()->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0)); },
		nullptr
	);
	stateInitialWait->name = "InitWait";
	State* statePushOut = new State(
		[&](float dt, void* v) { this->PushOut(dt); },
		nullptr,
		[&](void* v) { tCur = 0.0f; GetGO()->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0)); },
		nullptr
	);
	statePushOut->name = "PushOut";
	State* stateDrawBack = new State(
		[&](float dt, void* v) { this->DrawBack(dt); },
		nullptr,
		[&](void* v) { tCur = 0.0f; GetGO()->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0)); },
		nullptr
	);
	stateDrawBack->name = "DrawBack";

	stateMachine->AddState(stateInitialWait);
	stateMachine->AddState(statePushOut);
	stateMachine->AddState(stateDrawBack);


	stateMachine->AddTransition(new StateTransition(
		[&](void* a, void* b)->bool {
			return tCur > tOffset;
		},
		nullptr, nullptr,
			stateInitialWait, statePushOut
			));
	stateMachine->AddTransition(new StateTransition(
		[&](void* a, void* b)->bool {
			return tCur >= tPushout;
		},
		nullptr, nullptr,
			statePushOut, stateDrawBack,
			tPushoutWait, nullptr
			));
	stateMachine->AddTransition(new StateTransition(
		[&](void* a, void* b)->bool {
			return tCur >= tDrawback;
		},
		nullptr, nullptr,
			stateDrawBack, statePushOut,
			tCD, nullptr
			));

	stateMachine->Enter();
}

void AutoPusher::Wait(float dt)
{
	tCur += dt;
}

void AutoPusher::PushOut(float dt)
{
	tCur += dt;
	if (tCur > tPushout) {
		tCur = tPushout;
	}
	float t = tCur / tPushout;
	float dis = t * t * pushoutDis;
	Vector3 pos = posDefault + GetGO()->GetTransform().GetForward() * dis;
	GetGO()->GetTransform().SetPosition(pos);
}

void AutoPusher::DrawBack(float dt)
{
	tCur += dt;
	if (tCur > tDrawback) {
		tCur = tDrawback;
	}
	float t = tCur / tDrawback;
	float dis = (1 - t) * pushoutDis;
	Vector3 pos = posDefault + GetGO()->GetTransform().GetForward() * dis;
	GetGO()->GetTransform().SetPosition(pos);
}

void AutoPusher::Update(float dt)
{
	stateMachine->Update(dt);
}

void AutoPusher::ImportFromJson(Json::Value json)
{
	tOffset = json["tOffset"].asDouble();
	tPushout = json["tPushout"].asDouble();
	tPushoutWait = json["tPushoutWait"].asDouble();
	tDrawback = json["tDrawback"].asDouble();
	tCD = json["tCD"].asDouble();
	pushoutDis = json["pushoutDis"].asDouble();
}
