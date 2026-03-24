#pragma once
#include <map>
#include <string>
#include <vector>


class BitwiseCombiner {
	
public:
	//将三个int合并为一个long
	static long long combine(int a, int b, int c);
	//从long long 中还原三个int
	static void extract(long long combined, int& a, int& b, int&c);

	//分别获取三个int
	static int getFirst(long long combined);
	static int getSecond(long long combined);
	static int getThird(long long combined);

private:
	static const int BITS = 10;
	static const int MASK = (1 << BITS) - 1;
};

struct DroneAndType {
	std::string DroneName;
	int domain;
	int type;
	int sub_type;
	DroneAndType(std::string _DroneName, int _domai, int _type, int _sub_type)
	{
		DroneName = _DroneName;
		domain = _domai;
		type = _type;
		sub_type = _sub_type;
	}
};

struct PayloadAndType {
	std::string payloadName;
	int type;
	int sub_type;
	int gimbalindex;
	PayloadAndType(std::string _payloadName, int _type, int _sub_type, int _gimbalindex)
	{
		payloadName = _payloadName;
		type = _type;
		sub_type = _sub_type;
		gimbalindex = _gimbalindex;
	}
};

class DroneEnum2Type {
public:
	DroneEnum2Type();
	
	bool getTypeIDFromDroneName(std::string droneName, int& domain, int& type, int& sub_type);
	bool getDroneNameFromTypeID(int domain, int type, int sub_type, std::string& droneName);
	std::map<std::string, long long> getNames();
private:
	void init();
	void combine();
private:
	std::vector<DroneAndType> droneTypes;
	std::map<long long, std::string> typeDroneEnums;
	std::map<std::string, long long> nameDroneEnums;
};

class PayloadEnum2Type {
public:
	PayloadEnum2Type();

	bool getTypeIDFromPayloadName(std::string droneName, int& domain, int& type, int& sub_type);
	bool getPayloadNameFromTypeID(int domain, int type, int sub_type, std::string& droneName);
	std::map<std::string, long long> getNames();

private:
	void init();
	void combine();
private:
	std::vector<PayloadAndType> PayloadTypes;
	std::map<long long, std::string> typePayloadEnums;
	std::map<std::string, long long> namePayloadEnums;
};