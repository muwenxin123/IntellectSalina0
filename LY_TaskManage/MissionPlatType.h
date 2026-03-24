#ifndef MISSIONPLATTYPE_H
#define MISSIONPLATTYPE_H

#include <string>
#include <vector>

using namespace std;

class missionPlatType
{
public:
    void setIndex(unsigned short _index)
    {
        index = _index;
    }
    unsigned short getIndex()
    {
        return index;
    }

    void setMissionID(string _MissionID)
    {
        MissionID = _MissionID;
    }
    string getMissionID()
    {
        return MissionID;
    }

    void setMissionName(string _MissionName)
    {
        MissionName = _MissionName;
    }
    string getMissionName()
    {
        return MissionName;
    }

    void setWeaponType(int _weaponType)
    {
        WeaponType = _weaponType;
    }
    int  getWeaponType()
    {
        return WeaponType;
    }

    void setPlatTypeName(string _PlatTypeName)
    {
        PlatTypeName = _PlatTypeName;
    }
    string getPlatTypeName()
    {
        return PlatTypeName;
    }

    void setPlatTypeID(string _PlatTypeID)
    {
        PlatTypeID = _PlatTypeID;
    }
    string getPlatTypeID()
    {
        return PlatTypeID;
    }

    void setPlatNum(unsigned short _PlatNum)
    {
        PlatNum = _PlatNum;
    }
    unsigned short getPlatNum()
    {
        return PlatNum;
    }

    void setPlatNames_Selected(vector<string> _PlatNames)
    {
        PlatNames_selected = _PlatNames;
    }
    vector<string> getPlatNames_Selected()
    {
        return PlatNames_selected;
    }

    void setPlatNames(vector<string> _PlatNames)
    {
        PlatNames = _PlatNames;
    }
    vector<string> getPlatNames()
    {
        return PlatNames;
    }

    void setControlers(vector<string> _controlers)
    {
        Controlers = _controlers;
    }
    vector<string> getControlers()
    {
        return Controlers;
    }

    void setAllControlers(vector<string> _controlers)
    {
        AllControlers = _controlers;
    }
    vector<string> getAllControlers()
    {
        return AllControlers;
    }

private:
    unsigned short index;
    string MissionID;
    string MissionName;
    int	   WeaponType;
    string PlatTypeName;
    string PlatTypeID;
    unsigned short PlatNum;
    vector<string> PlatNames_selected;
    vector<string> PlatNames;
    vector<string> Controlers;
    vector<string> AllControlers;
};

#endif