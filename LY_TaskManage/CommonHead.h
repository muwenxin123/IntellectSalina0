#pragma once
#include "QColor"

#define SCENARIO_TRACK_COLOR_UNKNOW		QColor(141,131,124)
#define SCENARIO_TRACK_COLOR_ENEMY		QColor(0,0,0)
#define SCENARIO_TRACK_COLOR_ME			QColor(250,0, 0)
#define SCENARIO_TRACK_COLOR_FRIEND		QColor(0,0,255)
#define SCENARIO_TRACK_COLOR_AEROPLANE	QColor(105,182,80)

static QColor GetScenarioAttributeColor(int nAttribute)
{
    if (0 == nAttribute)
    {
        return SCENARIO_TRACK_COLOR_UNKNOW;
    }
    else if (1 == nAttribute)
    {
        return SCENARIO_TRACK_COLOR_ENEMY;
    }
    else if (2 == nAttribute)
    {
        return SCENARIO_TRACK_COLOR_ME;
    }
    else if (3 == nAttribute)
    {
        return SCENARIO_TRACK_COLOR_FRIEND;
    }
    else if (4 == nAttribute)
    {
        return SCENARIO_TRACK_COLOR_AEROPLANE;
    }
    return SCENARIO_TRACK_COLOR_UNKNOW;
}

