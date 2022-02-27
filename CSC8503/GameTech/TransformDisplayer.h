#pragma once
#include "../CSC8503Common/Component.h"

using namespace NCL;
using namespace NCL::CSC8503;

class TransformDisplayer :
    public Component
{
public:
    virtual ~TransformDisplayer();
    void Update(float dt) override;
protected:
    void ClearElements();
};

