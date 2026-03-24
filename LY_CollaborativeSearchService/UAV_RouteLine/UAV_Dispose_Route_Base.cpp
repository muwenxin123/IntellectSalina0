#include "UAV_Dispose_Route_Base.h"
#include "LY_CollaborativeSearchService.h"

UAV_Dispose_Route_Base::UAV_Dispose_Route_Base()
{
}

UAV_Dispose_Route_Base::~UAV_Dispose_Route_Base()
{
}

void UAV_Dispose_Route_Base::setUAVID(int _UAVID)
{
	UAVID = _UAVID;
}

int UAV_Dispose_Route_Base::getUAVID()
{
	return UAVID;
}

void UAV_Dispose_Route_Base::setUAVStrName(std::string _UAVStrName)
{
	UAVStrName = _UAVStrName;
}

std::string UAV_Dispose_Route_Base::getUAVStrName()
{
	return UAVStrName;
}

void UAV_Dispose_Route_Base::setUAVPos(casic_vector3d _UAVPos)
{
	UAVPos = _UAVPos;
}

casic_vector3d UAV_Dispose_Route_Base::getUAVPos()
{
	return UAVPos;
}

void UAV_Dispose_Route_Base::setMarkType(int _targetType)
{
	targetType = _targetType;
}

int UAV_Dispose_Route_Base::getMarkType()
{
	return targetType;
}

void UAV_Dispose_Route_Base::setMarkPos(casic_vector3d _targetPos)
{
	targetPos = _targetPos;
}

casic_vector3d UAV_Dispose_Route_Base::getMarkPos()
{
	return targetPos;
}

void UAV_Dispose_Route_Base::setNoFlyZonePoints(std::vector<LLQPoints> _NoFlyZonePoints)
{
	NoFlyZonePoints = _NoFlyZonePoints;
}

std::vector<LLQPoints> UAV_Dispose_Route_Base::getNoFlyZonePoints()
{
	return NoFlyZonePoints;
}

std::vector<casic_vector3d> UAV_Dispose_Route_Base::getSearchMapPoints()
{
	return AreaSearchPointMap;
}

void UAV_Dispose_Route_Base::Dispose_route(casic_vector3d start, casic_vector3d goal, double safeDis)
{
	auto sg1 = casic_vector3d(start.x, goal.y, goal.z);
	auto sg2 = casic_vector3d(goal.x, start.y, goal.z);
	QGeoCoordinate point1(start.y, start.x);
	QGeoCoordinate point2(sg1.y, sg1.x);
	QGeoCoordinate point3(goal.y, goal.x);
	QGeoCoordinate point4(sg2.y, sg2.x);

	double length_ = point1.distanceTo(point2);
	double width_ = point1.distanceTo(point4);
	double alt_ = goal.z;

	AStar astar;
	astar.setNoflyZonePoints(NoFlyZonePoints);
	astar.noFlyAreaPointVec = noFlyAreaPointVec;

	astar.init(length_, width_, alt_, safeDis, start, goal);
	astar.pathPlan(AreaSearchPointMap);
	sparsePath(AreaSearchPointMap);

}

void UAV_Dispose_Route_Base::sparsePath(std::vector<casic_vector3d>& searchPoints)
{
	if (searchPoints.empty())
		return;
	double angleDiffMax = 10;

	std::vector<casic_vector3d>* tempPath = &(searchPoints);
	auto pointItor = tempPath->begin();
	QGeoCoordinate point1(pointItor->y, pointItor->x, pointItor->z);
	pointItor++;

	if (pointItor == tempPath->end())
	{
		return;
	}

	auto point2Itor = pointItor;
	QGeoCoordinate point2(pointItor->y, pointItor->x, pointItor->z);
	pointItor++;
	for (pointItor; pointItor != tempPath->end(); )
	{
		QGeoCoordinate point3(pointItor->y, pointItor->x, pointItor->z);
		double angle1_2 = point1.azimuthTo(point2);
		double angle2_3 = point2.azimuthTo(point3);
		double angleDiff = fabs(angle1_2 - angle2_3);
		if (angleDiff < angleDiffMax)
		{
			pointItor = tempPath->erase(point2Itor);
			point2 = point3;
			point2Itor = pointItor;
			pointItor++;
		}
		else
		{
			point1 = point2;
			point2 = point3;
			point2Itor = pointItor;
			pointItor++;
		}
	}

}

