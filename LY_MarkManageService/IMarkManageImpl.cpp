#include "IMarkManageImpl.h"
#include "QCString.h"
#include "QSqlQuery"
#include "QString"
#include "QVariant"
#include "DataManage/Mark/Mark.h"
#include "LY_MarkManageService.h"
#include "TinyXML/tinyxml.h"
#include "LY_MarkManageServiceInterface.h"
#include <QDateTime>
#include <QSettings>

using namespace qnzkna::MarkManage;

qnzkna::MarkManage::IMarkManageImpl::IMarkManageImpl()
{
    m_MarkListVec.clear();
    loadLoginName();
    InitDataBase();
    LoadDBData();
    LoadPosDataBase();
    LoadMissionTypeDBData();
    LoadZoneDBData();

    m_nIconID = 0;
}

qnzkna::MarkManage::IMarkManageImpl::~IMarkManageImpl()
{
    db.close();
}

CSystemMarkInfoVec qnzkna::MarkManage::IMarkManageImpl::GetMarkList() const
{
    QMutexLocker locker(&sMutex);
    return m_MarkListVec;
}

CSystemMarkInfoVec qnzkna::MarkManage::IMarkManageImpl::GetMarkPosList() const
{
    return m_MarkPosListVec;
}

CSystemMarkInfoVec qnzkna::MarkManage::IMarkManageImpl::GetMarkPosList(const std::string &strID) const
{
    CSystemMarkInfoVec InfoVec;
    for (auto data : m_MarkPosListVec)
    {
        if (0 == strcmp(strID.c_str(), data.GetID().c_str()))
        {
            InfoVec.push_back(data);
        }
    }
    return InfoVec;
}

void qnzkna::MarkManage::IMarkManageImpl::InitDataBase()
{
    QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
    ini.beginGroup("Database");
    QString strDbName = ini.value("DbName", "LY").toString();
    QString strDbIP = ini.value("SvrIP", "192.168.1.115").toString();
    QString strDbUser = ini.value("User", "root").toString();
    QString strDbPass = ini.value("Password", "123456").toString();
    QString strDbDriver = ini.value("Driver", "QMYSQL").toString();

    db = QSqlDatabase::addDatabase(strDbDriver);
    db.setHostName(strDbIP);
    db.setPort(3306);
    db.setDatabaseName(strDbName);
    db.setUserName(strDbUser);
    db.setPassword(strDbPass);
    bool bresult = db.open();
}

void qnzkna::MarkManage::IMarkManageImpl::LoadPosDataBase()
{
    QSqlQuery query;

    QString strSql = "SELECT `SJ`, `MBBH`, `JD`, `WD`, `GD`";
    strSql += " FROM `ZZMB_XTMBWZ`";
    bool success = query.exec(strSql);
    if (success)
    {
        m_MarkPosListVec.clear();

        while (query.next())
        {
            QString str1 = query.value(0).toString();
            QString str2 = query.value(1).toString();
            QString str3 = query.value(2).toString();
            QString str4 = query.value(3).toString();
            QString str5 = query.value(4).toString();

            SystemMarkInfo stInfo;
            stInfo.SetFindTime(str1.toStdString());
            stInfo.SetID(str2.toStdString());
            stInfo.SetLon(str3.toDouble());
            stInfo.SetLat(str4.toDouble());
            stInfo.SetHeight(str5.toDouble());

            m_MarkPosListVec.push_back(stInfo);
        }
    }
}

void qnzkna::MarkManage::IMarkManageImpl::LoadDBData()
{
    QMutexLocker locker(&sMutex);

    QSqlQuery query;
    QString strSql = "SELECT `MBBH`, `MBMC`, `MBLX`, `MBSX`, `MBCZ`, `MBJD`, `MBWD`, `MBGD`, `MBSD`, `MBHX`, `MBMS`";
    strSql += ", `VXDJ` , `FXSJ`, `FXZB`, `CCCD`, `CCKD`, `CCGD`, `ZCHSYQ` ";
    strSql += ", `SMDJSJ` , `YHBWDX`, `YHBWFX`, `MBMJ`, `SHD`, `FKZB`, `HSZB` ,`MBZT`";
    strSql += " FROM `ZZMB_XTMBXX`";
    bool success = query.exec(strSql);
    if (success)
    {
        m_MarkListVec.clear();

        while (query.next())
        {
            QString str1 = query.value(0).toString();
            QString str2 = query.value(1).toString();
            QString str3 = query.value(2).toString();
            QString str4 = query.value(3).toString();
            QString str5 = query.value(4).toString();
            QString str6 = query.value(5).toString();
            QString str7 = query.value(6).toString();
            QString str8 = query.value(7).toString();
            QString str9 = query.value(8).toString();
            QString str10 = query.value(9).toString();
            QString str11 = query.value(10).toString();
            QString str12 = query.value(11).toString();
            QString str13 = query.value(12).toString();
            QString str14 = query.value(13).toString();

            QString str17 = query.value(14).toString();
            QString str18 = query.value(15).toString();
            QString str19 = query.value(16).toString();
            QString str20 = query.value(17).toString();
            QString str21 = query.value(18).toString();
            QString str22 = query.value(19).toString();
            QString str23 = query.value(20).toString();
            QString str24 = query.value(21).toString();
            QString str25 = query.value(22).toString();
            QString str26 = query.value(23).toString();
            QString str27 = query.value(24).toString();
            QString str28 = query.value(25).toString();

            SystemMarkInfo stInfo;
            stInfo.SetID(str1.toStdString());
            stInfo.SetName(str2.toStdString());
            stInfo.SetType(stInfo.GetTypeENum(str3.toInt()));
            stInfo.SetAttribute(stInfo.GetAttributeENum(str4.toInt()));
            stInfo.SetMaterial(stInfo.GetMaterialENum(str5.toInt()));
            stInfo.SetLon(str6.toDouble());
            stInfo.SetLat(str7.toDouble());
            stInfo.SetHeight(str8.toDouble());
            stInfo.SetSpeed(str9.toDouble());
            stInfo.SetPitch(str10.toDouble());
            stInfo.SetDirections(str11.toStdString());
            stInfo.SetThreatenDegree(stInfo.GetThreatenDegreeENum(str12.toInt()));
            stInfo.SetFindTime(str13.toStdString());
            stInfo.SetFindEqu(str14.toStdString());

            stInfo.SetSize(str17.toDouble(), str18.toDouble(), str19.toDouble());
            stInfo.SetDetectRequest(str20.toStdString());

            stInfo.SeAttackWindow(str21.toDouble());
            stInfo.SetVitalSize(str22.toDouble());
            stInfo.SetVitalDirection(str23.toDouble());
            stInfo.SetMarkArea(str24.toDouble());
            stInfo.SetMarkHurt(str25.toDouble());
            stInfo.SetDefenceNorm(str26.toDouble());
            stInfo.SetHurtNorm(str27.toDouble());
            stInfo.SetState(stInfo.GetStateENum(str28.toInt()));

            m_MarkListVec.push_back(stInfo);
        }
    }
}

