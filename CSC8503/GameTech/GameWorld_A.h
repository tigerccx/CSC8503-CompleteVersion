#pragma once
#include "../CSC8503Common/GameWorld.h"

class WorldAController;

class GameWorld_A :
    public GameWorld
{
public:
    ~GameWorld_A();
    void InitWorld();

    WorldAController* gameController;
    
};

