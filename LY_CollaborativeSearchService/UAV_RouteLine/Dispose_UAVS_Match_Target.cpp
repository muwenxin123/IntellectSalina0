#include "Dispose_UAVS_Match_Target.h"

void Dispose_uavsMatchTarget::setTargets_Selected(const std::vector<targetInfo>& _targets)
{
	targets_selected.clear();
	targets_selected = _targets;
}

void Dispose_uavsMatchTarget::addTarget_Selected(const targetInfo& _target)
{
	auto _targetItor = findTargetID(_target.targetID);
	if (_targetItor != targets_selected.end())
	{
		*_targetItor = _target;
	}
	else
	{
		targets_selected.push_back(_target);
	}
}

void Dispose_uavsMatchTarget::delTarget_Selected(const targetInfo & _target)
{
	delTarget_Selected(_target.targetID);

}

void Dispose_uavsMatchTarget::delTarget_Selected(std::string targetID)
{
	auto _targetItor = findTargetID(targetID);
	if (_targetItor != targets_selected.end())
	{
		_targetItor = targets_selected.erase(_targetItor);
	}
}

std::vector<targetInfo>::iterator Dispose_uavsMatchTarget::findTargetID(std::string _targetID)
{
	auto targetItor = targets_selected.begin();
	while (targetItor != targets_selected.end())
	{
		if (targetItor->targetID == _targetID)
		{
			return targetItor;
		}
		targetItor++;
	}
	return targetItor;
}

void Dispose_uavsMatchTarget::setUAVS_Selected(std::vector<std::string> uavs)
{
	UAVIDs_selected = uavs;
}

void Dispose_uavsMatchTarget::addUAV_Selected(std::string uavID)
{
	auto uavIDItor = findUAVID(uavID);
	if (uavIDItor == UAVIDs_selected.end())
	{
		UAVIDs_selected.push_back(uavID);
	}
}

void Dispose_uavsMatchTarget::delUAV_Selected(std::string uavID)
{
	auto uavIDItor = findUAVID(uavID);
	if (uavIDItor != UAVIDs_selected.end())
	{
		uavIDItor = UAVIDs_selected.erase(uavIDItor);
	}
}

std::vector<std::string>::iterator Dispose_uavsMatchTarget::findUAVID(std::string uavID)
{
	return std::find(UAVIDs_selected.begin(), UAVIDs_selected.end(), uavID);
}
