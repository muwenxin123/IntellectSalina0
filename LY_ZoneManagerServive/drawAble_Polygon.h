#pragma once
#include "drawAble_Base.h"

class drawAble_Polygon :
    public drawAble_Base
{
public:
    drawAble_Polygon();
    ~drawAble_Polygon();

    virtual void OnEarthMessage(int wParam, void *lParam);

};

