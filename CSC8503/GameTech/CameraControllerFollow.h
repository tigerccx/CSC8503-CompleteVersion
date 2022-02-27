#pragma once
#include "../CSC8503Common/Component.h"
#include "../../Common/Camera.h"

using namespace NCL;
using namespace NCL::CSC8503;

class CameraControllerFollow :
    public Component
{
public:
    CameraControllerFollow(Camera* camera) : camera(camera) {}
    Camera* camera;

    void ResetFollow(GameObject* goFollow, Vector3 offset);
    void Update(float dt) override;

protected:
    GameObject* goFollow;
    Vector3 offset;
};

