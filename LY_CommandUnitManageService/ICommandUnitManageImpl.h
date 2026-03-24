#pragma once
#include <QMutex>
#include <QString>
#include "DataManage/ICommandUnitManageService.h"

namespace qnzkna
{
namespace CCommandUnitManage
{

#define RESULT_CORRECT		0x00
#define ERROR_NULLPOINT		0x01
#define ERROR_NULLPATH		0x02
#define ERROR_LOADXML		0x03
#define ERROR_SAVEXML		0x04
#define ERROR_GETXMLROOT	0x05
#define ERROR_SETCOMMAND	0x06
#define ERROR_DELCOMMAND	0x07
#define ERROR_NOCOMMAND		0x08
#define ERROR_LOADCOMMAND	0x09
#define ERROR_LOADMAPSHOW	0x10

class CCommandUnitServiceImpl : public ICommandUnitManageService
{
public:
    CCommandUnitServiceImpl(void);
    virtual ~CCommandUnitServiceImpl(void);

private:
    CCommandUnitMap		m_mapCommandUnits;
    HeightLayerSetMap	m_mapHeightLayerSet;
    QMutex				m_critialObject;
    QString				m_strCmdUnitDataPath;
    ARMY_NUM_UINT		m_nMaxCommandUnitNum;
public:
    virtual std::string CreateCommandUnitID();
    virtual bool GetAllCommandUnits(CCommandUnitMap *pCommunitMap);
    virtual bool GetVisualAllCommandUnits(CCommandUnitMap *pCommunitMap);
    virtual bool GetAllArmyBaseInfos(CBaseArmyInfoMap *pBaseInfo);
    virtual bool GetTeamCommandUnitsCopy(ETeam eTeam, CCommandUnitMap *pCommunitMap);
    virtual bool GetArmyBaseInfoMap(ETeam eTeam, CBaseArmyInfoMap *pBaseInfo);
    virtual bool GetCommandUnitName(const std::string &strId, bool bFullName, std::string &strName);
    virtual bool GetCommandUnitCurPosition(const std::string &strCmdUnitId, std::string &curPositionId);
    virtual bool SetCommandUnitCurPosition(const std::string &strCommandUnitID, const std::string &strCurPosID);
    virtual bool GetCommandUnit(const std::string &strID, CCommandUnit &data);
    virtual bool GetCommandUnitsByPosID(const std::string &strPosID, CCommandUnitMap &data);
    virtual bool SetCommandUnit(const CCommandUnit &tempUnit);
    virtual bool RemoveCommandUnit(const std::string &strCommandUnitID);
    virtual bool SetCommandUnitLevel(const std::string &strCmdUnitID, unsigned char uchrLevel);
    virtual bool SetArmyLevel(const std::string &nCmUnitID, unsigned char uchrLevel);
    virtual bool SetCommandUnitWorkState(const std::string &nCmdUnitID, unsigned char uchrLevel);
    virtual bool GetArmyTransTime(const std::string &strCmdUnitID, unsigned long &nTransTime);
    virtual bool SetArmyTransTime(const std::string &strCmdUnitID, unsigned long nTransTime);
    virtual bool GetCmdPostTransTime(const std::string &strCmdUnitID, unsigned long &nTransTime);
    virtual bool SetCmdPostTransTime(const std::string &strCmdID, unsigned long nTransTime);
    virtual bool AddCommandUnit(const CCommandUnit &CommandUnit);
    virtual bool ModifyCommandUnit(const CCommandUnit &CommandUnit);
    virtual bool IsExistCommandUnit(const std::string &strCmdUnitID);
    virtual	bool SetCmdVisFlg(const std::string &strCmdID, bool bShow);
    virtual	bool SetCmdLeader(const std::string &strCmdID, const std::string &strLeaderID, ELeaderType eLeaderType);
    virtual void SetAllCmdVisFlg(ETeam eType, bool bVisFlg);
    virtual bool GetETeamByID(const std::string &strCmdID, ETeam &eType);
    virtual bool SetCmdETeam(const std::string &strCmdID, ETeam eDestType);
    virtual bool GetWeaponSystemPairMap(StringMap *pStringMap);
    virtual bool GetCmdUnitIdByWeaponSystemId(const std::string &strWeaponSysID, std::string &);
    virtual bool GetCommandUnitWeaponList(const std::string &strCmdUnitID, StringSet *pWeaponSysList);
    virtual bool GetCommandUnitSubordinateList(const std::string &strCmUnitId, StringSet *pSubordinateList);
    virtual bool GetCommandUnitTopSubGrade(const std::string &strCmdUnitId, EArmyGrade &eArmyGrade);

