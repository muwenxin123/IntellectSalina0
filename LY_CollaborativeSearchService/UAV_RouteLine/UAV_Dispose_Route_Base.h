#ifndef __UAV_DISPOS_ROUTE_BASE__
#define __UAV_DISPOS_ROUTE_BASE__

#include <string>
#include <vector>

#include "../AreaSearchAlgorithm.h"

class UAV_Dispose_Route_Base
{
public:
	UAV_Dispose_Route_Base();
	~UAV_Dispose_Route_Base();

public:
	void setUAVID(int _UAVID);
	int  getUAVID();
	void setUAVStrName(std::string _UAVStrName);
	std::string getUAVStrName();
	void setUAVPos(casic_vector3d _UAVPos);
	casic_vector3d getUAVPos();

	void setMarkType(int _targetType);
	int getMarkType();
	void setMarkPos(casic_vector3d _targetPos);
	casic_vector3d getMarkPos();

	void setNoFlyZonePoints(std::vector<LLQPoints> _NoFlyZonePoints);
	std::vector<LLQPoints> getNoFlyZonePoints();

	std::vector<casic_vector3d> getSearchMapPoints();

public:

	void Dispose_route(casic_vector3d start, casic_vector3d goal, double safeDis);

private:
	void sparsePath(std::vector<casic_vector3d>& searchPoints);

private:
	int				UAVID;
	std::string		UAVStrName;
	casic_vector3d	UAVPos;

	int				targetType = 0;
	casic_vector3d	targetPos;

	std::vector<LLQPoints> NoFlyZonePoints;

	std::vector<casic_vector3d> AreaSearchPointMap;

public:
	std::vector<LLQPoints> noFlyAreaPointVec;

};

#endif
