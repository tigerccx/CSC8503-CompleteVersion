#pragma once
#include "../CSC8503Common/Component.h"
#include "../../Common/Window.h"

using namespace NCL;
using namespace NCL::CSC8503;

class MainMenuController :
    public Component
{
public:
    void Update(float dt) override;
};

