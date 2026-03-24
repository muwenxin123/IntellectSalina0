#ifndef __TARGET_UAV_INFO__
#define __TARGET_UAV_INFO__

#include <string>
#include <vector>
namespace DISPOSE_ROUTE
{

	enum DISPOSE_WAY
	{
		DISPOSE_THROW = 0,
		DISPOSE_FIRE,
	};

	enum DISPOSE_ROUTE_FIRST
	{
		DISPOSE_ROUTE_FIRST_TIME = 0,
		DISPOSE_ROUTE_FIRST_SAFE,
	};

	struct targetInfo
	{
		std::string targetID;
		double		longitude;
		double		latitude;
		float		altitude;
		int			size_long = 3;
		int			size_width = 3;
	};

	struct UAVInfo
	{
		std::string uavID;
		DISPOSE_WAY	uavType = DISPOSE_THROW;
		double		longitude;
		double		latitude;
		float		altitude;
		double		longitude_fire;
		double		latitude_fire;
		float		altitude_fire;
		int			dispose_heiOrDis = 30;
	};

	struct target_UAVS_Info
	{
		targetInfo				target_info;
		std::vector<UAVInfo>	uavsInfos;
	};

}

#endif
