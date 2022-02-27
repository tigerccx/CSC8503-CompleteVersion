#pragma once
#include "../CSC8503Common/Component.h"

using namespace NCL;
using namespace NCL::CSC8503;

class PartASuccess :
    public Component
{
public:
    void OnCollisionBegin(GameObject* otherObject) override;
};

