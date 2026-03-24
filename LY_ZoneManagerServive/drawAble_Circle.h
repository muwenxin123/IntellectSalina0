#pragma once
#include "drawAble_Base.h"

class drawAble_Circle :
    public drawAble_Base
{
public:
    drawAble_Circle();
    ~drawAble_Circle();

    virtual void OnEarthMessage(int wParam, void *lParam);

};

