#pragma once
#include "../CSC8503Common/Component.h"

using namespace NCL;
using namespace NCL::CSC8503;

class WorldASuccessController :
    public Component
{
public:
    void Update(float dt) override;
};

