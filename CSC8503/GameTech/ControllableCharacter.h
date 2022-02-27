#pragma once
#include "../CSC8503Common/Component.h"
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Window.h"
#include "IImportable.h"

using namespace NCL;
using namespace NCL::CSC8503;

class ControllableCharacter :
    public Component, public IImportable
{
public:
    ControllableCharacter() = default;
    ~ControllableCharacter() = default;

    float speedScale = 0.5f;

    virtual void Update(float dt) override;
    virtual void OnSelected() override;
    virtual void OnUnSelected() override;

    void ImportFromJson(Json::Value json);

protected:
    bool selected = false;
};

