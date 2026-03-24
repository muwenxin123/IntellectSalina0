#include "ICommandUnitManageImpl.h"
#include <QCString.h>
#include <QFile>
#include <QMessageBox>
#include "QDir"
#include <QDebug>
#include "TinyXML/tinyxml.h"

#ifndef WIN32
    #ifndef ltoa
        #include <private/qlocale_tools_p.h>
        #define ltoa(a,b,c) strcpy(b, qulltoa(a, c, ' ').toLocal8Bit().data())
    #endif
#endif

namespace qnzkna
{
namespace CCommandUnitManage
{

CCommandUnitServiceImpl::CCommandUnitServiceImpl(void)
{
    m_strCmdUnitDataPath = "../data/CmdUnitManageFile.xml";
    m_strHeightXMLPath = "../data/dem/90";
    m_nMaxCommandUnitNum = MAX_ARMY_NUM;

    Init();
}

CCommandUnitServiceImpl::~CCommandUnitServiceImpl(void)
{
}

std::string CCommandUnitServiceImpl::CreateCommandUnitID()
{
    long long nUnitIndex = ARMY_ID_RANGE_BEGIN;
    while (IsExistCommandUnit(nUnitIndex))
    {
        nUnitIndex++;
        if (nUnitIndex > ARMY_ID_RANGE_END)
        {
            nUnitIndex = ARMY_ID_RANGE_BEGIN;
            break;
        }
    }
    return NodeIdToString_Hex(nUnitIndex);
}

bool CCommandUnitServiceImpl::GetAllCommandUnits(CCommandUnitMap *pCommunitMap)
{
    if (!pCommunitMap)
    {
        return false;
    }
    LockMemoryCmdUnit();
    *pCommunitMap = m_mapCommandUnits;
    UnLockMemoryCmdUnit();
    return true;
}

bool CCommandUnitServiceImpl::GetVisualAllCommandUnits(CCommandUnitMap *pCommunitMap)
{
    if (!pCommunitMap)
    {
        return false;
    }
    LockMemoryCmdUnit();
    CCommandUnitMapItr itrMap = m_mapCommandUnits.begin();
    for (; itrMap != m_mapCommandUnits.end(); itrMap++)
    {
        if (true == itrMap->second.GetUnitVisual())
        {
            pCommunitMap->insert(std::make_pair(itrMap->first, itrMap->second));
        }
    }
    UnLockMemoryCmdUnit();
    return true;
}

bool CCommandUnitServiceImpl::GetAllArmyBaseInfos(CBaseArmyInfoMap *pBaseInfoMap)
{
    if (!pBaseInfoMap)
    {
        return false;
    }
    LockMemoryCmdUnit();
    CCommandUnitMapItr ItrMap = m_mapCommandUnits.begin();
    for (; ItrMap != m_mapCommandUnits.end(); ItrMap++)
    {
        CCommandUnit *pCmdUnit = &(ItrMap->second);
        if (pCmdUnit)
        {
            CBaseArmyInfo *pBaseInfo = (CBaseArmyInfo *)pCmdUnit;
            pBaseInfoMap->insert(std::make_pair(pBaseInfo->GetID(), *pBaseInfo));
        }
    }
    UnLockMemoryCmdUnit();
    return true;
}

bool CCommandUnitServiceImpl::GetTeamCommandUnitsCopy(ETeam eTeam, CCommandUnitMap *pCommunitMap)
{
    if (!pCommunitMap)
    {
        return false;
    }

    if (Team_OurArmy == eTeam ||
            Team_NeutralArmy == eTeam ||
            Team_EnemyArmy == eTeam ||
            Team_FriendArmy == eTeam)
    {

        LockMemoryCmdUnit();
        CCommandUnitMap::iterator itrMap = m_mapCommandUnits.begin();
        for (; itrMap != m_mapCommandUnits.end(); itrMap++)
        {
            CCommandUnit *pCommandUnit = &(itrMap->second);
            if (pCommandUnit &&
                    (eTeam == pCommandUnit->GetArmyTeam()))
            {
                pCommunitMap->insert(std::make_pair(pCommandUnit->GetID(), *pCommandUnit));
            }
        }
        UnLockMemoryCmdUnit();
        return true;
    }
    return false;
}

bool CCommandUnitServiceImpl::GetArmyBaseInfoMap(ETeam eTeam, CBaseArmyInfoMap *pBaseInfoMap)
{
    if (!pBaseInfoMap)
    {
        return false;
    }

    if (Team_OurArmy == eTeam ||
            Team_NeutralArmy == eTeam ||
            Team_EnemyArmy == eTeam)
    {

        LockMemoryCmdUnit();
        CCommandUnitMap::iterator itrMap = m_mapCommandUnits.begin();
        for (; itrMap != m_mapCommandUnits.end(); itrMap++)
        {
            CCommandUnit *pCommandUnit = &(itrMap->second);
            CBaseArmyInfo *pBaseInfo = (CBaseArmyInfo *)pCommandUnit;
            if (pBaseInfo &&
                    (eTeam == pBaseInfo->GetArmyTeam()))
            {
                pBaseInfoMap->insert(std::make_pair(pBaseInfo->GetID(), *pBaseInfo));
            }
        }
        UnLockMemoryCmdUnit();
        return true;
    }
    return false;
}

bool CCommandUnitServiceImpl::GetCommandUnit(const std::string &strID, CCommandUnit &data)
{
    if ("" == strID)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnitMapItr ItrMap = m_mapCommandUnits.find(strID);
    if (ItrMap != m_mapCommandUnits.end())
    {
        data = ItrMap->second;
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitsByPosID(const std::string &strPosID, CCommandUnitMap &CmdUnitMap)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnitMap::iterator iterCmd = m_mapCommandUnits.begin();
    for (; iterCmd != m_mapCommandUnits.end(); iterCmd++)
    {
        CCommandUnit cmd = iterCmd->second;
        if (ExistPosition(cmd, strPosID))
        {
            CmdUnitMap.insert(std::make_pair(cmd.GetID(), cmd));
            bRet = true;
            break;
        }
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

CCommandUnit *CCommandUnitServiceImpl::GetCommandUnit(const std::string &strCmdUnitID)
{
    CCommandUnit *pUnit = NULL;
    CCommandUnitMap::iterator itrMap = m_mapCommandUnits.find(strCmdUnitID);
    if (itrMap != m_mapCommandUnits.end())
    {
        pUnit = &(itrMap->second);
    }
    return pUnit;
}

bool CCommandUnitServiceImpl::SetCommandUnit(const CCommandUnit &tempUnit)
{
    bool bRet = false;

    std::string id = tempUnit.GetID();

    if (tempUnit.GetID_LLong() >= ARMY_ID_RANGE_BEGIN &&
            tempUnit.GetID_LLong() <= ARMY_ID_RANGE_END)
    {
        LockMemoryCmdUnit();
        CCommandUnitMapItr ItrMap = m_mapCommandUnits.find(tempUnit.GetID());
        if (ItrMap != m_mapCommandUnits.end())
        {
            ItrMap->second = tempUnit;
        }
        else
        {
            m_mapCommandUnits.insert(std::make_pair(tempUnit.GetID(), tempUnit));
        }
        UnLockMemoryCmdUnit();
        bRet = true;
    }
    return bRet;
}

bool CCommandUnitServiceImpl::RemoveCommandUnit(const std::string &strCmdUnitID)
{
    if ("" == strCmdUnitID)
    {
        return false;
    }
    bool bRet = false;

    LockMemoryCmdUnit();
    CCommandUnitMapItr ItrMap = m_mapCommandUnits.find(strCmdUnitID);
    if (ItrMap != m_mapCommandUnits.end())
    {
        m_mapCommandUnits.erase(ItrMap);
        bRet = true;
    }
    UnLockMemoryCmdUnit();

    return bRet;
}

int CCommandUnitServiceImpl::Init()
{
    if (ReadXML(m_strCmdUnitDataPath))
    {
        return ERROR_LOADCOMMAND;
    }
    if (ReadHeightLayerData(m_strHeightXMLPath))
    {
        return ERROR_LOADMAPSHOW;
    }
    return RESULT_CORRECT;
}

int CCommandUnitServiceImpl::ReadXML(const QString &strFullName )
{
    if ("" == strFullName)
    {
        return ERROR_NULLPATH;
    }
    TiXmlDocument pDoc;
    if (!pDoc.LoadFile(strFullName.toLocal8Bit().data()))
    {
        return ERROR_LOADXML;
    }
    TiXmlElement *pRoot = pDoc.RootElement();
    if (!pRoot)
    {
        return ERROR_GETXMLROOT;
    }
    std::string strMaxNum = pRoot->Attribute("CapacityNum");
    m_nMaxCommandUnitNum = StringToInt(strMaxNum);
    TiXmlElement *pCommandNode = pRoot->FirstChildElement("CmdUnitNode");
    while (pCommandNode)
    {
        CCommandUnit CmdUnitObject;
        std::string strTeamKind = pCommandNode->Attribute("TeamKind");
        unsigned int nTeamKind = StringToInt(strTeamKind);
        CmdUnitObject.SetArmyTeam(TeamPropertyFromNumber(nTeamKind));
        CmdUnitObject.SetID(pCommandNode->Attribute("ID"));
        CmdUnitObject.SetName(pCommandNode->Attribute("Designation"));
        CmdUnitObject.SetSimpleName(pCommandNode->Attribute("ShortenedForm"));
        CmdUnitObject.SetArmyGrade(StringToEArmyGrade(pCommandNode->Attribute("ArmyGrade")));
        CmdUnitObject.SetArmyKind(StringToEArmyKind(pCommandNode->Attribute("ArmyKind")));
        CmdUnitObject.SetCorpsKind(StringToECorpsKind(pCommandNode->Attribute("CorpsKind")));
        bool bShowFlag = false;
        std::string strShowFlags = TilXMLTextToString(pCommandNode->Attribute("Show"));
        if (0 != StringToInt(strShowFlags))
        {
            bShowFlag = true;
        }
        CmdUnitObject.SetUnitVisual(bShowFlag);
        TiXmlElement *pTransTimeNode = pCommandNode->FirstChildElement("ConversionTime");
        if (pTransTimeNode)
        {
            CmdUnitObject.SetArmyTransTime(StringToInt(pTransTimeNode->Attribute("Army")));
            CmdUnitObject.SetCmdPostTransTime(StringToInt(pTransTimeNode->Attribute("Cmd")));
        }
        TiXmlElement *pSuperiorList = pCommandNode->FirstChildElement("LeaderList");
        if (pSuperiorList)
        {
            TiXmlElement *pSuperiorNode = pSuperiorList->FirstChildElement("Leader");
            while (pSuperiorNode)
            {
                CLeader tmpLeader;
                tmpLeader.SetLeaderID(pSuperiorNode->Attribute("ID"));
                tmpLeader.SetLeaderType(StringToELeaderType(pSuperiorNode->Attribute("Type")));
                CmdUnitObject.GetLeaderVec()->push_back(tmpLeader);
                pSuperiorNode = pSuperiorNode->NextSiblingElement("Leader");
            }
            CheckUnitLeader(CmdUnitObject.GetLeaderVec());
        }
        TiXmlElement *pPositionList = pCommandNode->FirstChildElement("PositionList");
        if (pPositionList)
        {
            TiXmlElement *pPositionNode = pPositionList->FirstChildElement("Position");
            while (pPositionNode)
            {
                std::string strName = pPositionNode->Attribute("ID");
                CmdUnitObject.GetPositionsVec()->insert(strName);
                std::string strCurPos = pPositionNode->Attribute("IsCurPos");
                if ("1" == strCurPos)
                {
                    CmdUnitObject.SetCurPosition(strName);
                }
                pPositionNode = pPositionNode->NextSiblingElement("Position");
            }
        }
        TiXmlElement *pWeaponList = pCommandNode->FirstChildElement("WeaponSystemList");
        if (pWeaponList)
        {
            TiXmlElement *pWeaponNode = pWeaponList->FirstChildElement("WeaponSystem");
            while (pWeaponNode)
            {
                std::string strWeaponSysID = pWeaponNode->Attribute("ID");
                CmdUnitObject.GetWeaponSystemVec()->insert(strWeaponSysID);
                pWeaponNode = pWeaponNode->NextSiblingElement("WeaponSystem");
            }
        }
        TiXmlElement *pFriendlyList = pCommandNode->FirstChildElement("FriendList");
        if (pFriendlyList)
        {
            TiXmlElement *pFriendlyNode = pFriendlyList->FirstChildElement("Friend");
            while (pFriendlyNode)
            {
                std::string strName = pFriendlyNode->Attribute("ID");
                CmdUnitObject.GetNeighborsVec()->insert(strName);
                pFriendlyNode = pFriendlyNode->NextSiblingElement("Friend");
            }
        }

        SetCommandUnit(CmdUnitObject);
        pCommandNode = pCommandNode->NextSiblingElement("CmdUnitNode");
    }

    return RESULT_CORRECT;
}

void CCommandUnitServiceImpl::LockMemoryCmdUnit()
{
    m_critialObject.lock();
}

void CCommandUnitServiceImpl::UnLockMemoryCmdUnit()
{
    m_critialObject.unlock();
}

int CCommandUnitServiceImpl::SaveXML(const QString &strXMLPath)
{
    if ("" == strXMLPath)
    {
        return ERROR_NULLPATH;
    }
    return RESULT_CORRECT;
}

bool CCommandUnitServiceImpl::LoadData(const QString &strPath )
{
    if ("" == strPath)
    {

        if(0 == ReadXML(m_strCmdUnitDataPath))
        {
            return true;
        }
    }
    else
    {
        if (0 == ReadXML(strPath))
        {
            return true;
        }
    }
    return  false;
}

bool CCommandUnitServiceImpl::SaveData(const QString &strPath )
{
    return true;

    if ("" == strPath)
    {

        if(0 == SaveXML(m_strCmdUnitDataPath))
        {
            return true;
        }
    }
    else
    {
        if (0 == SaveXML(strPath))
        {
            return true;
        }
    }
    return false;
}

bool CCommandUnitServiceImpl::SetCommandUnitCurPosition(const std::string &strCmdUnitID, const std::string &strCurPosID)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCommandUnit = GetCommandUnit(strCmdUnitID);
    if (pCommandUnit)
    {
        if (strCurPosID.size() <= MAX_POSITION_ID_LENGTH)
        {
            pCommandUnit->SetCurPosition(strCurPosID);
            bRet = true;
        }
    }
    UnLockMemoryCmdUnit();

    return bRet;
}

int CCommandUnitServiceImpl::ReadHeightLayerData(std::string strPath)
{
    return ERROR_NULLPOINT;
}

int CCommandUnitServiceImpl::ModifyHeightLayerData(std::string strPath)
{
    return ERROR_NULLPOINT;
}

void CCommandUnitServiceImpl::SetHeightData(CHeightLayerSet heightLayerSet)
{
    int nHeight = heightLayerSet.nHeight;
    HeightLayerSetMap::iterator itrMap = m_mapHeightLayerSet.find(nHeight);
    if (itrMap != m_mapHeightLayerSet.end())
    {
        itrMap->second = heightLayerSet;
    }
    else
    {
        m_mapHeightLayerSet.insert(std::make_pair(nHeight, heightLayerSet));
    }
}

bool CCommandUnitServiceImpl::SetHeightShow(int nHeight, bool nShow)
{
    bool bRt = false;
    HeightLayerSetMap::iterator itrMap = m_mapHeightLayerSet.find(nHeight);
    if (itrMap != m_mapHeightLayerSet.end())
    {
        itrMap->second.bShowFlag = nShow;
        bRt = true;
    }
    return bRt;
}

bool CCommandUnitServiceImpl::SetHeightColor(int nHeight, unsigned int nColor)
{
    bool bRt = false;
    HeightLayerSetMap::iterator itrMap = m_mapHeightLayerSet.find(nHeight);
    if (itrMap != m_mapHeightLayerSet.end())
    {
        itrMap->second.nColor = nColor;
        bRt = true;
    }
    return bRt;
}

bool CCommandUnitServiceImpl::RemoveHeightData(int nHeight)
{
    bool bRt(false);
    HeightLayerSetMap::iterator itrMap = m_mapHeightLayerSet.find(nHeight);
    if (itrMap != m_mapHeightLayerSet.end())
    {
        m_mapHeightLayerSet.erase(itrMap);
        bRt = true;
    }
    return bRt;
}

unsigned long CCommandUnitServiceImpl::ConverHexStrToULong(std::string strTrans)
{
    unsigned long ret = 0;

    if (strTrans.size() <= 2)
    {
        for (int i = 0; i < strTrans.size(); i++)
        {

            char c = strTrans.at(i);
            int addvalue = 0;
            if ((c >= '0') && (c <= '9'))
            {
                addvalue = c - '0';
                ret = ret * 16 + addvalue;
            }
            else if ((c >= 'a') && (c <= 'f'))
            {
                addvalue = c - 'a' + 10;
                ret = ret * 16 + addvalue;
            }
            else if ((c >= 'A') && (c <= 'F'))
            {
                addvalue = c - 'A' + 10;
                ret = ret * 16 + addvalue;
            }
            else
            {
                return 0;
            }
        }
    }
    else if (strTrans.size() > 2)
    {

        if (('0' == strTrans.at(0)) && (('x' == strTrans.at(1)) || ('X' == strTrans.at(1))))
        {
            for (int j = 2; j < strTrans.size(); j++)
            {

                char c = strTrans.at(j);
                int addvalue = 0;
                if ((c >= '0') && (c <= '9'))
                {
                    addvalue = c - '0';
                }
                else if ((c >= 'A') && (c <= 'F'))
                {
                    addvalue = c - 'A' + 10;
                }
                else if ((c >= 'a') && (c <= 'f'))
                {
                    addvalue = c - 'a' + 10;
                }
                else
                {
                    return 0;
                }

                ret = ret * 16 + addvalue;
            }
        }
        else
        {
            for (int k = 0; k < strTrans.size(); k++)
            {
                char c = strTrans.at(k);
                int addvalue = 0;
                if ((c >= '0') && (c <= '9'))
                {
                    addvalue = c - '0';
                }
                else if ((c >= 'a') && (c <= 'f'))
                {
                    addvalue = c - 'a' + 10;
                }
                else if ((c >= 'A') && (c <= 'F'))
                {
                    addvalue = c - 'A' + 10;
                }
                else
                {
                    return 0;
                }
                ret = ret * 16 + addvalue;
            }
        }
    }

    return ret;
}

std::string CCommandUnitServiceImpl::ConvertIntToStr(int nInt)
{
    char chrTemp[10];
    memset(chrTemp, 0, 10);
    itoa(nInt, chrTemp, 10);
    return chrTemp;
}

std::string CCommandUnitServiceImpl::ConvertULongToStr(unsigned long UnLongValue)
{
    char chrTemp[20];
    memset(chrTemp, 0, 20);
    ltoa(UnLongValue, chrTemp, 16);
    return chrTemp;
}

HeightLayerSetMap *CCommandUnitServiceImpl::GetHeightLayerSetMap()
{
    return &m_mapHeightLayerSet;
}

void CCommandUnitServiceImpl::ClearHeightLayerSetMap()
{
    m_mapHeightLayerSet.clear();
}

bool CCommandUnitServiceImpl::SetCommandUnitLevel(const std::string &strCmdUnitID, unsigned char uchrLevel)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCommandUnit = GetCommandUnit(strCmdUnitID);
    if (pCommandUnit)
    {
        if (1 == uchrLevel)
        {
            pCommandUnit->SetCommandPostLevel(ECmdPostLevel1);
            bRet = true;
        }
        else if (2 == uchrLevel)
        {
            pCommandUnit->SetCommandPostLevel(ECmdPostLevel2);
            bRet = true;
        }
        else if (3 == uchrLevel)
        {
            pCommandUnit->SetCommandPostLevel(ECmdPostLevel3);
            bRet = true;
        }
        else if (4 == uchrLevel)
        {
            pCommandUnit->SetCommandPostLevel(ECmdPostLevel4);
            bRet = true;
        }
    }
    UnLockMemoryCmdUnit();

    return bRet;
}

bool CCommandUnitServiceImpl::SetArmyLevel(const std::string &nCmUnitID, unsigned char uchrLevel)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCommandUnit = GetCommandUnit(nCmUnitID);
    if (pCommandUnit)
    {
        if (1 == uchrLevel)
        {
            pCommandUnit->SetArmyLevel(EArmyLevel1);
            bRet = true;
        }
        else if (2 == uchrLevel)
        {
            pCommandUnit->SetArmyLevel(EArmyLevel2);
            bRet = true;
        }
        else if (3 == uchrLevel)
        {
            pCommandUnit->SetArmyLevel(EArmyLevel3);
            bRet = true;
        }
        else if (4 == uchrLevel)
        {
            pCommandUnit->SetArmyLevel(EArmyLevel4);
            bRet = true;
        }
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCommandUnitWorkState(const std::string &nCmdUnitID, unsigned char uchrState)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCommandUnit = GetCommandUnit(nCmdUnitID);
    if (pCommandUnit)
    {
        pCommandUnit->SetCommandPostState(uchrState);
        bRet = true;
    }
    UnLockMemoryCmdUnit();

    return bRet;
}

bool CCommandUnitServiceImpl::AddCommandUnit(const CCommandUnit &CommandUnit)
{
    if (GetCommandUnitNum() >= GetMaxCommandUnitNum())
    {
        return false;
    }

    bool bRet = false;

    LockMemoryCmdUnit();
    if ("" != CommandUnit.GetID())
    {
        CCommandUnitMap::iterator itrMap = m_mapCommandUnits.find(CommandUnit.GetID());
        if (itrMap == m_mapCommandUnits.end())
        {
            m_mapCommandUnits.insert(make_pair(CommandUnit.GetID(), CommandUnit));
            bRet = true;
        }
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::ModifyCommandUnit(const CCommandUnit &CommandUnit)
{
    bool bRet = false;

    LockMemoryCmdUnit();
    do
    {
        CCommandUnitMap::iterator itrMap = m_mapCommandUnits.find(CommandUnit.GetID());
        if (itrMap == m_mapCommandUnits.end())
        {
            break;
        }
        itrMap->second = CommandUnit;
        bRet = true;

    }
    while (false);

    UnLockMemoryCmdUnit();

    return bRet;
}

bool CCommandUnitServiceImpl::IsExistCommandUnit(const std::string &strCmdUnitID)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnitMap::iterator itrMap = m_mapCommandUnits.find(strCmdUnitID);
    if (itrMap != m_mapCommandUnits.end())
    {
        bRet = true;
    }
    UnLockMemoryCmdUnit();

    return bRet;
}

bool CCommandUnitServiceImpl::IsExistCommandUnit(long long nNodeId)
{
    bool bRet = false;
    std::string strId = NodeIdToString_Hex(nNodeId);
    LockMemoryCmdUnit();
    do
    {
        CCommandUnitMapItr ItrMap = m_mapCommandUnits.find(strId);
        if (ItrMap != m_mapCommandUnits.end())
        {
            bRet = true;
            break;
        }
    }
    while (false);
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCmdVisFlg(const std::string &strCmdID, bool bShow)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnitMap::iterator itrMap = m_mapCommandUnits.find(strCmdID);
    if (itrMap != m_mapCommandUnits.end())
    {
        itrMap->second.SetUnitVisual(bShow);
        bRet = true;
    }

    UnLockMemoryCmdUnit();

    return bRet;
}

bool CCommandUnitServiceImpl::SetCmdLeader(const std::string &strCmdID, const std::string &strLeaderID, ELeaderType eLeaderType)
{
    bool bRet = false;

    LockMemoryCmdUnit();

    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdID);
    CCommandUnit *pLeaderCmdUnit = GetCommandUnit(strLeaderID);
    if (pCmdUnit &&
            pLeaderCmdUnit)
    {
        CLeader leader(strLeaderID, eLeaderType);
        pCmdUnit->SetLeader(leader);
        bRet = true;
    }

    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCmdETeam(const std::string &strCmdID, ETeam eDestType)
{
    if ("" == strCmdID)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnitMapItr ItrMap = m_mapCommandUnits.find(strCmdID);
    if (ItrMap != m_mapCommandUnits.end())
    {
        ItrMap->second.SetArmyTeam(eDestType);
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

void CCommandUnitServiceImpl::SetAllCmdVisFlg(ETeam eType, bool bVisFlg)
{
    LockMemoryCmdUnit();
    CCommandUnitMapItr ItrMap = m_mapCommandUnits.begin();
    for (; ItrMap != m_mapCommandUnits.end(); ItrMap++)
    {
        CCommandUnit *pCmdUnit = &(ItrMap->second);
        if (pCmdUnit &&
                pCmdUnit->GetArmyTeam() == eType)
        {
            pCmdUnit->SetUnitVisual(bVisFlg);
        }
    }
    UnLockMemoryCmdUnit();
}

bool CCommandUnitServiceImpl::GetETeamByID(const std::string &strCmdID, ETeam &eType)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdID);
    if (pCmdUnit)
    {
        eType = pCmdUnit->GetArmyTeam();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitName(const std::string &strId, bool bFullName , std::string &strName)
{
    if ("" == strId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnitMap::iterator itrMap = m_mapCommandUnits.find(strId);
    if (itrMap != m_mapCommandUnits.end())
    {
        CCommandUnit *pCmdUnit = &(itrMap->second);
        if (pCmdUnit)
        {
            if (bFullName)
            {
                strName = pCmdUnit->GetName();
            }
            else
            {
                strName = pCmdUnit->GetSimpleName();
            };
            bRet = true;
        }
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

void CCommandUnitServiceImpl::CheckUnitLeader(CLeaderVector *pLeaderVec)
{
    CLeader *pNormalLeader = NULL;
    CLeader *pFightLeader = NULL;
    CLeaderVector::iterator itrVec = pLeaderVec->begin();
    for (; itrVec != pLeaderVec->end(); itrVec++)
    {
        CLeader *pLeader = &(*itrVec);
        if (pLeader &&
                (ELeaderType_Normal == pLeader->GetLeaderType()))
        {
            pNormalLeader = pLeader;
        }
        else if (pLeader &&
                 (ELeaderType_Fight == pLeader->GetLeaderType()))
        {
            pFightLeader = pLeader;
        }
    }
    if (pNormalLeader && (!pFightLeader))
    {
        CLeader leader = *pNormalLeader;
        leader.SetLeaderType(ELeaderType_Fight);
        pLeaderVec->push_back(leader);
    }
}

bool CCommandUnitServiceImpl::GetCommandUnitCurPosition(const std::string &strCmdUnitId, std::string &curPositionId)
{
    if ("" == strCmdUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnitMapItr itrMap = m_mapCommandUnits.find(strCmdUnitId);
    if (itrMap != m_mapCommandUnits.end())
    {
        curPositionId = itrMap->second.GetCurPosition();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::ExistPosition(CCommandUnit cmd, std::string strPosID)
{
    bool bRet = false;
    StringSet *pPostionVec = cmd.GetPositionsVec();
    if (pPostionVec)
    {
        StringSetItr iterPosition = pPostionVec->begin();
        for (; iterPosition != pPostionVec->end(); iterPosition++)
        {
            if (*iterPosition == strPosID)
            {
                bRet = true;
                break;;
            }
        }
    }
    return bRet;
}

bool CCommandUnitServiceImpl::GetArmyTransTime(const std::string &strCmdUnitID, unsigned long &nTransTime)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitID);
    if (pCmdUnit)
    {
        nTransTime = pCmdUnit->GetArmyTransTime();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetArmyTransTime(const std::string &strCmdUnitID, unsigned long nTransTime)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitID);
    if (pCmdUnit)
    {
        pCmdUnit->SetArmyTransTime(nTransTime);
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCmdPostTransTime(const std::string &strCmdUnitID, unsigned long &nTransTime)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitID);
    if (pCmdUnit)
    {
        nTransTime = pCmdUnit->GetCmdPostTransTime();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCmdPostTransTime(const std::string &strCmdID, unsigned long nTransTime)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdID);
    if (pCmdUnit)
    {
        pCmdUnit->SetCmdPostTransTime(nTransTime);
        bRet = true;
    }
    UnLockMemoryCmdUnit();

    return bRet;
}

bool CCommandUnitServiceImpl::GetWeaponSystemPairMap(StringMap *pStringMap)
{
    if (!pStringMap)
    {
        return false;
    }
    LockMemoryCmdUnit();
    CCommandUnitMapItr itrMap = m_mapCommandUnits.begin();
    for (; itrMap != m_mapCommandUnits.end(); itrMap++)
    {
        CCommandUnit *pCmdUnit = &(itrMap->second);
        if (pCmdUnit)
        {
            StringSetItr strSetItr = pCmdUnit->GetWeaponSystemVec()->begin();
            for (; strSetItr != pCmdUnit->GetWeaponSystemVec()->end(); strSetItr++)
            {
                pStringMap->insert(std::make_pair(*strSetItr, pCmdUnit->GetID()));
            }
        }
    }
    UnLockMemoryCmdUnit();
    return true;
}

bool CCommandUnitServiceImpl::GetCmdUnitIdByWeaponSystemId(const std::string &strWeaponSysID, std::string &strUnitId)
{
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnitMapItr itrMap = m_mapCommandUnits.begin();
    for (; itrMap != m_mapCommandUnits.end(); itrMap++)
    {
        CCommandUnit *pCmdUnit = &(itrMap->second);
        if (pCmdUnit &&
                pCmdUnit->IsExistWeaponSystem(strWeaponSysID))
        {
            strUnitId = itrMap->first;
            bRet = true;
            break;
        }
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitWeaponList(const std::string &strCmdUnitID, StringSet *pWeaponSysList)
{
    bool bRet = false;
    if (pWeaponSysList)
    {
        LockMemoryCmdUnit();
        CCommandUnitMapItr itrMap = m_mapCommandUnits.find(strCmdUnitID);
        if (itrMap != m_mapCommandUnits.end())
        {
            CCommandUnit *pCmdUnit = &(itrMap->second);
            *pWeaponSysList = *(pCmdUnit->GetWeaponSystemVec());
            bRet = true;
        }
        UnLockMemoryCmdUnit();
    }
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitSubordinateList(const std::string &strCmUnitId, StringSet *pSubordinateList)
{
    if (!pSubordinateList)
    {
        return false;
    };
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmUnitId);
    if (pCmdUnit)
    {
        CCommandUnitMapItr itrMap = m_mapCommandUnits.begin();
        for (; itrMap != m_mapCommandUnits.end(); itrMap++)
        {
            if (itrMap->second.IsExistLeader(strCmUnitId))
            {
                pSubordinateList->insert(itrMap->first);
            }
        }
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitTopSubGrade(const std::string &strCmUnitId, EArmyGrade &eArmyGrade)
{
    if ("" == strCmUnitId)
    {
        return false;
    }
    EArmyGrade eGrade = EArmyGrade_Platoon;
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmUnitId);
    if (pCmdUnit)
    {
        CCommandUnitMapItr itrMap = m_mapCommandUnits.begin();
        for (; itrMap != m_mapCommandUnits.end(); itrMap++)
        {
            if (itrMap->second.IsExistLeader(strCmUnitId))
            {
                if (NsArmy::EGradeCompareResult_Small == NsArmy::CompareArmyGrade(eGrade, itrMap->second.GetArmyGrade()))
                {
                    eGrade = itrMap->second.GetArmyGrade();
                }
                bRet = true;
            }
        }
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitGrade(const std::string &strCmUnitId, EArmyGrade &eArmyGrade)
{
    if ("" == strCmUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmUnitId);
    if (pCmdUnit)
    {
        eArmyGrade = pCmdUnit->GetArmyGrade();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCommandUnitGrade(const std::string &strCmUnitId, EArmyGrade eArmyGrade)
{
    if ("" == strCmUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmUnitId);
    if (pCmdUnit)
    {
        pCmdUnit->SetArmyGrade(eArmyGrade);
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCommandUnitDirectionId(const std::string &strCmdUnitId, const std::string &strDirectionId)
{
    if ("" == strCmdUnitId || "" == strDirectionId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        bRet = pCmdUnit->SetDirectionInList(strDirectionId);
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::RemoveCommandUnitDirectionId(const std::string &strCmdUnitId, const std::string &strDirectionId)
{
    if ("" == strCmdUnitId || "" == strDirectionId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        bRet = pCmdUnit->RemoveDirectionInList(strDirectionId);
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCommandUnitRegionId(const std::string &strCmdUnitId, const std::string &strDirectionId)
{
    if ("" == strCmdUnitId || "" == strDirectionId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        bRet = pCmdUnit->SetRegionInList(strDirectionId);
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::RemoveCommandUnitRegionId(const std::string &strCmdUnitId, const std::string &strDirectionId)
{
    if ("" == strCmdUnitId || "" == strDirectionId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        bRet = pCmdUnit->RemoveRegionInList(strDirectionId);
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::RemoveCommandUnitWeaponSysId(const std::string &strCmdUnitId, const std::string &strWeaponSysId)
{
    if ("" == strCmdUnitId ||
            "" == strWeaponSysId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        bRet = pCmdUnit->RemoveWeaponSysInList(strWeaponSysId);
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::RemoveCommandUnitVitalAreaId(const std::string &strCmdUnitId, const std::string &strVitalAreaId)
{
    if ("" == strCmdUnitId ||
            "" == strVitalAreaId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        bRet = pCmdUnit->RemoveVitalAreaInList(strVitalAreaId);
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::RemoveCommandUnitPositionId(const std::string &strCmdUnitId, const std::string &strPosId)
{
    if ("" == strCmdUnitId ||
            "" == strPosId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        bRet = pCmdUnit->RemovePositionInList(strPosId);
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::RemoveCommandUnitNeighborId(const std::string &strCmdUnitId, const std::string &strFriendUnitId)
{
    if ("" == strCmdUnitId ||
            "" == strFriendUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        bRet = pCmdUnit->RemoveNeighborInList(strFriendUnitId);
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitCmdPostFirstLineVisual(const std::string &strCmUnitId, bool &bShow)
{
    if ("" == strCmUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmUnitId);
    if (pCmdUnit)
    {
        bShow = pCmdUnit->GetCmdPostFirstLineVisual();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitArmyFirstLineVisual(const std::string &strCmUnitId, bool &bShow)
{
    if ("" == strCmUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmUnitId);
    if (pCmdUnit)
    {
        bShow = pCmdUnit->GetArmyFirstLineVisual();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitCommandEndLineVisual(const std::string &strCmdUnitId, bool &bShow)
{
    if ("" == strCmdUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        bShow = pCmdUnit->GetCommandEndLineVisual();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCommandUnitCmdPostFirstLineVisual(const std::string &strCmdUnitId, bool bShow)
{
    if ("" == strCmdUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        pCmdUnit->SetCmdPostFirstLineVisual(bShow);
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCommandUnitArmyFirstLineVisual(const std::string &strCmdUnitId, bool bShow)
{
    if ("" == strCmdUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        pCmdUnit->SetArmyFirstLineVisual(bShow);
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCommandUnitCommandEndLineVisual(const std::string &strCmdUnitId, bool bShow)
{
    if ("" == strCmdUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        pCmdUnit->SetCommandEndLineVisual(bShow);
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitCmdPostFirstLine(const std::string &strCmUnitId, double &bValue)
{
    if ("" == strCmUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmUnitId);
    if (pCmdUnit)
    {
        bValue = pCmdUnit->GetCmdPostFirstLine();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitArmyFirstLine(const std::string &strCmUnitId, double &bValue)
{
    if ("" == strCmUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmUnitId);
    if (pCmdUnit)
    {
        bValue = pCmdUnit->GetArmyFirstLine();
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::GetCommandUnitCommandEndLine(const std::string &strCmdUnitId)
{

    return true;
}

bool CCommandUnitServiceImpl::SetCommandUnitCmdPostFirstLine(const std::string &strCmdUnitId, double bVaulue)
{
    if ("" == strCmdUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        pCmdUnit->SetCmdPostFirstLine(bVaulue);
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCommandUnitArmyFirstLine(const std::string &strCmdUnitId, double bValue)
{
    if ("" == strCmdUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        pCmdUnit->SetArmyFirstLine(bValue);
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

bool CCommandUnitServiceImpl::SetCommandUnitCommandEndLine(const std::string &strCmdUnitId, double bValue)
{
    if ("" == strCmdUnitId)
    {
        return false;
    }
    bool bRet = false;
    LockMemoryCmdUnit();
    CCommandUnit *pCmdUnit = GetCommandUnit(strCmdUnitId);
    if (pCmdUnit)
    {
        pCmdUnit->SetCommandEndLine(bValue);
        bRet = true;
    }
    UnLockMemoryCmdUnit();
    return bRet;
}

ARMY_NUM_UINT CCommandUnitServiceImpl::GetMaxCommandUnitNum()
{
    LockMemoryCmdUnit();
    ARMY_NUM_UINT nNum = m_nMaxCommandUnitNum;
    UnLockMemoryCmdUnit();
    return nNum;
}

ARMY_NUM_UINT CCommandUnitServiceImpl::GetCommandUnitNum()
{
    LockMemoryCmdUnit();
    ARMY_NUM_UINT nNum = m_mapCommandUnits.size();
    UnLockMemoryCmdUnit();
    return nNum;
}

}
}