    virtual bool GetCommandUnitCmdPostFirstLineVisual(const std::string &strCmdUnitId, bool &bShow);
    virtual bool GetCommandUnitArmyFirstLineVisual(const std::string &strCmdUnitId, bool &bShow);
    virtual bool GetCommandUnitCommandEndLineVisual(const std::string &strCmdUnitId, bool &bShow);
    virtual bool SetCommandUnitCmdPostFirstLineVisual(const std::string &strCmdUnitId, bool bShow);
    virtual bool SetCommandUnitArmyFirstLineVisual(const std::string &strCmdUnitId, bool bShow);
    virtual bool SetCommandUnitCommandEndLineVisual(const std::string &strCmdUnitId, bool bShow);
    virtual bool GetCommandUnitCmdPostFirstLine(const std::string &strCmdUnitId, double &bValue);
    virtual bool GetCommandUnitArmyFirstLine(const std::string &strCmdUnitId, double &bValue);
    virtual bool GetCommandUnitCommandEndLine(const std::string &strCmdUnitId);
    virtual bool SetCommandUnitCmdPostFirstLine(const std::string &strCmdUnitId, double bVaulue);
    virtual bool SetCommandUnitArmyFirstLine(const std::string &strCmdUnitId, double bValue);
    virtual bool SetCommandUnitCommandEndLine(const std::string &strCmdUnitId, double bValue);
    virtual bool GetCommandUnitGrade(const std::string &strCmdUnitId, EArmyGrade &eArmyGrade);
    virtual bool SetCommandUnitGrade(const std::string &strCmdUnitId, EArmyGrade eArmyGrade);

    virtual bool SetCommandUnitDirectionId(const std::string &strCmdUnitId, const std::string &strDirectionId);
    virtual bool RemoveCommandUnitDirectionId(const std::string &strCmdUnitId, const std::string &strDirectionId);
    virtual bool SetCommandUnitRegionId(const std::string &strCmdUnitId, const std::string &strDirectionId);
    virtual bool RemoveCommandUnitRegionId(const std::string &strCmdUnitId, const std::string &strDirectionId);
    virtual bool RemoveCommandUnitWeaponSysId(const std::string &strCmdUnitId, const std::string &strWeaponSysId);
    virtual bool RemoveCommandUnitVitalAreaId(const std::string &strCmdUnitId, const std::string &strVitalAreaId);
    virtual bool RemoveCommandUnitNeighborId(const std::string &strCmdUnitId, const std::string &strFriendUnitId);
    virtual bool RemoveCommandUnitPositionId(const std::string &strCmdUnitId, const std::string &strPosId);
    virtual ARMY_NUM_UINT GetMaxCommandUnitNum();
    virtual ARMY_NUM_UINT GetCommandUnitNum();
private:
    CCommandUnit *GetCommandUnit(const std::string &strCmdUnitID);
public:
    int Init();
    void LockMemoryCmdUnit();
    void UnLockMemoryCmdUnit();
    bool LoadData(const QString &strPath = ""); 				
    bool SaveData(const QString &strPath = ""); 				
private:
    std::string		m_strHeightXMLPath;
    int ReadXML(const QString &strXMLName);						
    int SaveXML(const QString &strXMLPath);						

private:
    int ReadHeightLayerData(std::string strPath);
    int ModifyHeightLayerData(std::string strPath);
    unsigned long ConverHexStrToULong(std::string strColorValue);
    std::string ConvertIntToStr(int nInt);
    std::string ConvertULongToStr(unsigned long);

public:
    HeightLayerSetMap *GetHeightLayerSetMap();
    virtual void SetHeightData(CHeightLayerSet);
    virtual bool SetHeightShow(int nHeight, bool nShow);
    virtual bool SetHeightColor(int nHeight, unsigned int nColor);
    virtual bool RemoveHeightData(int nHeight);

private:
    void ClearHeightLayerSetMap();
    void CheckUnitLeader(CLeaderVector *pLeaderVec);
    bool ExistPosition(CCommandUnit, std::string);
    bool IsExistCommandUnit(long long nNodeId);
};

}
}