void qnzkna::MarkManage::IMarkManageImpl::InitMapIcon()
{
    QMutexLocker locker(&sMutex);

    if (IMarkManageServiceActivator::Get().getPTaskManageService() == nullptr)
    {
        return;
    }
    int nActionID = IMarkManageServiceActivator::Get().getPTaskManageService()->GetActivatingActionID();

    CRelationActionMarkVec m_RelationActionMarkVec = IMarkManageServiceActivator::Get().getPTaskManageService()->GetRelationMark(nActionID);

    for (auto &data : m_MarkListVec)
    {
        if (!JudeActionMark(m_RelationActionMarkVec, data.GetID()))
        {
            continue;
        }
        int nID = GenerateIconID();
        data.SetIconID(nID);
        double dLon = data.GetLon();
        double dLat = data.GetLat();
        VecStr vs;
        vs.push_back(nID);
        vs.push_back(data.GetName().c_str());
        vs.push_back(0);
        ESystemMarkType eType = data.GetType();
        ESystemMarkAttribute eAttribute = data.GetAttribute();

        data.SetMapIconID(LY_MarkManageService::Get().AddIcon(dLon, dLat, eType, eAttribute, vs));
        LY_MarkManageService::Get().Add3DIcon(data);

    }
}

qnzkna::MarkManage::IMarkManageImpl &qnzkna::MarkManage::IMarkManageImpl::Get()
{
    static IMarkManageImpl sLY_MarkManageService;
    return sLY_MarkManageService;
}

