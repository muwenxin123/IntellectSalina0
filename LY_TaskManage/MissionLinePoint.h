#pragma once
#include <vector>
struct MissionLinePoint
{
	char						cLineMode;
	char						cTurnMode;
	char						cLanding;
	char						cHeightCtrl;
	int							nNo;
	int							nLineID;
	double                      dLon;
	double                      dLat;
	double                      dAlt;
	double                      dSpeed;
	double                      dHoverTime;
	double						DRadius;

	MissionLinePoint() :
		nNo(0),
		nLineID(0),
		dLon(0),
		dLat(0),
		dAlt(0),
		dSpeed(0),
		dHoverTime(0),
		cHeightCtrl(1),
		DRadius(0),
		cLineMode(1),
		cTurnMode(1),
		cLanding(0) {};
};
typedef std::vector<MissionLinePoint> MissionLinePointVec;
