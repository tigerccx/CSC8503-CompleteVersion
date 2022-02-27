#pragma once
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/Component.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "IImportable.h"

using namespace NCL;
using namespace NCL::CSC8503;

class AutoPusher :
	public Component, public IImportable
{
public:
	AutoPusher() = default;
	~AutoPusher();

    float tOffset = 0.3f;
    float tPushout = 0.3f;
    float tPushoutWait = 0.2f;
    float tDrawback = 1.4f;
    float tCD = 3.4f;
    float pushoutDis = 10.0f;


    void ResetState();
    void Wait(float dt);
    void PushOut(float dt);
    void DrawBack(float dt);

    virtual void Update(float dt) override;

    void ImportFromJson(Json::Value json);

protected:
    float tCur = 0.0f;
    Vector3 posDefault;

    StateMachine* stateMachine;
};