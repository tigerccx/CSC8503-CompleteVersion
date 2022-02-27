#pragma once
#include "../CSC8503Common/Component.h"
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../../Common/Maths.h"
#include "../../Common/Window.h"
#include "IImportable.h"

using namespace NCL;
using namespace NCL::CSC8503;
using namespace NCL::Maths;

class RotatablePlatform :
    public Component, public IImportable
{
public:
    RotatablePlatform() = default;
    ~RotatablePlatform() = default;

    float angleLimit = 10.0f;
    float speed = 0.02;

    virtual void Update(float dt) override;
    virtual void OnSelected() override;
    virtual void OnUnSelected() override;

    void ImportFromJson(Json::Value json);

protected:
    bool selected = false;
};