bool qnzkna::MarkManage::IMarkManageImpl::EditDBData(SystemMarkInfo &data)
{
    std::string strID = data.GetID();

    if (!IsIDExisted(strID))
    {
        return false;
    }

    QString strSql("UPDATE ZZMB_XTMBXX set ");
    strSql += "`MBMC` = '" + QString::fromStdString(data.GetName()) + "',";
    strSql += "`MBLX` = " + QString::number(data.GetTypeENum(data.GetType())) + ",";
    strSql += "`MBSX` = " + QString::number(data.GetAttributeENum(data.GetAttribute())) + ",";
    strSql += "`MBCZ` = " + QString::number(data.GetMaterialENum(data.GetMaterial())) + ",";
    strSql += "`MBJD` = " + QString::number(data.GetLon(), 'f', 7) + ",";
    strSql += "`MBWD` = " + QString::number(data.GetLat(), 'f', 7) + ",";
    strSql += "`MBGD` = " + QString::number(data.GetHeight(), 'f', 2) + ",";
    strSql += "`MBSD` = " + QString::number(data.GetSpeed(), 'f', 2) + ",";
    strSql += "`MBHX` = " + QString::number(data.GetPitch(), 'f', 2) + ",";
    strSql += "`MBMS` = '" + QString::fromStdString(data.GetDirections()) + "',";
    strSql += "`VXDJ` = " + QString::number(data.GetThreatenDegreeENum(data.GetThreatenDegree())) + "," ;
    strSql += "`FXSJ` = '" + QString::fromStdString(data.GetFindTime()) + "',";
    strSql += "`FXZB` = '" + QString::fromStdString(data.GetFindEqu()) + "',";

    strSql += "`CCCD` = " + QString::number(data.GetSizeX(), 'f', 6) + ",";
    strSql += "`CCKD` = " + QString::number(data.GetSizeY(), 'f', 6) + ",";
    strSql += "`CCGD` = " + QString::number(data.GetSizeZ(), 'f', 6) + ",";

    strSql += "`SMDJSJ` = " +	QString::number(data.GetAttackWindow(), 'f', 2) + ",";
    strSql += "`YHBWDX` = " +	QString::number(data.GetVitalSize(), 'f', 2) + ",";
    strSql += "`YHBWFX` = " +	QString::number(data.GetVitalDirection(), 'f', 2) + ",";
    strSql += "`MBMJ` = " +		QString::number(data.GetMarkArea(), 'f', 2) + ",";
    strSql += "`SHD` = " +		QString::number(data.GetMarkHurt(), 'f', 2) + ",";
    strSql += "`FKZB` = " +		QString::number(data.GetDefenceNorm(), 'f', 2) + ",";
    strSql += "`HSZB` = " +		QString::number(data.GetHurtNorm(), 'f', 2) + ",";
    strSql += "`MBZT` = " + QString::number(data.GetStateENum(data.GetStats())) + ",";

    strSql += "`ZCHSYQ` = '" + QString::fromStdString(data.GetDetectRequest()) + "' ";
    strSql += "where `MBBH` = '" + QString::fromStdString(data.GetID()) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success)
    {
        sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Mark, ESystemOperatorType::ESystemOperatorType_Add, strID.data());
    }
    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::EditDBData(SystemMarkInfo &data, std::string strOldID)
{

    if (!IsIDExisted(strOldID))
    {
        return false;
    }

    QString strSql("UPDATE ZZMB_XTMBXX set ");
    strSql += "`MBBH` = '" + QString::fromStdString(data.GetID()) + "',";
    strSql += "`MBMC` = '" + QString::fromStdString(data.GetName()) + "',";
    strSql += "`MBLX` = " + QString::number(data.GetTypeENum(data.GetType())) + ",";
    strSql += "`MBSX` = " + QString::number(data.GetAttributeENum(data.GetAttribute())) + ",";
    strSql += "`MBCZ` = " + QString::number(data.GetMaterialENum(data.GetMaterial())) + ",";
    strSql += "`MBJD` = " + QString::number(data.GetLon(), 'f', 7) + ",";
    strSql += "`MBWD` = " + QString::number(data.GetLat(), 'f', 7) + ",";
    strSql += "`MBGD` = " + QString::number(data.GetHeight(), 'f', 2) + ",";
    strSql += "`MBSD` = " + QString::number(data.GetSpeed(), 'f', 2) + ",";
    strSql += "`MBHX` = " + QString::number(data.GetPitch(), 'f', 2) + ",";
    strSql += "`MBMS` = '" + QString::fromStdString(data.GetDirections()) + "',";
    strSql += "`VXDJ` = " + QString::number(data.GetThreatenDegreeENum(data.GetThreatenDegree())) + ",";
    strSql += "`FXSJ` = '" + QString::fromStdString(data.GetFindTime()) + "',";
    strSql += "`FXZB` = '" + QString::fromStdString(data.GetFindEqu()) + "',";

    strSql += "`CCCD` = " + QString::number(data.GetSizeX(), 'f', 6) + ",";
    strSql += "`CCKD` = " + QString::number(data.GetSizeY(), 'f', 6) + ",";
    strSql += "`CCGD` = " + QString::number(data.GetSizeZ(), 'f', 6) + ",";

    strSql += "`SMDJSJ` = " + QString::number(data.GetAttackWindow(), 'f', 2) + ",";
    strSql += "`YHBWDX` = " + QString::number(data.GetVitalSize(), 'f', 2) + ",";
    strSql += "`YHBWFX` = " + QString::number(data.GetVitalDirection(), 'f', 2) + ",";
    strSql += "`MBMJ` = " + QString::number(data.GetMarkArea(), 'f', 2) + ",";
    strSql += "`SHD` = " + QString::number(data.GetMarkHurt(), 'f', 2) + ",";
    strSql += "`FKZB` = " + QString::number(data.GetDefenceNorm(), 'f', 2) + ",";
    strSql += "`HSZB` = " + QString::number(data.GetHurtNorm(), 'f', 2) + ",";
    strSql += "`MBZT` = " + QString::number(data.GetStateENum(data.GetStats())) + ",";

    strSql += "`ZCHSYQ` = '" + QString::fromStdString(data.GetDetectRequest()) + "' ";
    strSql += "where `MBBH` = '" + QString::fromStdString(strOldID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);

    if (success)
    {
        sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Mark, ESystemOperatorType::ESystemOperatorType_Add, strOldID.data());
    }

    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::AddDBData(SystemMarkInfo &data)
{
    std::string strID = data.GetID();

    if (IsIDExisted(strID))
    {
        return false;
    }

    QString strSql("INSERT INTO ZZMB_XTMBXX (`MBBH`, `MBMC`, `MBLX`, `MBSX`, `MBCZ`, `MBJD`, `MBWD`, `MBGD`, `MBSD`, `MBHX`, `MBMS`,`VXDJ`,`FXSJ`,`FXZB`, `CCCD`, `CCKD`, `CCGD`, `ZCHSYQ`, `SMDJSJ` , `YHBWDX`, `YHBWFX`, `MBMJ`, `SHD`, `FKZB`, `HSZB`, `MBZT`) VALUES (");
    strSql += "'" + QString::fromStdString(data.GetID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetName()) + "',";
    strSql += "'" + QString::number(data.GetTypeENum(data.GetType())) + "',";
    strSql += "'" + QString::number(data.GetAttributeENum(data.GetAttribute())) + "',";
    strSql += "'" + QString::number(data.GetMaterialENum(data.GetMaterial())) + "',";
    strSql += "'" + QString::number(data.GetLon(), 'f', 7) + "',";
    strSql += "'" + QString::number(data.GetLat(), 'f', 7) + "',";
    strSql += "'" + QString::number(data.GetHeight(), 'f', 2) + "',";
    strSql += "'" + QString::number(data.GetSpeed(), 'f', 2) + "',";
    strSql += "'" + QString::number(data.GetPitch(), 'f', 2) + "',";
    strSql += "'" + QString::fromStdString(data.GetDirections()) + "',";
    strSql += "'" + QString::number(data.GetThreatenDegreeENum(data.GetThreatenDegree())) + "',";
    strSql += "'" + QString::fromStdString(data.GetFindTime()) + "',";
    strSql += "'" + QString::fromStdString(data.GetFindEqu()) + "',";

    strSql += "'" + QString::number(data.GetSizeX(), 'f', 6) + "',";
    strSql += "'" + QString::number(data.GetSizeY(), 'f', 6) + "',";
    strSql += "'" + QString::number(data.GetSizeZ(), 'f', 6) + "',";
    strSql += "'" + QString::fromStdString(data.GetDetectRequest()) + "',";

    strSql += "'" + QString::number(data.GetAttackWindow(), 'f', 2) + "',";
    strSql += "'" + QString::number(data.GetVitalSize(), 'f', 2) + "',";
    strSql += "'" + QString::number(data.GetVitalDirection(), 'f', 2) + "',";
    strSql += "'" + QString::number(data.GetMarkArea(), 'f', 2) + "',";
    strSql += "'" + QString::number(data.GetMarkHurt(), 'f', 2) + "',";
    strSql += "'" + QString::number(data.GetDefenceNorm(), 'f', 2) + "',";
    strSql += "'" + QString::number(data.GetHurtNorm(), 'f', 2) + "'";
    strSql += "'" + QString::number(data.GetStateENum(data.GetStats())) + ",";

    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);

    if (success)
    {
        sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Mark, ESystemOperatorType::ESystemOperatorType_Add, strID.data());
    }

    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::WQPTLBisMBID1Existed(std::string strID)
{
    QString strSql("SELECT  * FROM `RWSJ_WQPTLB` WHERE `MBID1` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success == false)
    {
        return false;
    }
    if (query.size() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool qnzkna::MarkManage::IMarkManageImpl::WQPTLBisMBID2Existed(std::string strID)
{
    QString strSql("SELECT  * FROM `RWSJ_WQPTLB` WHERE `MBID2` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success == false)
    {
        return false;
    }
    if (query.size() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool qnzkna::MarkManage::IMarkManageImpl::WQPTLBsetNullForMarkID(std::string MarkId)
{

    if (!WQPTLBisMBID1Existed(MarkId) && !WQPTLBisMBID2Existed(MarkId))
    {
        return false;
    }

    QString strSql1("UPDATE RWSJ_WQPTLB set ");
    strSql1 += "`MBID1` = NULL";
    strSql1 += "`MBMC1` = NULL";
    strSql1 += "`MBHS1` = NULL";
    strSql1 += "where `MBID1` = '" + QString::fromStdString(MarkId) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql1);

    QString strSql2("UPDATE RWSJ_WQPTLB set ");
    strSql1 += "`MBID2` = NULL";
    strSql1 += "`MBMC2` = NULL";
    strSql1 += "`MBHS2` = NULL";
    strSql1 += "where `MBID2` = '" + QString::fromStdString(MarkId) + "'";
    success = query.exec(strSql1);

    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::RWLBisMB1IDExisted(std::string strID)
{
    QString strSql("SELECT  * FROM `RWSJ_RWLB` WHERE `MB1ID` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success == false)
    {
        return false;
    }
    if (query.size() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool qnzkna::MarkManage::IMarkManageImpl::RWLBisMB2IDExisted(std::string strID)
{
    QString strSql("SELECT  * FROM `RWSJ_RWLB` WHERE `MB2ID` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success == false)
    {
        return false;
    }
    if (query.size() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool qnzkna::MarkManage::IMarkManageImpl::RWLBsetNullForMarkID(std::string MarkId)
{

    if (!RWLBisMB1IDExisted(MarkId) && !RWLBisMB2IDExisted(MarkId))
    {
        return false;
    }

    QString strSql1("UPDATE RWSJ_RWLB set ");
    strSql1 += "`MB1ID` = NULL";
    strSql1 += "`MB1HS` = NULL";
    strSql1 += "where `MB1ID` = '" + QString::fromStdString(MarkId) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql1);

    QString strSql2("UPDATE RWSJ_RWLB set ");
    strSql1 += "`MB2ID` = NULL";
    strSql1 += "`MB2HS` = NULL";
    strSql1 += "where `MB2ID` = '" + QString::fromStdString(MarkId) + "'";
    success = query.exec(strSql1);

    return success;
}

void qnzkna::MarkManage::IMarkManageImpl::loadLoginName(const std::string &strLoginFile)
{
    loginName.clear();
    TiXmlDocument docElement;
    if (!docElement.LoadFile(strLoginFile.c_str()))
    {
        return;
    }

    TiXmlElement *pRootEle = docElement.RootElement();
    if (!pRootEle)
    {
        return;
    }
    TiXmlElement *pRecordListEle = pRootEle->FirstChildElement("µÇÂ¼¼ÇÂ¼ÁÐ±í");
    if (!pRecordListEle)
    {
        return;
    }

    TiXmlElement *pRecordEle = pRecordListEle->FirstChildElement("µÇÂ¼¼ÇÂ¼");
    while (pRecordEle)
    {
        const char *pStrUserName = pRecordEle->Attribute("µÇÂ¼Ãû³Æ");
        loginName = pStrUserName;
        pRecordEle = pRecordEle->NextSiblingElement("µÇÂ¼¼ÇÂ¼");
    }
}

int qnzkna::MarkManage::IMarkManageImpl::sendSystemUpdate(ESystemUpdateType systemUpdateType, ESystemOperatorType systemOperatorType, const char *objectID)
{
    char sendBuf[sizeof(NetHeader) + sizeof(SystemUpdate)] = { 0 };
    NetHeader *pHeader = (NetHeader *)sendBuf;
    SystemUpdate *su = (SystemUpdate *)(sendBuf + sizeof(NetHeader));
    pHeader->ucType = NETHEADER_UCTYPE_SYSTEM_UPDATE;
    pHeader->ucSubType = 0x01;
    pHeader->unLength = sizeof(NetHeader) + sizeof(SystemUpdate);

    pHeader->unSeatType = 1;
    pHeader->unNodeType = 1;

    su->unMainType = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_SYSTEMUPDATE;
    memcpy(su->Operator, loginName.data(), loginName.size());
    su->updateType = systemUpdateType;
    su->operatorType = systemOperatorType;
    memcpy(su->unObjectId, objectID, strlen(objectID));

    int len = LY_MarkManageServiceInterface::Get().SendData(sendBuf, sizeof(sendBuf));
    return len;
}

bool qnzkna::MarkManage::IMarkManageImpl::ISExisted(std::string strID)
{
    for (auto data : m_MarkListVec)
    {
        if (0 == strcmp(strID.c_str(), data.GetID().c_str()))
        {
            return true;
        }
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::RemoveDBData(std::string strID)
{

    if (!IsIDExisted(strID))
    {
        return false;
    }

    QString strSql("DELETE FROM ZZMB_XTMBXX where MBBH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);

    if (success)
    {
        sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Mark, ESystemOperatorType::ESystemOperatorType_Delete, strID.data());
    }

    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::EditMissionDBData(SystemMarkInfo &data)
{
    std::string strID = data.GetID();

    if (IsMissionIDExisted(strID))
    {
        QString strSql("UPDATE ZZMB_RWMB set ");
        strSql += "`RWLX` = '" + QString::number(data.GetMissionTypeENum(data.GetMissionType())) + "' ";
        strSql += "where `MBBH` = '" + QString::fromStdString(data.GetID()) + "'";
        QSqlQuery query;
        bool success = query.exec(strSql);
        return success;
    }
    else
    {
        QString strSql("INSERT INTO ZZMB_RWMB (`MBBH`, `RWLX`) VALUES (");
        strSql += "'" + QString::fromStdString(data.GetID()) + "',";
        strSql += "'" + QString::number(data.GetMissionTypeENum(data.GetMissionType())) + "'";
        strSql += ")";
        QSqlQuery query;
        bool success = query.exec(strSql);
        return success;
    }
}

bool qnzkna::MarkManage::IMarkManageImpl::RemoveMissionDBData(std::string strID)
{

    if (!IsMissionIDExisted(strID))
    {
        return false;
    }

    QString strSql("DELETE FROM ZZMB_RWMB where MBBH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success)
    {
        sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Mark, ESystemOperatorType::ESystemOperatorType_Add, strID.data());
    }
    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::IsMissionIDExisted(std::string strID)
{
    QString strSql("SELECT  * FROM `ZZMB_RWMB` WHERE `MBBH` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success == false)
    {
        return false;
    }
    if (query.size() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool qnzkna::MarkManage::IMarkManageImpl::EditZoneDBData(CZoneInfo &data)
{
    std::string strID = data.GetID();

    if (!IsZoneIDExisted(strID))
    {
        return false;
    }

    QString strSql("UPDATE ZZQY_List set ");
    strSql += "`KYMC` = '"			+ QString::fromStdString(data.GetName()) + "',";
    strSql += "`KYSX` = "			+ QString::number(data.GetAttributeENum(data.GetAttribute())) + ",";
    strSql += "`KYXZ` = "			+ QString::number(data.GetShapeENum(data.GetShape())) + ",";
    strSql += "`CJSJ` = '"			+ QString::fromStdString(data.GetCreateTime()) + "',";
    strSql += "`GLMBID` = '"		+ QString::fromStdString(data.GetMarkID()) + "',";
    strSql += "`KYMS` = '"			+ QString::fromStdString(data.GetDescribe()) + "', ";
    strSql += "`XY` = "				+ QString::number(data.GetShowStates()) + " ";
    strSql += "where `KYBH` = '"	+ QString::fromStdString(data.GetID()) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success == false)
    {
        return false;
    }

    if (data.GetShape() == EZoneShape_Circle)
    {
        RemoveZoneDBData_Ciecle(strID);
        RemoveZoneDBData_Polygon(strID);
        RemoveZoneDBData_Sector(strID);
        success = AddZoneBData_Ciecle(data);
    }
    else if (data.GetShape() == EZoneShape_Polygon)
    {
        RemoveZoneDBData_Ciecle(strID);
        RemoveZoneDBData_Polygon(strID);
        RemoveZoneDBData_Sector(strID);
        success = AddZoneBData_Polygon(data);
    }
    else if (data.GetShape() == EZoneShape_Sector)
    {
        RemoveZoneDBData_Ciecle(strID);
        RemoveZoneDBData_Polygon(strID);
        RemoveZoneDBData_Sector(strID);
        success = AddZoneBData_Sector(data);
    }

    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::EditZoneShowDatesDBData(CZoneInfo &data)
{
    QString strSql("UPDATE ZZQY_List set ");
    strSql += "`XY` = " + QString::number(data.GetShowStates()) + " ";
    strSql += "where `KYBH` = '" + QString::fromStdString(data.GetID()) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::AddZoneBData(CZoneInfo &data)
{
    std::string strID = data.GetID();

    if (IsZoneIDExisted(strID))
    {
        return false;
    }

    QString strSql("INSERT INTO ZZQY_List (`KYBH`, `KYMC`, `KYSX`, `KYXZ`, `CJSJ`, `GLMBID`, `KYMS`,`XY`) VALUES (");
    strSql += "'" + QString::fromStdString(data.GetID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetName()) + "',";
    strSql += "'" + QString::number(data.GetAttributeENum(data.GetAttribute())) + "',";
    strSql += "'" + QString::number(data.GetShapeENum(data.GetShape())) + "',";
    strSql += "'" + QString::fromStdString(data.GetCreateTime()) + "',";
    strSql += "'" + QString::fromStdString(data.GetMarkID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetDescribe()) + "',";
    strSql += "'" + QString::number(data.GetShowStates()) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success == false)
    {
        return false;
    }

    if (data.GetShape() == EZoneShape_Circle)
    {
        success = AddZoneBData_Ciecle(data);
    }
    else if (data.GetShape() == EZoneShape_Polygon)
    {
        success = AddZoneBData_Polygon(data);
    }
    else if (data.GetShape() == EZoneShape_Sector)
    {
        success = AddZoneBData_Sector(data);
    }
    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::RemoveZoneDBData(std::string strID)
{

    if (!IsZoneIDExisted(strID))
    {
        return false;
    }

    QString strSql("DELETE FROM ZZQY_List where KYBH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::IsZoneIDExisted(std::string strID)
{
    QString strSql("SELECT  * FROM `ZZQY_List` WHERE `KYBH` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success == false)
    {
        return false;
    }
    if (query.size() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool qnzkna::MarkManage::IMarkManageImpl::AddZoneBData_Polygon(CZoneInfo &data)
{
    for (auto info : data.GetPoints())
    {
        QString strSql("INSERT INTO ZZQY_Polygon (`GLBH`, `Lon`, `Lat`, `Height`) VALUES (");
        strSql += "'" + QString::fromStdString(data.GetID()) + "',";
        strSql += "'" + QString::number(info.dLon, 'f', 7) + "',";
        strSql += "'" + QString::number(info.dLat, 'f', 7) + "',";
        strSql += "'" + QString::number(info.dHeight, 'f', 2) + "'";
        strSql += ")";
        QSqlQuery query;
        bool success = query.exec(strSql);
        if (success == false)
        {
            return success;
        }
    }
    return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::AddZoneBData_Sector(CZoneInfo &data)
{
    for (auto info : data.GetPoints())
    {
        QString strSql("INSERT INTO ZZQY_Sector (`GLBH`, `Lon`, `Lat`, `Height`,`Radius`,`StartAngle`,`EndAngle`) VALUES (");
        strSql += "'" + QString::fromStdString(data.GetID()) + "',";
        strSql += "'" + QString::number(info.dLon, 'f', 7) + "',";
        strSql += "'" + QString::number(info.dLat, 'f', 7) + "',";
        strSql += "'" + QString::number(info.dHeight, 'f', 2) + "',";
        strSql += "'" + QString::number(info.dRadius, 'f', 2) + "',";
        strSql += "'" + QString::number(info.dStartAngle, 'f', 2) + "',";
        strSql += "'" + QString::number(info.dEndAngle, 'f', 2) + "'";
        strSql += ")";
        QSqlQuery query;
        qDebug() << strSql;
        bool success = query.exec(strSql);
        if (success == false)
        {
            return success;
        }
    }
    return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::AddZoneBData_Ciecle(CZoneInfo &data)
{
    for (auto info : data.GetPoints())
    {
        QString strSql("INSERT INTO ZZQY_Ciecle (`QYGLBH`, `Lon`, `Lat`, `Height`,`Radius`) VALUES (");
        strSql += "'" + QString::fromStdString(data.GetID()) + "',";
        strSql += "'" + QString::number(info.dLon, 'f', 7) + "',";
        strSql += "'" + QString::number(info.dLat, 'f', 7) + "',";
        strSql += "'" + QString::number(info.dHeight, 'f', 2) + "',";
        strSql += "'" + QString::number(info.dRadius, 'f', 2) + "'";
        strSql += ")";
        QSqlQuery query;
        bool success = query.exec(strSql);
        if (success == false)
        {
            return success;
        }
    }
    return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::RemoveZoneDBData_Polygon(std::string strID)
{
    QString strSql("DELETE FROM ZZQY_Polygon where GLBH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::RemoveZoneDBData_Ciecle(std::string strID)
{
    QString strSql("DELETE FROM ZZQY_Ciecle where QYGLBH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::RemoveZoneDBData_Sector(std::string strID)
{
    QString strSql("DELETE FROM ZZQY_Sector where GLBH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::AddPosDBData(SystemMarkInfo &data)
{
    std::string strID = data.GetID();

    QString strSql("INSERT INTO ZZMB_XTMBWZ (`SJ`, `MBBH`, `JD`, `WD`, `GD`) VALUES (");
    strSql += "'" + QString::fromStdString(data.GetFindTime()) + "',";
    strSql += "'" + QString::fromStdString(data.GetID()) + "',";
    strSql += "'" + QString::number(data.GetLon(), 'f', 7) + "',";
    strSql += "'" + QString::number(data.GetLat(), 'f', 7) + "',";
    strSql += "'" + QString::number(data.GetHeight(), 'f', 2) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::MarkManage::IMarkManageImpl::RemovePosDBData(std::string strID)
{
    QString strSql("DELETE FROM ZZMB_XTMBWZ where MBBH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

void qnzkna::MarkManage::IMarkManageImpl::LoadMissionTypeDBData()
{
    if (m_MarkListVec.size() == 0)
    {
        return;
    }

    QSqlQuery query;
    bool success = query.exec("SELECT `MBBH`,`RWLX` FROM `ZZMB_RWMB`");
    if (success)
    {
        while (query.next())
        {
            QString str1 = query.value(0).toString();
            QString str2 = query.value(1).toString();

            for (auto &data : m_MarkListVec)
            {
                if (QString::compare(str1, QString::fromStdString(data.GetID())) == 0)
                {
                    data.SetMissionType(data.GetMissionTypeENum(str2.toInt()));
                    break;
                }
            }
        }
    }
}

void qnzkna::MarkManage::IMarkManageImpl::LoadZoneDBData()
{
    QMutexLocker locker(&sMutexAirSapce);

    QSqlQuery query;
    bool success = query.exec("SELECT `KYBH`, `KYMC`, `KYSX`, `KYXZ`, `CJSJ`, `GLMBID`, `KYMS`,`XY` FROM `ZZQY_List`");
    if (success)
    {
        m_ZoneInfoVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());
			QString str8 = tr2(query.value(7).toString().toLatin1().data());

            CZoneInfo stInfo;
            stInfo.SetID(str1.toStdString());
            stInfo.SetName(str2.toStdString());
            stInfo.SetAttribute(stInfo.GetAttributeENum(str3.toInt()));
            stInfo.SetShape(stInfo.GetShapeENum(str4.toInt()));
            stInfo.SetCreateTime(str5.toStdString());
            stInfo.SetMarkID(str6.toStdString());
            stInfo.SetDescribe(str7.toStdString());
            stInfo.SetShowStates(str8.toInt());

            if (stInfo.GetShape() == EZoneShape_Polygon)
            {
                LoadZoneDBData_Polygon(stInfo);
            }
            else if (stInfo.GetShape() == EZoneShape_Circle)
            {
                LoadZoneDBData_Ciecle(stInfo);
            }
            else if (stInfo.GetShape() == EZoneShape_Sector)
            {
                LoadZoneDBData_Sector(stInfo);
            }
            m_ZoneInfoVec.push_back(stInfo);
        }
    }
}

void qnzkna::MarkManage::IMarkManageImpl::LoadZoneDBData_Polygon(CZoneInfo &data)
{
    QSqlQuery query;
    QString strSql("SELECT `Lon`, `Lat`, `Height` FROM `ZZQY_Polygon` WHERE `GLBH` = ");
    strSql += "'" + QString::fromStdString(data.GetID()) + "'";

    bool success = query.exec(strSql);
    if (success)
    {
        data.GetPoints().clear();
        ZonePointVec stVec;
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());

            ZonePoint stPoint;
            stPoint.dLon = str1.toDouble();
            stPoint.dLat = str2.toDouble();
            stPoint.dHeight = str3.toDouble();
            stVec.push_back(stPoint);
        }
        data.SetPoints(stVec);
    }
}

void qnzkna::MarkManage::IMarkManageImpl::LoadZoneDBData_Ciecle(CZoneInfo &data)
{
    QSqlQuery query;
    QString strSql("SELECT `Lon`, `Lat`, `Height`,`Radius` FROM `ZZQY_Ciecle` WHERE `QYGLBH` = ");
    strSql += "'" + QString::fromStdString(data.GetID()) + "'";

    bool success = query.exec(strSql);
    if (success)
    {
        data.GetPoints().clear();
        if (query.size() > 1)
        {
            return;
        }
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());

            ZonePoint stPoint;
            stPoint.dLon = str1.toDouble();
            stPoint.dLat = str2.toDouble();
            stPoint.dHeight = str3.toDouble();
            stPoint.dRadius = str4.toDouble();

            ZonePointVec stVec;
            stVec.push_back(stPoint);

            data.SetPoints(stVec);
        }
    }
}

void qnzkna::MarkManage::IMarkManageImpl::LoadZoneDBData_Sector(CZoneInfo &data)
{
    QSqlQuery query;
    QString strSql("SELECT `Lon`, `Lat`, `Height`,`Radius`,`StartAngle`,`EndAngle` FROM `ZZQY_Sector` WHERE `GLBH` = ");
    strSql += "'" + QString::fromStdString(data.GetID()) + "'";

    bool success = query.exec(strSql);
    if (success)
    {
        data.GetPoints().clear();
        if (query.size() > 1)
        {
            return;
        }
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLatin1().data());
			QString str6 = tr2(query.value(5).toString().toLatin1().data());

            ZonePoint stPoint;
            stPoint.dLon = str1.toDouble();
            stPoint.dLat = str2.toDouble();
            stPoint.dHeight = str3.toDouble();
            stPoint.dRadius = str4.toDouble();
            stPoint.dStartAngle = str5.toInt();
            stPoint.dEndAngle = str6.toInt();

            ZonePointVec stVec;
            stVec.push_back(stPoint);

            data.SetPoints(stVec);
        }
    }
}

int qnzkna::MarkManage::IMarkManageImpl::GenerateIconID()
{
    m_nIconID++;
    return m_nIconID;
}

bool qnzkna::MarkManage::IMarkManageImpl::JudeActionMark(CRelationActionMarkVec &Rvec, const std::string &strID)
{
    for (auto data : Rvec)
    {
        if (0 == strcmp(strID.c_str(), data.GetMarkID().c_str()))
        {
            return true;
        }
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::IsIDExisted(std::string strID)
{
    QString strSql("SELECT  * FROM `ZZMB_XTMBXX` WHERE `MBBH` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success == false)
    {
        return false;
    }
    if (query.size() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

SystemMarkInfo qnzkna::MarkManage::IMarkManageImpl::GetMark(const std::string &strID) const
{
    QMutexLocker locker(&sMutex);
    SystemMarkInfo stInfo;
    for (auto data : m_MarkListVec)
    {
        if (0 == strcmp(strID.c_str(), data.GetID().c_str()))
        {
            stInfo = data;
            break;
        }
    }
    return stInfo;
}

std::vector<QString> qnzkna::MarkManage::IMarkManageImpl::GetMarkIDListFromDB() const
{
    std::vector<QString> MarkIDList;
    QSqlQuery query("SELECT `MBBH`, `MBMC` FROM `ZZMB_XTMBXX`");
    if (query.exec())
    {
        while (query.next())
        {
            MarkIDList.push_back(query.value(0).toString());
        }
    }

    return MarkIDList;
}

std::vector<QString> qnzkna::MarkManage::IMarkManageImpl::GetMarkIDListFromDB_ActivatingAction() const
{
    std::vector<QString> MarkIDList;
    if (IMarkManageServiceActivator::Get().getPTaskManageService() == nullptr)
    {
        return MarkIDList;
    }

    const int nActionID = IMarkManageServiceActivator::Get().getPTaskManageService()->GetActivatingActionID();

    QString strSql("SELECT `MBBH`, `XDBH` FROM `Relation_RWLB_MB` WHERE `XDBH` = ");
    strSql += "'" + QString::number(nActionID) + "'";
    QSqlQuery query(strSql);
    if (query.exec())
    {
        while (query.next())
        {
            MarkIDList.push_back(query.value(0).toString());
        }
    }

    return MarkIDList;
}

SystemMarkInfo qnzkna::MarkManage::IMarkManageImpl::GetMarkFromDB(const QString &strID) const
{
    SystemMarkInfo stInfo;
    QString strSql("SELECT `MBBH`, `MBMC`, `MBLX`, `MBSX`, `MBCZ`, `MBJD`, `MBWD`, `MBGD`, `MBSD`, `MBHX`, `MBMS` , `VXDJ` , `FXSJ`, `FXZB` FROM `ZZMB_XTMBXX` WHERE `MBBH` = ");
    strSql += "'" + strID + "'";
    QSqlQuery query(strSql);
    if ((query.exec()) && (query.size() == 1) && query.next())
    {
        const QString str1 = (query.value(0).toString());
        const QString str2 = (query.value(1).toString());
        const QString str3 = (query.value(2).toString());
        const QString str4 = (query.value(3).toString());
        const QString str5 = (query.value(4).toString());
        const QString str6 = (query.value(5).toString());
        const QString str7 = (query.value(6).toString());
        const QString str8 = (query.value(7).toString());
        const QString str9 = (query.value(8).toString());
        const QString str10 = (query.value(9).toString());
        const QString str11 = (query.value(10).toString());
        const QString str12 = (query.value(11).toString());
        const QString str13 = (query.value(12).toString());
        const QString str14 = (query.value(13).toString());

        stInfo.SetID(str1.toStdString());
        stInfo.SetName(str2.toStdString());
        stInfo.SetType(stInfo.GetTypeENum(str3.toInt()));
        stInfo.SetAttribute(stInfo.GetAttributeENum(str4.toInt()));
        stInfo.SetMaterial(stInfo.GetMaterialENum(str5.toInt()));
        stInfo.SetLon(str6.toDouble());
        stInfo.SetLat(str7.toDouble());
        stInfo.SetHeight(str8.toDouble());
        stInfo.SetSpeed(str9.toDouble());
        stInfo.SetPitch(str10.toDouble());
        stInfo.SetDirections(str11.toStdString());
        stInfo.SetThreatenDegree(stInfo.GetThreatenDegreeENum(str12.toInt()));
        stInfo.SetFindTime(str13.toStdString());
        stInfo.SetFindEqu(str14.toStdString());
    }
    return stInfo;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetTaregt(SystemMarkInfo &data)
{
    QMutexLocker locker(&sMutex);
    auto itr = m_MarkListVec.begin();
    for (; itr != m_MarkListVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetID().c_str(), data.GetID().c_str()))
        {
            int nIconID = itr->GetIconID();
            int nMapIconID = itr->GetMapIconID();

			char m_cUuid[UID_LENGTH] = {};
			const QByteArray &ba = QUuid::createUuid().toRfc4122();
			memcpy(m_cUuid, ba.data(), UID_LENGTH);
			data.SetUuid(m_cUuid);

            ESystemMarkType eType = itr->GetType();
            ESystemMarkAttribute eAttribute = itr->GetAttribute();
            LY_MarkManageService::Get().DeleteIcon(eType, eAttribute, nMapIconID);
            LY_MarkManageService::Get().Delete3DIcon(*itr);

            *itr = data;

            bool bresutl = EditDBData(data);
            if (bresutl == false)
            {
                return false;
            }
            EditMissionDBData(data);

            itr->SetIconID(nIconID);

            double dLon = itr->GetLon();
            double dLat = itr->GetLat();
            VecStr vs;
            vs.push_back(itr->GetIconID());
            vs.push_back(itr->GetName().c_str());
            vs.push_back(0);
            eType = itr->GetType();
            eAttribute = itr->GetAttribute();
            itr->SetMapIconID(LY_MarkManageService::Get().EditIcon(dLon, dLat, eType, eAttribute, vs));
            LY_MarkManageService::Get().Add3DIcon(*itr);

            return true;
        }
    }

    if (itr == m_MarkListVec.end())
    {

        int nID = GenerateIconID();
        data.SetIconID(nID);
        double dLon = data.GetLon();
        double dLat = data.GetLat();
        ESystemMarkType eType = data.GetType();
        ESystemMarkAttribute eAttribute = data.GetAttribute();

        VecStr vs;
        vs.push_back(nID);
        vs.push_back(data.GetName().c_str());
        vs.push_back(0);
        data.SetMapIconID(LY_MarkManageService::Get().AddIcon(dLon, dLat, eType, eAttribute, vs));
        LY_MarkManageService::Get().Add3DIcon(data);

        m_MarkListVec.push_back(data);
        if (AddDBData(data) == false)
        {
            return false;
        }

        EditMissionDBData(data);

        if (IMarkManageServiceActivator::Get().getPTaskManageService() != nullptr)
        {
            int nActionID = IMarkManageServiceActivator::Get().getPTaskManageService()->AddRelationMark(data.GetID());
        }

        return true;

    }

    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetTaregt(SystemMarkInfo &data, std::string strOldID)
{
    QMutexLocker locker(&sMutex);
    auto itr = m_MarkListVec.begin();
    for (; itr != m_MarkListVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetID().c_str(), strOldID.c_str()))
        {
            int nIconID = itr->GetIconID();
            int nMapIconID = itr->GetMapIconID();

            ESystemMarkType eType = itr->GetType();
            ESystemMarkAttribute eAttribute = itr->GetAttribute();
            LY_MarkManageService::Get().DeleteIcon(eType, eAttribute, nMapIconID);
            LY_MarkManageService::Get().Delete3DIcon(*itr);

            *itr = data;

            bool bresult = EditDBData(data, strOldID);
            if (bresult == false)
            {
                return false;
            }

            EditMissionDBData(data);

            itr->SetIconID(nIconID);

            double dLon = itr->GetLon();
            double dLat = itr->GetLat();
            VecStr vs;
            vs.push_back(itr->GetIconID());
            vs.push_back(itr->GetName().c_str());
            vs.push_back(0);
            eType = itr->GetType();
            eAttribute = itr->GetAttribute();
            itr->SetMapIconID(LY_MarkManageService::Get().EditIcon(dLon, dLat, eType, eAttribute, vs));
            LY_MarkManageService::Get().Add3DIcon(*itr);
            return true;
        }
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetMovingTaregt(SystemMarkInfo &data)
{
    bool breslut = SetTaregt(data);
    QDateTime datatime = QDateTime::currentDateTime();
    QString strTime = datatime.toString("yyyy-MM-dd hh:mm:ss");

    SystemMarkInfo stInfo;
    stInfo.SetFindTime(strTime.toStdString());
    stInfo.SetID(data.GetID());
    stInfo.SetLat(data.GetLat());
    stInfo.SetLon(data.GetLon());
    stInfo.SetHeight(data.GetHeight());

    breslut = AddPosDBData(stInfo);
    if (breslut == true)
    {
        m_MarkPosListVec.push_back(stInfo);
    }
    return breslut;
}

bool qnzkna::MarkManage::IMarkManageImpl::UpdateTaregtState(SystemMarkInfo &data)
{
    QMutexLocker locker(&sMutex);
    bool bresutl = EditDBData(data);
    return bresutl;
}

bool qnzkna::MarkManage::IMarkManageImpl::RmoveTartget(std::string &strID)
{
    QMutexLocker locker(&sMutex);

    auto itr = m_MarkListVec.begin();
    for (; itr != m_MarkListVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetID().c_str(), strID.c_str()))
        {

            int nID = itr->GetMapIconID();
            ESystemMarkType eType = itr->GetType();
            ESystemMarkAttribute eAttribute = itr->GetAttribute();
            LY_MarkManageService::Get().DeleteIcon(eType, eAttribute, nID);
            LY_MarkManageService::Get().Delete3DIcon(*itr);

            m_MarkListVec.erase(itr);
            RemoveMissionDBData(strID);
            RemoveDBData(strID);
            RemovePosDBData(strID);
            LoadPosDataBase();
            return true;
        }
    }
    return false;

}

bool qnzkna::MarkManage::IMarkManageImpl::MergeMarkAToB(int actionID, std::string MarkAId, std::string MarkBId)
{
    QMutexLocker locker(&sMutex);

    auto itr = m_MarkListVec.begin();
    for (; itr != m_MarkListVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetID().c_str(), MarkAId.c_str()))
        {

            int nID = itr->GetMapIconID();
            ESystemMarkType eType = itr->GetType();
            ESystemMarkAttribute eAttribute = itr->GetAttribute();
            LY_MarkManageService::Get().DeleteIcon(eType, eAttribute, nID);
            LY_MarkManageService::Get().Delete3DIcon(*itr);

            m_MarkListVec.erase(itr);
            RemoveMissionDBData(MarkAId);
            RemoveDBData(MarkAId);
            RemovePosDBData(MarkAId);
            RWLBsetNullForMarkID(MarkAId);
            WQPTLBsetNullForMarkID(MarkAId);
            LoadPosDataBase();
            return true;
        }
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::RmoveAllTartget()
{
    QMutexLocker locker(&sMutex);

    for (auto data : m_MarkListVec)
    {
        ESystemMarkType eType = data.GetType();
        ESystemMarkAttribute eAttribute = data.GetAttribute();
        int nID = data.GetMapIconID();
        if (nID != -1)
        {
            LY_MarkManageService::Get().DeleteIcon(eType, eAttribute, nID);
            LY_MarkManageService::Get().Delete3DIcon(data);
        }
    }

    m_MarkListVec.clear();
    return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::WakeUpMarkWindow(SystemMarkInfo &data)
{
    LY_MarkManageService::Get().GetDlgMark()->SetDataType(1);
    LY_MarkManageService::Get().GetDlgMark()->SetWindowInfo(data);
    LY_MarkManageService::Get().GetDlgMark()->show();
    return true;
}

CZoneInfoVec qnzkna::MarkManage::IMarkManageImpl::GetZoneList() const
{
    QMutexLocker locker(&sMutexAirSapce);
    return m_ZoneInfoVec;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetZone(CZoneInfo &data)
{
    QMutexLocker locker(&sMutexAirSapce);

    auto itr = m_ZoneInfoVec.begin();
    for (; itr != m_ZoneInfoVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetID().c_str(), data.GetID().c_str()))
        {
            *itr = data;

            EditZoneDBData(data);
            break;
        }
    }

    if (itr == m_ZoneInfoVec.end())
    {
        m_ZoneInfoVec.push_back(data);
        AddZoneBData(data);
    }

    return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::RmoveZone(const std::string &strID)
{
    QMutexLocker locker(&sMutexAirSapce);

    auto itr = m_ZoneInfoVec.begin();
    for (; itr != m_ZoneInfoVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetID().c_str(), strID.c_str()))
        {
            RemoveZoneDBData(strID);
            m_ZoneInfoVec.erase(itr);
            return true;
        }
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetZoneShowStates(const std::string &strID, bool bShow)
{
    QMutexLocker locker(&sMutexAirSapce);

    for (auto &Info : m_ZoneInfoVec)
    {
        if (0 == strcmp(Info.GetID().c_str(), strID.c_str()))
        {
            Info.SetShowStates(bShow);
            EditZoneShowDatesDBData(Info);

            locker.unlock();

            LY_MarkManageService::Get().GetDlgZoneList()->UpdataWindowData_Zone();
            LY_MarkManageService::Get().GetDlgZoneList()->UpdateListData_CombatZone();

            return true;
        }
    }
    return false;
}

CZoneInfo qnzkna::MarkManage::IMarkManageImpl::GeAirApaceByID(const std::string &strID)
{
    QMutexLocker locker(&sMutexAirSapce);
    CZoneInfo stinfo;

    int pos = strID.find(";");
    std::string str = strID.substr(0, pos);

    for (auto &Info : m_ZoneInfoVec)
    {
        if (0 == strcmp(Info.GetID().c_str(), str.c_str()))
        {
            stinfo = Info;
            return stinfo;
        }
    }
    return stinfo;

}

CZoneInfoVec qnzkna::MarkManage::IMarkManageImpl::GeAirApaceByMarkID(std::string &strID)
{
    QMutexLocker locker(&sMutexAirSapce);
    CZoneInfoVec stInfoVec;

    for (auto &Info : m_ZoneInfoVec)
    {
        if (0 == strcmp(Info.GetMarkID().c_str(), strID.c_str()))
        {
            stInfoVec.push_back(Info);
        }
    }
    return stInfoVec;

}

bool qnzkna::MarkManage::IMarkManageImpl::SetMarkImage(const std::string &strID, const QString &strImgPath)
{
    if ("" == strID || "" == strImgPath)
    {
        return false;
    }
    QMutexLocker locker(&sMutex);

    QFile dir(strImgPath);
    if (!dir.exists())
    {
        qDebug() << "Error Image File Path is Error. \n";
        return false;
    }

    QPixmap inPixmap;
    inPixmap.load(strImgPath);

    QByteArray inByteArray;
    QBuffer inBuffer(&inByteArray);
    inBuffer.open(QIODevice::WriteOnly);
    inPixmap.save(&inBuffer, "PNG");
    if (inByteArray.size() > 16 * 1024 * 1024)
    {
        qDebug() << "Error Image File Size is To Big.\n";
        return false;
    }

    QString strFileName = QString(QDateTime::currentDateTime().toString("yyyy_MM_dd-hh:mm:ss.zzz"));
    QSqlQuery query;
    QString strSql = QStringLiteral("INSERT INTO XDMB_ZP (ZPMC,ZPNR,MBBH) VALUES (:ZPMC, :ZPNR, :MBBH)");
    bool b =  query.prepare(strSql);
    query.bindValue(":MBBH", strID.c_str());
    query.bindValue(":ZPMC", strFileName);
    query.bindValue(":ZPNR", inByteArray);
    if (!query.exec())
    {
        qDebug() << "Error inserting MB image into table.\n";
        return false;
    }
    return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetMarkImage(const std::string &strID, const QPixmap &InImg)
{
    if ("" == strID)
    {
        return false;
    }
    QMutexLocker locker(&sMutex);

    QByteArray inByteArray;
    QBuffer inBuffer(&inByteArray);
    inBuffer.open(QIODevice::WriteOnly);
    InImg.save(&inBuffer, "PNG");
    if (inByteArray.size() > 16 * 1024 * 1024)
    {
        qDebug() << "Error Image File Size is To Big.\n";
        return false;
    }

    QString strFileName = QString(QDateTime::currentDateTime().toString("yyyy_MM_dd-hh:mm:ss.zzz"));
    QSqlQuery query;
    QString strSql = QStringLiteral("INSERT INTO `XDMB_ZP` (ZPMC,ZPNR,MBBH) VALUES ('%1', :imageData,'%2')").arg(strFileName).arg(strID.c_str());
    query.prepare(strSql);
    query.bindValue(":imageData", inByteArray);
    if (!query.exec())
    {
        qDebug() << "Error inserting MB image into table.\n";
        return false;
    }
    return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::GetMarkImageNamesList(const std::string &strID, std::vector<std::string> *ImageList)
{
    if ("" == strID || nullptr == ImageList)
    {
        return false;
    }
    QMutexLocker locker(&sMutex);
    QSqlQuery query;
    QString strSql = QStringLiteral("SELECT `ZPMC` FROM `XDMB_ZP` WHERE `MBBH` like '%1'").arg(strID.c_str());
    bool success = query.exec(strSql);
    if (success)
    {
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
            ImageList->push_back(str1.toStdString());
        }
        return true;
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::GetMarkImageData(const std::string &imageName, QPixmap *OutImage)
{
    if ("" == imageName || nullptr == OutImage)
    {
        return false;
    }
    QMutexLocker locker(&sMutex);
    QSqlQuery query;
    QString strSql = QStringLiteral("SELECT `ZPNR` FROM `XDMB_ZP` WHERE `ZPMC` like '%1'").arg(imageName.c_str());
    bool success = query.exec(strSql);
    if (success)
    {
        while (query.next())
        {
            const QByteArray &arr = query.value(0).toByteArray();
            OutImage->loadFromData(arr, "PNG");
            return true;
        }
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::RemoveMarkImageData(const std::string &imageName)
{
    if ("" == imageName)
    {
        return false;
    }
    QMutexLocker locker(&sMutex);
    QSqlQuery query;
    QString strSql = QStringLiteral("DELETE FROM `XDMB_ZP` WHERE `ZPMC` like '%1'").arg(imageName.c_str());
    bool success = query.exec(strSql);
    if (success)
    {
        return true;
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetMarkVideo(const std::string &strID, const QString &strVideoPath)
{
    if ("" == strID || "" == strVideoPath)
    {
        return false;
    }
    QMutexLocker locker(&sMutex);

    QFile file(strVideoPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open video file!\n";
        return false;
    }

    QString strFileName = QString(QDateTime::currentDateTime().toString("yyyy_MM_dd-hh:mm:ss.zzz"));
    QSqlQuery query;
    QString strSql = QStringLiteral("INSERT INTO `XDMB_SP` (SPMC,SPNR,MBBH) VALUES ('%condition1%', :videoData,'%condition2%')").arg(strFileName).arg(strID.c_str());
    query.prepare(strSql);
    query.bindValue(":data", file.readAll());
    if (!query.exec())
    {
        qDebug() << "Failed to store video in database!\n";
        return 1;
    }
    return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetMarkVideo(const std::string &strID, const QByteArray &InVideo)
{
    if ("" == strID || 0 == InVideo.size())
    {
        return false;
    }
    QMutexLocker locker(&sMutex);

    if (InVideo.size() > 4*1024*1024*102)
    {
        qDebug() << "Video file is too big!\n";
        return false;
    }

    QString strFileName = QString(QDateTime::currentDateTime().toString("yyyy_MM_dd-hh:mm:ss.zzz"));
    QSqlQuery query;
    QString strSql = QStringLiteral("INSERT INTO `XDMB_SP` (SPMC,SPNR,MBBH) VALUES ('%condition1%', :videoData,'%condition2%')").arg(strFileName).arg(strID.c_str());
    query.prepare(strSql);
    query.bindValue(":data", InVideo);
    if (!query.exec())
    {
        qDebug() << "Failed to store video in database!\n";
        return false;
    }
    return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::GetMarkVideoNamesList(const std::string &strID, std::vector<std::string> *VideoList)
{
    if ("" == strID || nullptr == VideoList)
    {
        return false;
    }
    QMutexLocker locker(&sMutex);
    QSqlQuery query;
    QString strSql = QStringLiteral("SELECT `SPMC` FROM `XDMB_SP` WHERE `MBBH` like '%1'").arg(strID.c_str());
    bool success = query.exec(strSql);
    if (success)
    {
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
            VideoList->push_back(str1.toStdString());
        }
        return true;
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::GetMarkVideoData(const std::string &videoName, QByteArray *VideoData)
{
    if ("" == videoName || nullptr == VideoData)
    {
        return false;
    }
    QMutexLocker locker(&sMutex);
    QSqlQuery query;
    QString strSql = QStringLiteral("SELECT `SPNR` FROM `XDMB_SP` WHERE `SPMC` like '%1'").arg(videoName.c_str());
    bool success = query.exec(strSql);
    if (success)
    {
        while (query.next())
        {
            *VideoData = query.value(0).toByteArray();
            return true;
        }
    }
    return false;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetThreatenSequencePosition(const double& dLon, const double& dLat, const double& dAlt)
{
	if (dLon == 0 || dLat == 0)
	{
		return false;
	}

	m_dTSLon = dLon;
	m_dTSLat = dLat;
	m_dTSAlt = dAlt;
	return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::GetThreatenSequencePosition(double& dLon, double& dLat, double& dAlt)
{
	dLon = m_dTSLon;
	dLat = m_dTSLat;
	dAlt = m_dTSAlt;
	return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetThreatenSequenceTakeOffpoint(const double& dLon, const double& dLat, const double& dAlt)
{
	if (dLon == 0 || dLat == 0)
	{
		return false;
	}

	m_dTSTakeOffLon = dLon;
	m_dTSTakeOffLat = dLat;
	m_dTSTakeOffAlt = dAlt;
	return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::GetThreatenSequenceTakeOffpoint(double& dLon, double& dLat, double& dAlt)
{
	dLon = m_dTSTakeOffLon;
	dLat = m_dTSTakeOffLat;
	dAlt = m_dTSTakeOffAlt;
	return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::SetThreatenSequenceIDList(QStringList& strIDList)
{
	m_ThreatenSequenceIDList = strIDList;
	return true;
}

bool qnzkna::MarkManage::IMarkManageImpl::GetThreatenSequenceIDList(QStringList& strIDList)
{
	strIDList = m_ThreatenSequenceIDList;
	return true;
}
