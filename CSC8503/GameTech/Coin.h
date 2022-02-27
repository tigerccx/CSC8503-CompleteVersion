#pragma once
#include "../CSC8503Common/Component.h"
#include "../CSC8503Common/GameObject.h"
#include "ControllableCharacter.h"

using namespace NCL;
using namespace NCL::CSC8503;

class Coin :
    public Component
{
public:
    static int coinCount;
    void Update(float dt) override;

    GameObject* goCollector;
protected:
    bool collected = false;
};

