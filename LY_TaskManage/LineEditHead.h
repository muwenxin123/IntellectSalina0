#ifndef LINEEDITHEAD_H
#define LINEEDITHEAD_H

#include <list>

typedef struct ChildItem
{
    int    m_ItemID;
    double m_SaleLeftWidth;
    double m_SaleRightWidth;
    double m_SaleTopHeight;
    double m_SaleBottomHeight;
} ChildItem;

class MissionCmdVariant
{
public:
    MissionCmdVariant()                 : _strPara(""),   _dPara(0.0f), _bPara(false), _valueType(-1) {}
    MissionCmdVariant(std::string para) : _strPara(para), _dPara(0.0f), _bPara(false), _valueType(1) {}
    MissionCmdVariant(double para)      : _strPara(""),   _dPara(para), _bPara(false), _valueType(2) {}
    MissionCmdVariant(bool para)        : _strPara(""),   _dPara(para), _bPara(para),  _valueType(3) {}
    MissionCmdVariant(const MissionCmdVariant &cmdVariant) : _strPara(cmdVariant.toString()), _dPara(cmdVariant.toDouble()), _bPara(cmdVariant.toBool()), _valueType(cmdVariant.valueType()) {}
    MissionCmdVariant &operator=(const MissionCmdVariant &cmdVariant)
    {
        _strPara   = (cmdVariant.toString());
        _dPara     = (cmdVariant.toDouble());
        _bPara     = (cmdVariant.toBool());
        _valueType = (cmdVariant.valueType());
        return *this;
    }
    std::string toString() const
    {
        return _strPara;
    }
    double      toDouble() const
    {
        return _dPara;
    }
    bool        toBool()   const
    {
        return _bPara;
    }
    int         valueType()const
    {
        return _valueType;
    }

private:
    std::string _strPara;
    double      _dPara;
    bool        _bPara;
    int         _valueType;
};

class MissionCmdParamInfo
{

public:
    MissionCmdParamInfo();

    MissionCmdParamInfo(const MissionCmdParamInfo &other);
    const MissionCmdParamInfo &operator=(const MissionCmdParamInfo &other);

    int                    decimalPlaces(void) const
    {
        return _decimalPlaces;
    }
    double                 defaultValue(void) const
    {
        return _defaultValue;
    }
    std::list<std::string> enumStrings(void) const
    {
        return _enumStrings;
    }
    std::list<MissionCmdVariant>     enumValues(void) const
    {
        return _enumValues;
    }
    std::string            label(void) const
    {
        return _label;
    }
    int                    param(void) const
    {
        return _param;
    }
    std::string            units(void) const
    {
        return _units;
    }
    bool                   nanUnchanged(void) const
    {
        return _nanUnchanged;
    }

private:
    int                    _decimalPlaces;
    double                 _defaultValue;
    std::list<std::string> _enumStrings;
    std::list<MissionCmdVariant>     _enumValues;
    std::string            _label;
    int                    _param;
    std::string            _units;
    bool                   _nanUnchanged;

    friend class MissionCommandUIInfo;
};

#endif
