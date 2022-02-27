#pragma once
#include "../CSC8503Common/Component.h"
#include "../../Common/Camera.h"

using namespace NCL;
using namespace NCL::CSC8503;

class CameraViewSelector;

class CameraController1 :
    public Component
{
public:
    CameraController1(Camera* camera) : camera(camera) {}
    void Update(float dt) override;

    Camera* camera;
    CameraViewSelector* selector;
};

