#include "ManualPusher.h"
#include "TutorialGame.h"
#include "../../Common/UICanvasBase.h"
#include "UIElementDebugWord.h"
#include "SharedMacros.h"

ManualPusher::~ManualPusher()
{
	stateMachine->Leave();
	delete stateMachine;
}

void ManualPusher::ResetState()
{
	posDefault = GetGO()->GetTransform().GetPosition();
	stateMachine = new StateMachine();

	State* stateInitialIdle = new State(
		[&](float dt, void* v) { this->WaitForInteract(dt); },
		nullptr,
		[&](void* v) { tCur = 0.0f; triggered = false; },
		nullptr
	);
	stateInitialIdle->name = "InitIdle";
	State* statePushOut = new State(
		[&](float dt, void* v) { this->PushOut(dt); },
		nullptr,
		[&](void* v) { tCur = 0.0f; },
		[&](void* v) { GetGO()->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0)); posEnd = GetGO()->GetTransform().GetPosition(); }
	);
	statePushOut->name = "PushOut";
	State* stateDrawBack = new State(
		[&](float dt, void* v) { this->DrawBack(dt); },
		nullptr,
		[&](void* v) { tCur = 0.0f; },
		nullptr
	);
	stateDrawBack->name = "DrawBack";

	stateMachine->AddState(stateInitialIdle);
	stateMachine->AddState(statePushOut);
	stateMachine->AddState(stateDrawBack);


	stateMachine->AddTransition(new StateTransition(
		[&](void* a, void* b)->bool {
			return triggered;
		},
		nullptr, nullptr,
			stateInitialIdle, statePushOut
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
			stateDrawBack, stateInitialIdle,
			0, nullptr
			));

	stateMachine->Enter();
}

void ManualPusher::WaitForInteract(float dt)
{
	if (selected) {
		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::MIDDLE)) {
			triggered = true;
		}
	}
}

void ManualPusher::PushOut(float dt)
{
	tCur += dt;
	if (tCur > tPushout) {
		tCur = tPushout;
	}
	float a = pushoutDis / (tPushout * tPushout);
	float v = a * tCur;
	GetGO()->GetPhysicsObject()->SetLinearVelocity(GetGO()->GetTransform().GetForward() * v);
}

void ManualPusher::DrawBack(float dt)
{
	tCur += dt;
	if (tCur > tDrawback) {
		tCur = tDrawback;
	}
	float t = tCur / tDrawback;
	float s = (posEnd - posDefault).Length();
	float dis = (1 - t) * s;
	Vector3 pos = posDefault + GetGO()->GetTransform().GetForward() * dis;
	GetGO()->GetTransform().SetPosition(pos);
}

void ManualPusher::Update(float dt)
{
	stateMachine->Update(dt);
}

void ManualPusher::OnSelected()
{
	selected = true;
	TutorialGame::GetInstance()->uiCanvas->AddUIElement("Tip_2", new UIElementDebugWord("Use (Mouse Middle) to push.", Vector2(UI_ELEMENT_BOTTOM_X_OFFSET, UI_ELEMENT_BOTTOM_Y_OFFSET + UI_ELEMENT_BOTTOM_TIP_Y_OFFSET), UI_ELEMENT_TIP_COLOUR, 15.0f));
}

void ManualPusher::OnUnSelected()
{
	selected = false;
	TutorialGame::GetInstance()->uiCanvas->RemoveUIElement("Tip_2");
}

void ManualPusher::ImportFromJson(Json::Value json)
{
	tPushout = json["tPushout"].asDouble();
	tPushoutWait = json["tPushoutWait"].asDouble();
	tDrawback = json["tDrawback"].asDouble();
	pushoutDis = json["pushoutDis"].asDouble();
}
