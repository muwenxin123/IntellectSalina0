#pragma once
#include "drawAble_Base.h"

class drawAble_Rect :
    public drawAble_Base
{
public:
    drawAble_Rect();
    ~drawAble_Rect();

    virtual void OnEarthMessage(int wParam, void *lParam);

private:
    bool point2isRight = true;

    osgEarth::Annotation::FeatureNode *tempLineNode = nullptr;

};

