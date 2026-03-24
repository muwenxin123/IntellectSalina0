#pragma once
#include "drawAble_Base.h"

class drawAble_LineString :
    public drawAble_Base
{
public:
    drawAble_LineString();
    ~drawAble_LineString();

    virtual void OnEarthMessage(int wParam, void *lParam);

};

