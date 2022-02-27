#pragma once
#include "../CSC8503Common/Component.h"
#include "../../Common/Camera.h"

using namespace NCL;
using namespace NCL::CSC8503;

class CameraTransformDisplayer :
    public Component
{
public:
    CameraTransformDisplayer(Camera* cam): camera(cam) {}
    virtual ~CameraTransformDisplayer();
    void Update(float dt) override;
protected:
    Camera* camera;

    void ClearElements();
};

