#pragma once
#include "../CSC8503Common/GameWorld.h"

class NavigationController;
class WorldBController;

namespace NCL {
    namespace CSC8503 {
        class NavigationMesh;
    }
}

class GameWorld_B :
    public GameWorld
{
public:
    void InitWorld();

    NavigationController* navController;
    WorldBController* gameController;

protected:
    GameObject* goController;
    GameObject* goMainCharacter;
};

