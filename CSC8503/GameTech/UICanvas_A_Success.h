#pragma once
#include "UIElementDebugWord.h"
#include "../../Common/UICanvasBase.h"

class UICanvas_A_Success :
    public UICanvasBase
{
public:
    UICanvas_A_Success(float timeCount):timeCount(timeCount){}
    void Init() override;
protected:
    float timeCount = 0;
};

