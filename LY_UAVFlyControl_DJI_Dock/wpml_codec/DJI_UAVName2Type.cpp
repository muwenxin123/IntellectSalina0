#include "DJI_UAVName2Type.h"

long long BitwiseCombiner::combine(int a, int b, int c)
{
	if (a < 0 || a > 999 || b < 0 || b > 999 || c < 0 || c > 999)
	{
		return -1;
	}
	else
	{
		return (static_cast<long long>(a) << (2 * BITS)) |
			(static_cast<long long>(b) << BITS) |
			static_cast<long long>(c);
	}
}

void BitwiseCombiner::extract(long long combined, int & a, int & b, int & c)
{
	a = (combined >> (2 * BITS)) & MASK;
	b = (combined >> BITS) & MASK;
	c = combined & MASK;
}

int BitwiseCombiner::getFirst(long long combined)
{
	return (combined >> (2 * BITS)) & MASK;
}

int BitwiseCombiner::getSecond(long long combined)
{
	return (combined >> BITS) & MASK;
}

int BitwiseCombiner::getThird(long long combined)
{
	return combined & MASK;
}

DroneEnum2Type::DroneEnum2Type()
{
	init();
	combine();
}

bool DroneEnum2Type::getTypeIDFromDroneName(std::string droneName, int & domain, int & type, int & sub_type)
{
	auto nameDroneEnumsItor = nameDroneEnums.find(droneName);
	if (nameDroneEnumsItor != nameDroneEnums.end())
	{
		BitwiseCombiner::extract(nameDroneEnumsItor->second, domain, type, sub_type);
		return true;
	}
	else
	{
		return false;
	}
}

bool DroneEnum2Type::getDroneNameFromTypeID(int domain, int type, int sub_type, std::string & droneName)
{
	auto typeDroneEnumsItor = typeDroneEnums.find(BitwiseCombiner::combine(domain, type, sub_type));
	if (typeDroneEnumsItor != typeDroneEnums.end())
	{
		droneName = typeDroneEnumsItor->second;
		return true;
	}
	else
	{
		return false;
	}
}

std::map<std::string, long long> DroneEnum2Type::getNames()
{
	return nameDroneEnums;
}

void DroneEnum2Type::init()
{
	droneTypes.push_back(DroneAndType("Matrice 400",		0, 103, 0));
	droneTypes.push_back(DroneAndType("Matrice 350 RTK",	0, 89, 0));
	droneTypes.push_back(DroneAndType("Matrice 300 RTK",	0, 60, 0));
	droneTypes.push_back(DroneAndType("Matrice 30",			0, 67, 0));
	droneTypes.push_back(DroneAndType("Matrice 30T",		0, 67, 1));
	droneTypes.push_back(DroneAndType("Mavic 3 行业系列(M3E相机)", 0, 77, 0));
	droneTypes.push_back(DroneAndType("Mavic 3 行业系列(M3T相机)", 0, 77, 1));
	droneTypes.push_back(DroneAndType("Mavic 3 行业系列(M3TA相机)", 0, 77, 3));
	droneTypes.push_back(DroneAndType("Matrice 3D",			0, 91, 0));
	droneTypes.push_back(DroneAndType("Matrice 3TD",		0, 91, 1));
	droneTypes.push_back(DroneAndType("Matrice 4D",			0, 100, 0));
	droneTypes.push_back(DroneAndType("Matrice 4TD",		0, 100, 1));
	droneTypes.push_back(DroneAndType("Matrice 4 系列(M4E相机)", 0, 99, 0));
	droneTypes.push_back(DroneAndType("Matrice 4 系列(M4T相机)", 0, 99, 1));
	droneTypes.push_back(DroneAndType("大疆机场",			3, 1, 0));
	droneTypes.push_back(DroneAndType("大疆机场 2",			3, 2, 0));
	droneTypes.push_back(DroneAndType("大疆机场 3",			3, 3, 0));
}

void DroneEnum2Type::combine()
{
	for (int i = 0; i < droneTypes.size(); i++)
	{
		long long type = BitwiseCombiner::combine(droneTypes[i].domain, droneTypes[i].type, droneTypes[i].sub_type);
		if (type >= 0)
		{
			typeDroneEnums[type] = droneTypes[i].DroneName;
			nameDroneEnums[droneTypes[i].DroneName] = type;
		}
	}
}

PayloadEnum2Type::PayloadEnum2Type()
{
	init();
	combine();
}

bool PayloadEnum2Type::getTypeIDFromPayloadName(std::string droneName, int & domain, int & type, int & sub_type)
{
	auto nameDroneEnumsItor = namePayloadEnums.find(droneName);
	if (nameDroneEnumsItor != namePayloadEnums.end())
	{
		BitwiseCombiner::extract(nameDroneEnumsItor->second, domain, type, sub_type);
		return true;
	}
	else
	{
		return false;
	}
}

bool PayloadEnum2Type::getPayloadNameFromTypeID(int domain, int type, int sub_type, std::string & droneName)
{
	auto typeDroneEnumsItor = typePayloadEnums.find(BitwiseCombiner::combine(domain, type, sub_type));
	if (typeDroneEnumsItor != typePayloadEnums.end())
	{
		droneName = typeDroneEnumsItor->second;
		return true;
	}
	else
	{
		return false;
	}
}

std::map<std::string, long long> PayloadEnum2Type::getNames()
{
	return namePayloadEnums;
}

void PayloadEnum2Type::init()
{
	PayloadTypes.push_back(PayloadAndType("Matrice 30 Camera", 52, 0, 0));
	PayloadTypes.push_back(PayloadAndType("Matrice 30T Camera", 53, 0, 1));
	PayloadTypes.push_back(PayloadAndType("Matrice 4E Camera", 88, 0, 0));
	PayloadTypes.push_back(PayloadAndType("Matrice 4T Camera", 89, 0, 1));
	PayloadTypes.push_back(PayloadAndType("Mavic 3E Camera", 66, 0, 0));
	PayloadTypes.push_back(PayloadAndType("Mavic 3T Camera", 67, 0, 0));
	PayloadTypes.push_back(PayloadAndType("Mavic 3TA Camera", 129, 0, 0));
	PayloadTypes.push_back(PayloadAndType("Matrice 3D Camera", 80, 0, 0));
	PayloadTypes.push_back(PayloadAndType("Matrice 3TD Camera", 81, 0, 1));
	PayloadTypes.push_back(PayloadAndType("Matrice 4D Camera", 0, 98, 0));
	PayloadTypes.push_back(PayloadAndType("Matrice 4TD Camera", 0, 99, 1));
}

void PayloadEnum2Type::combine()
{
	for (int i = 0; i < PayloadTypes.size(); i++)
	{
		long long type = BitwiseCombiner::combine(PayloadTypes[i].type, PayloadTypes[i].sub_type, PayloadTypes[i].gimbalindex);
		if (type >= 0)
		{
			typePayloadEnums[type] = PayloadTypes[i].payloadName;
			namePayloadEnums[PayloadTypes[i].payloadName] = type;
		}
	}
}
