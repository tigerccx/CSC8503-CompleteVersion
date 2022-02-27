#pragma once
#include "../CSC8503Common/Component.h"
#include "../../Common/Camera.h"

using namespace NCL;
using namespace NCL::CSC8503;

class CameraViewSelector :
    public Component
{
public:
    void Update(float dt) override;
    //void OnActivate() override;
    void OnDeactivated() override;

    Camera* camera;

protected:
    void SelectObject(GameObject* go);
    void UnselectObject();

    GameObject* goSelected;
};

