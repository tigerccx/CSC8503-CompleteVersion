#pragma once
#include "../CSC8503Common/Component.h"
#include "../../Common/Vector3.h"
#include "../CSC8503Common/NavigationPath.h"
#include "IImportable.h"

using namespace NCL;
using namespace NCL::CSC8503;

class EnemyFollow :
    public Component
{
public:
    void Update(float dt);
    GameObject* goFollow;
    float speedFollow = 0.35f;
    
    float pathFindInterval = 0.1;

    void ImportFromJson(Json::Value json);
    
    static float posY;

protected:
    void ResetNavigationState();

    float timeCur = 0;
    NavigationPath navPath;

    Maths::Vector3 curFrom;
    Maths::Vector3 curTo;
    float curDis = 0;
};

