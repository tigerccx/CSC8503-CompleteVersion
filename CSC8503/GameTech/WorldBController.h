#pragma once
#include "../CSC8503Common/Component.h"

using namespace NCL;
using namespace NCL::CSC8503;

class WorldBController :
    public Component
{
public:
    void Update(float dt) override;
    void ResetScene();
    void LoadSuccessScene();
    void CleanUp();

protected:
    float timeCount = 0;
};

