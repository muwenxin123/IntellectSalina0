#ifndef __DISPOSE_UAVS_MATCH_TARGET__
#define __DISPOSE_UAVS_MATCH_TARGET__

#include "target_UAV_info.h"

#include <vector>
#include <string>

using namespace DISPOSE_ROUTE;

class Dispose_uavsMatchTarget
{

public:
	void setTargets_Selected(const std::vector<targetInfo>& _targets);
	void addTarget_Selected(const targetInfo& _target);
	void delTarget_Selected(const targetInfo& _target);
	void delTarget_Selected(std::string targetID);
	std::vector<targetInfo>::iterator findTargetID(std::string targetID);

	void setUAVS_Selected(std::vector<std::string> uavs);
	void addUAV_Selected(std::string uavID);
	void delUAV_Selected(std::string uavID);
	std::vector<std::string>::iterator findUAVID(std::string uavID);

private:
	std::vector<targetInfo> targets_selected;
	std::vector<std::string>	UAVIDs_selected;
};

#endif