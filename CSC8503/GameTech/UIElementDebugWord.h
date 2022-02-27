#pragma once
#include <string>

#include "../../Common/UIElementBase.h"
#include "../CSC8503Common/Debug.h"

using namespace NCL;

class UIElementDebugWord :
    public UIElementBase
{
public:
    UIElementDebugWord(std::string words="THIS IS AN UI ELEMENT", Vector2 pos=Vector2(50,50), Vector4 colour=Vector4(1,1,1,1), float size=20.0f, float tempDur=-1) :
        UIElementBase(tempDur), words(words), pos(pos), colour(colour), size(size) {};

    std::string words;
    Vector2 pos;
    Vector4 colour;
    float size;
    void Render() override;
};

