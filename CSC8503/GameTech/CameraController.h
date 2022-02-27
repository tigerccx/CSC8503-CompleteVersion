#pragma once
#include "../CSC8503Common/Component.h"
#include "../../Common/Camera.h"

using namespace NCL;
using namespace NCL::CSC8503;

class CameraController :
    public Component
{
public:
    CameraController(Camera* camera) : camera(camera) {}
    Camera* camera;

    void Update(float dt) override;
};

