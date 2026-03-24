#ifndef ITASKMANAGEIMPL_H
#define ITASKMANAGEIMPL_H

#include <time.h>
#include <QMutex>
#include <QXmlStreamReader>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/NebulaTask/NebulaMissionPlatform.h"
#include "DataManage/NebulaTask/NebulaMissionPlatformLoad.h"
#include "DataManage/NebulaTask/NebulaMissionLine.h"
#include "DataManage/NebulaTask/NebulaResources.h"
#include "DataManage/NebulaTask/NebulaScheme.h"
#include "DataManage/NebulaTask/NebulaSchemePoint.h"
#include "DataManage/NebulaTask/NebulaSchemeAmmo.h"
#include "DataManage/NebulaTask/NebulaSchemeSensor.h"
#include "DataManage/NebulaTask/NebulaSchemePointFA.h"
#include "DataManage/NebulaTask/NebulaActionSchemeGL.h"
#include "DataManage/NebulaTask/NebulaOperationalInformation.h"
#include "Protocol/XygsSystemControl/SystemControl.h"
#include "TinyXML/tinyxml.h"

#define FORM_UNIT_SCALE_LEN			100

namespace qnzkna
{
namespace TaskManage
{

class ITaskManageImpl:
    public INebulaTaskManageService
{
public:
    ITaskManageImpl(void);
    ~ITaskManageImpl(void);
private:
    std::string								m_strDatabasePath;
    NebulaTaskPtrMap						m_mapNebulaTask;

    unsigned int							m_nCurrectTask;
    unsigned int							m_nFormScaleUnit;

    QMutex									sMutex;
    QMutex									sMutex2;
    QMutex									sMutex3;
    QMutex									sMutex4;
    QMutex									sMutex5;
    QMutex									sMutex6;
    QMutex									sMutex7;
    QMutex									sMutex8;

    QMutex									sRsMutex1;
    QMutex									sRsMutex2;
    QMutex									sRsMutex3;

    QMutex									sRsMutex4;
    QMutex									sRsMutex5;
    QMutex									sRsMutex6;

    QMutex									sRsMutex7;
    QMutex									sReMutex8;
    QMutex									sReMutex9;

    QMutex									sFsMutex1;
    QMutex									sFsMutex2;
    QMutex									sFsMutex3;

    QMutex                                  sCsMutex1;

    QMutex                                  sReMissionMark;
    QMutex                                  sReMissionPT;

    QMutex									sRePlatMatch;

    QMutex									MutexMessageRecv;

    QMutex									sMutexMissionAmmo;

    QMutex									sMutexMissionRelation_PlatRoute;

    QMutex									sMutexCarryPlan;

    QMutex									sMutexOperationalInformation;

    QSqlDatabase							db;

    CNeubulaMissionVec						m_NeubulaMissionVec;
    CNeubulaMissionVec						m_NeubulaActivatingMissionVec;
    CNebulaMissionPlatformVec				m_MissionPlatformVec;
    CNebulaMissionPlatformLoadVec			m_MissionPlatformLoadVec;
    CNeubulaActionVec						m_NeubulaActionVec;
    CNeubulaMissionLineVec					m_NeubulaMissionLineVec;

    CRelationActionMarkVec				m_RelationActionMarkVec;
    CRelationMissionMarkVec				m_RelationMissionMarkVec;
    CRelationMissionResourceVec             m_RelationMissionPtVec;

    CNebulaPodNatureVec						m_NebulaPodNatureVec;
    CNebulaAmmoNatureVec					m_NebulaAmmoNatureVec;
    CNebulaPlatNatureVec					m_NebulaPlatNatureVec;

    CRelationActionTaskVec					m_RelationActionTaskVec;
    CNebulaAcitonRelationVec				m_AcitonRelationShowVec;

    CCodeSet_Load_DB_Vec					m_CodeSet_LoadVec;
    CCodeSet_Load_DB_Vec					m_CodeSet_LoadVec_Activating;

    NebulaResourcesVec						m_NebulaResourcesSensorVec;
    NebulaResourcesVec						m_NebulaResourcesPtVec;
    NebulaResourcesVec						m_NebulaResourcesAmmoVec;

    CRelationActionResourceVec              m_RelationActionPtVec;
    CRelationActionResourceVec              m_RelationActionSensorVec;
    CRelationActionResourceVec              m_RelationActionAmmoVec;

    CActionMarkRankVec                    m_ActionMarkRankVec;

    CNebulaSchemeVec							m_NebulaSchemeVec;
    CNebulaSchemePointVec					    m_NebulaSchemePointVec;
    CNebulaSchemePointFAVec					    m_NebulaSchemePointFAVec;
    CNebulaSchemeSensorVec					    m_NebulaSchemeSensorVec;
    CNebulaSchemeAmmoVec						m_NebulaSchemeAmmoVec;
    CNebulaActionSchemeGLVec					m_NebulaActionSchemeGLVec;

    CRelationMissionAmmoVec                     m_NebulaMissionAmmoVec;

    CNebulaMissionRelation_PlatRouteVec         m_NebulaMissionRelation_PlatRouteVec;

    CResourceCarryingVec						m_CResourceCarryingVec;

    CNeubulaOperationalInformationVec						m_COperationalInformationVec;

    int										m_ActivatedActionID = -1;

    std::string loginName;

private:
    bool ParseNebulaRouteGroupNode(TiXmlElement *pGroupNodeEle, CNebulaTask &nebulaTask, CNebulaRouteGroup &NebulaGroupInfo);
    bool ParseNebulaTaskPlatformListNode(TiXmlElement *pXMLNebulaTaskPlatformList, CNebulaTask &nebulaTask);
    bool ParseNebulaTaskPlatformNode(TiXmlElement *pXMLNebulaTaskPlatform, NebulaTaskPlatform &nebulaTaskPlatform);
    bool ParseNebulaTaskPlatformTaskLoadListNode(TiXmlElement *pXMLNebulaTaskPlatformTaskLoadList, NebulaTaskLoadList &nebulaTaskLoadList);
    bool ParseNebulaTaskPlatformMarkListNode(TiXmlElement *pXMLNebulaTaskMarkList, NebulaMarkTrackList &taskMarkList);
    bool ParseNebulaTaskPlatformRegionListNode(TiXmlElement *pXMLNebulaTaskRegionList, std::list<std::string> &taskRegionList);
    bool ParseNebulaMarkInfoListNode(TiXmlElement *pXMLNebulaMarkInfoList, CNebulaTask &nebulaTask);
    bool ParseNebulaRegionInfoListNode(TiXmlElement *pXMLNebulaRegionInfoList, CNebulaTask &nebulaTask);
    bool ParseNebulaRouteLineNode(TiXmlElement *pLineNodeEle, CNebulaRouteLine &NebulaLineInfo);
    bool ParseNebulaRoutePointNode(TiXmlElement *pPointNodeEle, CNebulaRoutePoint &NebulaPointInfo);

    bool BuildNebulaRouteGroupNode(CNebulaTask &nebulaTask, CNebulaRouteGroup &NebulaGroupInfo, TiXmlElement *pGroupNodeEle);
    bool BuildNebulaTaskPlatformListNode(CNebulaTask &nebulaTask, CNebulaRouteGroup &NebulaGroupInfo, TiXmlElement *pXMLNebulaTaskPlatformList);
    bool BuildNebulaTaskPlatformNode(NebulaTaskPlatform &nebulaTaskPlatform, TiXmlElement *pXMLNebulaTaskPlatform);
    bool BuildNebulaTaskPlatformTaskLoadListNode(NebulaTaskLoadList &nebulaTaskLoadList, TiXmlElement *pXMLNebulaTaskPlatformTaskLoadList);
    bool BuildNebulaTaskPlatformMarkListNode(NebulaMarkTrackList &taskMarkList, TiXmlElement *pXMLNebulaTaskMarkList);
    bool BuildNebulaTaskPlatformRegionListNode(std::list<std::string> &taskRegionList, TiXmlElement *pXMLNebulaTaskRegionList);
    bool BuildNebulaMarkInfoListNode(CNebulaTask &nebulaTask, TiXmlElement *pXMLNebulaMarkInfoList);
    bool BuildNebulaRegionInfoListNode(CNebulaTask &nebulaTask, TiXmlElement *pXMLNebulaRegionInfoList);
    bool BuildNebulaRouteLineNode(CNebulaRouteLine &NebulaLineInfo, TiXmlElement *pLineNodeEle);
    bool BuildNebulaRoutePointNode(const CNebulaRoutePoint &NebulaLineInfo, TiXmlElement *pPointNodeEle);

    CNebulaTask *GetNebulaTask(unsigned int nTaskID);
    CNebulaTask *GetNebulaTask(const char *nTaskName);

    void InitDataBase();

    void LoadDataBase();
    bool EditDBData(CNeubulaMission &data);
    bool EditDBData(CNeubulaMission &data, std::string &strOldID);
    bool AddDBData(CNeubulaMission &data);
    bool RemoveDBData(std::string strID);
    bool IsIDExisted(std::string strID);
    void UpdataMissionMarkList();
    void UpdataMissionMarkDamage();
    void UpdataMissionAmmoVec();

    void LoadPlatformDB();
    bool EditPlatformDBData(CNebulaMissionPlatform &data);
    bool AddPlatformDBData(CNebulaMissionPlatform &data);
    bool RemovePlatformDBData(std::string strID);
    bool IsPlatformIDExisted(std::string strID);

    void LoadPlatformLoadDB();
    bool EditPlatformLoadDBData(CNebulaMissionPlatformLoad &data);
    bool AddPlatformLoadDBData(CNebulaMissionPlatformLoad &data);
    bool RemovePlatformLoadDBData(std::string strID);
    bool IsPlatformLoadIDExisted(std::string strID);

    bool LoadPodNatureDB();
    bool LoadAmmoNatureDB();
    bool LoadPlatNatureDB();

    void LoadActivatedActionIDFormDB();
    void LoadActionformDB();
    bool EditActionDBData(CNeubulaAction &data);
    bool AddActionDBData(CNeubulaAction &data);
    bool RemoveActionDBData(int nID);
    bool IsActionIDExisted(int nID);
    bool SetActionActivateStatesDB(int nID);
    void UpdataAcitonMarkList();

    void LoadRelationMarkDB();
    bool EditRelationMarkDBData(CRelationActionMark &data);
    bool AddRelationMarkDBData(CRelationActionMark &data);
    bool RemoveRelationMarkDBData(CRelationActionMark &data);
    bool IsRelationMarkDBExisted(CRelationActionMark &data);
    int GetMaxRelationMarkID();

    void LoadRelationMissionMarkDB();
    bool EditRelationMissionMarkDBData(CRelationMissionMark &data);
    bool AddRelationMissionMarkDBData(CRelationMissionMark &data);
    bool RemoveRelationMissionMarkDBData(CRelationMissionMark &data);
    bool RemoveRelationMissionMarkDBData(const std::string &strMission);
    bool IsRelationMissionMarkDBExisted(CRelationMissionMark &data);
    int GetMaxRelationMissionMarkID();

    void LoadRelationTask();
    bool EditRelationTaskDBData(const CRelationActionTask data);
    bool EditRelationTaskDBData_LineID(const int nId, const std::string strLineID);
    bool AddRelationTaskDBData(const CRelationActionTask data);
    bool RemoveRelationTaskDBData(const int ID);
    bool IsRelationTaskDBExisted(const int ID);
    void UpdateRelationTaskShowData();
    void AddRelationTaskShowData_Action(const CNebulaAcitonRelation stAction);
    void AddRelationTaskShowData_Task(const int nActionID, const CNebulaAcitonRelation_Task stTask);
    void AddRelationTaskShowData_Platform(const int nActionID, std::string strTaskID, const CNebulaAcitonRelation_PlatForm stPlatForm);
    bool IsTaskInAction(const std::string strTaskID);

    void LoadCodeSet_Load_DB();
    bool EditLoadCodeSet_Load_DB(const  CCodeSet_Load_DB data);
    bool AddLoadCodeSet_Load_DB(const CCodeSet_Load_DB data);
    bool AddLoadCodeSet_Load_DB1(const CCodeSet_Load_DB data);
    bool RemoveCodeSet_Load_DB(const int nID);
    bool IsCodeSetIDExisted_Load_DB(const int strID);
    void UpdateCodeSet_Load_Activated();
    int GetMaxCodeSet_Load_ID_DB();

    void LoadLineDB();
    void LoadLineDBByTask(const std::string &taskID);
    bool AddlineDB(const CNeubulaMissionLine data);
    bool EditLineDB(const CNeubulaMissionLine data);
    bool RemovelineDB(const int nID);
    bool IsLineExistedDB(const int nID);
    bool SetLineShowStatesDB(const int nLineID, const bool bShow);

    bool AddLinePointDB(const CNeubulaMissionLinePoint data);
    bool AddLinePointsDB(const CNeubulaMissionLinePointVec data);
    bool EditLinePointDB(const CNeubulaMissionLinePoint data);
    bool InsertLinePointDB(const CNeubulaMissionLinePoint data);
    bool RemoveLinePointDB(const int nLineID, const int nPointID);
    bool RemoveLinePointDB(const int nLineID);
    bool IsLinePointExistedDB(const int nLineID, const int nPointID);

    void LoadResourceSensorDB();
    void LoadResourcePtDB();
    void LoadResourceAmmoDB();

    void LoadRelationMissionPtDB();
    void LoadRelationMissionPtDBre();
    void LoadRelationTaskPtDB();
    bool AddRelationMissionPtDBData(CRelationMissionResource &data);

    bool RemoveRelationMissionPtDBData(std::string strMissionID);
    int GetMaxRelationMissionPtID();
    void UpdataMissionPtList();

    void LoadRelationPtDB();
    void LoadRelationSensorDB();
    void LoadRelationAmmoDB();

    bool EditRelationPtDBData(CRelationActionResource &data);
    bool EditRelationSensorDBData(CRelationActionResource &data);
    bool EditRelationAmmoDBData(CRelationActionResource &data);

    bool AddRelationPtDBData(CRelationActionResource &data);
    bool AddRelationSensorDBData(CRelationActionResource &data);
    bool AddRelationAmmoDBData(CRelationActionResource &data);

    bool RemoveRelationPtDBData(CRelationActionResource &data);
    bool RemoveRelationSensorDBData(CRelationActionResource &data);
    bool RemoveRelationAmmoDBData(CRelationActionResource &data);

    bool IsRelationPtDBExisted(CRelationActionResource &data);
    bool IsRelationSensorDBExisted(CRelationActionResource &data);
    bool IsRelationAmmoDBExisted(CRelationActionResource &data);

    int GetMaxRelationPtID();
    int GetMaxRelationSensorID();
    int GetMaxRelationAmmoID();
    int GetMaxRelationTaskPtID();

    void UpdataAcitonPtList();
    void UpdataAcitonSensorList();
    void UpdataAcitonAmmoList();

    void LoadActionMarkRankListDB();

    bool IsMarkRankListDBExisted(CActionMarkRank &data);

    bool AddActionMarkRankListDB(CActionMarkRank &data);

    void LoadSchemeDB();
    void LoadSchemePointDB();
    void LoadSchemeAmmoDB();
    void LoadSchemeSensorDB();

    void LoadSchemePointFADB();

    void LoadActionSchemeGLDB();

    bool AddSchemeDB(NebulaScheme &data);
    bool AddSchemePointDB(NebulaSchemePoint &data);
    bool AddSchemeAmmoDB(NebulaSchemeAmmo &data);
    bool AddSchemeSensorDB(NebulaSchemeSensor &data);
    bool AddSchemePointFADB(NebulaSchemePointFA &data);
    bool AddActionSchemeGLDB(NebulaActionSchemeGL &data);

    bool EditSchemeDB(NebulaScheme &data);
    bool EditSchemePointDB(NebulaSchemePoint &data);
    bool EditSchemeAmmoDB(NebulaSchemeAmmo &data);
    bool EditSchemeSensorDB(NebulaSchemeSensor &data);
    bool EditSchemePointFADB(NebulaSchemePointFA &data);
    bool EditActionSchemeGLDB(NebulaActionSchemeGL &data);

    bool DeleteSchemeDB(NebulaScheme &data);
    bool DeleteSchemePointDB(NebulaSchemePoint &data);
    bool DeleteSchemeAmmoDB(NebulaSchemeAmmo &data);
    bool DeleteSchemeSensorDB(NebulaSchemeSensor &data);
    bool DeleteSchemePointFADB(NebulaSchemePointFA &data);
    bool DeleteActionSchemeGLDB(NebulaActionSchemeGL &data);

    bool IsSchemeDBExisted(NebulaScheme &scheme);
    bool IsSchemePointDBExisted(NebulaSchemePoint &point);
    bool IsSchemeAmmoDBExisted(NebulaSchemeAmmo &ammo);
    bool IsSchemeSensorDBExisted(NebulaSchemeSensor &sensor);
    bool IsSchemePointFADBExisted(NebulaSchemePointFA &data);
    bool IsActionSchemeGLDBExisted(NebulaActionSchemeGL &data);

    int GetMaxSchemeAmmoID();
    int GetMaxSchemeSensorID();

    void LoadMissionAmmoDB();

    bool AddMissionAmmoDB(const CRelationMissionAmmo &data);

    bool DeleteMissionAmmoDB(std::string nMissionID);

    bool IsMissionAmmoDBMissionIDDB(std::string nMissionID);

    void loadLoginName(const std::string &strLoginFile = "../data/Login.xml");

    int sendSystemUpdate(ESystemUpdateType systemUpdateType, ESystemOperatorType systemOperatorType, const char *objectID);

    void LoadMissionRelation_PlatRouteDB();

    bool AddMissionRelation_PlatRouteDB(const CNebulaMissionRelation_PlatRoute &data);

    bool DeleteMissionRelation_PlatRouteDB(std::string nMissionID);

    bool DeleteRowMissionRelation_PlatRouteDB(std::string sMissionID, int nRouteID);

    bool IsMissionRelation_PlatRouteDBMissionIDDB(std::string nMissionID);

    bool IsMissionPouteRelation_PlatRouteDBMissionIDDB(std::string sMissionID, int nRouteID);

    void LoadResourceCarryingPlanDB();

    bool AddResourceCarryingDB(const CResourceCarrying &data);

    bool DeleteResourceCarryingDB();

    bool IsResourceCarryingDB(std::string nResName);

    int GetCarryingPlanMaxBHDB();

    void LoadOperationalInformationDB();

    bool AddOperationalInformationDB(const CNeubulaOperationalInformation &data);

    bool DeleteOperationalInformationDB(int ntype, std::string sTaskID);

    bool DeleteOperationalInformationDB(int ntype, std::string sTaskID, std::string stargetID);

    bool IsOperationalInformationDB(int ntype, std::string sTemp);

    bool IsOperationalInformationDB(int ntype, std::string sTemp, std::string stargetID);

public:

    virtual NebulaTaskFileNameList GetAllNebulaTaskFileName();

    virtual EOperatorNebulaTask OpenNebulaTaskFile(const char *strFileName);

    virtual EOperatorNebulaTask SaveNebulaTaskFile(const char *strFileName);

    virtual EOperatorNebulaTask RemoveNebulaTaskFile(const char *strFileName);

    virtual EOperatorNebulaTask IsExistNebulaTaskFile(const char *strFileName);

    virtual bool GetNebulaTaskIDList(CNebulaTask::LPNebulaTaskIDList TaskIdList);

    virtual void ClearNebulaTaskList();

    virtual bool GetNebulaTask(unsigned int nNebulaTaskId, LPNebulaTask);

    virtual bool SetCurrentNebulaTaskID(unsigned int nNebulaTask);

    virtual bool GetCurrentNebulaTaskID(unsigned int &nNebulaTask);

    virtual bool GetCurrentNebulaLastPoint(const char *pWeaponId, LPNebulaRoutePoint pLastPoint);

    virtual bool GetCurrentNebulaLine(const char *pWeaponId, LPNebulaRouteLine pLastPoint);

    virtual bool SetNebulaTask(const CNebulaTask &data);

    virtual bool AddNebulaTask(const CNebulaTask &data);

    virtual bool RemoveNebulaTask(unsigned int nNebulaTaskId);

    virtual unsigned  int CreateNebulaTaskID();

    virtual bool SetNebulaTaskName(unsigned int nTaskId, const char *strTaskName);

    virtual bool GetNebulaTaskName(unsigned int nTaskId, std::string &strTaskName);

    virtual bool GetNebulaTaskID(const char *strTaskName, unsigned int &nTaskId);

    virtual bool IsNebulaExistTask(unsigned int nTaskId);

    virtual bool IsExistNebulaTask(const char *strTaskId);

    virtual bool GetNebulaRouteGroupIDList(unsigned int nTaskId, CNebulaRouteGroup::LPRouteGroupIDList);

    virtual bool GetNebulaRouteGroupNum(unsigned int nTaskId, int &nGroupNum);

    virtual bool GetNebulaRouteGroup(unsigned int nTaskId, unsigned int nGroupId, LPNebulaRouteGroup lpGroup);

    virtual bool AddNebulaRouteGroup(unsigned int nTaskId, const CNebulaRouteGroup &);

    virtual unsigned  int CreateNebulaGroupID(unsigned int nTaskId);

    virtual bool SetNebulaRouteGroup(unsigned int nTaskId, const CNebulaRouteGroup &);

    virtual bool IsExistNebulaRouteGroup(unsigned int nTaskId, unsigned int nGroupId);

    virtual bool GetNebulaRouteGroupName(unsigned int nTaskId, unsigned int nGroupId, std::string &strGroupName);

    virtual bool SetNebulaRouteGroupName(unsigned int nTaskId, unsigned int nGroupId, const char *strGroupName);

    virtual bool GetNebulaRouteGroupID(unsigned int nTaskId, const char *strGroupName, unsigned int &nGroupId);

    virtual bool RemoveNebulaRouteGroup(unsigned int nTaskId, unsigned int nGroupId);

    virtual bool GetNebulaRouteGroupFormMode(unsigned int nTaskId, unsigned int nGroupId, CNebulaRouteGroup::EFormMode &eMode, unsigned int &FormId);

    virtual bool SetNebulaRouteGroupFormMode(unsigned int nTaskId, unsigned int nGroupId, CNebulaRouteGroup::EFormMode eMode, unsigned int FormId);

    virtual bool GetNebulaWeaponIDList(unsigned int nTaskId, unsigned int nGroupId, LPStringVector);

    virtual bool GetNebulaRouteLineIDList(unsigned int nTaskId, unsigned int nGroupId, CNebulaRouteLine::LPRouteLineIDList);

    virtual bool GetNebulaRouteLineNum(unsigned int nTaskId, unsigned int nGroupId, int &nLineNum);

    virtual bool IsExistNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, unsigned int nRouteLine);

    virtual bool GetNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, LPNebulaRouteLine);

    virtual bool RemoveNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId);

    virtual bool SetNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, const CNebulaRouteLine &data);

    virtual bool AddNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, const CNebulaRouteLine &data);

    virtual bool GetNebulaRouteLineTail(unsigned int nTaskId, unsigned int nGroupId, LPNebulaRouteLine data);

    virtual bool GetNebulaRouteLineHead(unsigned int nTaskId, unsigned int nGroupId, LPNebulaRouteLine data);

    virtual bool SwapNebulaRouteLinePosition(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineID_1, unsigned int nLineID_2);

    virtual bool GetNebulaWeaponID(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, std::string &strWeaponId);

    virtual void SetFormUnitScaleLen(unsigned int nUnit);

    virtual	int GetFormUnitScaleLen();

    virtual bool GetNebulaRoutePointIDList(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, CNebulaRoutePoint::LPRoutePointIDList);

    virtual bool GetNebulaRoutePointNum(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, int &nPointNum);

    virtual bool IsExistNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId);

    virtual bool GetNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId, LPNebulaRoutePoint);

    virtual bool RemoveNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId);

    virtual bool RemoveNebulaAllRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId);

    virtual bool SetNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId, const CNebulaRoutePoint &data);

    virtual bool AddNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, const CNebulaRoutePoint &data);

    virtual bool InsertNeublaRoutrPoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId, const CNebulaRoutePoint &data);

    virtual bool GetNebulaRoutePointTail(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, LPNebulaRoutePoint data);

    virtual bool GetNebulaRoutePointHead(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, LPNebulaRoutePoint data);

    virtual CNeubulaMissionVec GetAllMission();

    virtual CNeubulaMissionVec GetActivatingMissions();

    virtual std::vector<std::string> GetMissionsByAction(int nActionID);

    virtual CNeubulaMission GetMission(const std::string &strID);

    virtual CNeubulaMissionVec GetMissionByModel(std::string &strID);

    virtual CNeubulaMissionVec GetMissionByLoadModel(std::string &strID);

    virtual bool SetMission(CNeubulaMission &data);
    virtual bool SetMission(CNeubulaMission &data, std::string strOldID);

    virtual bool RmoveMission(const std::string &strID);

    virtual bool RmoveAllMission();

    virtual bool IsExisted(const std::string &strID);

    virtual bool LoadData();

    virtual StringVector GetTaskLineByFileName(std::string &strID);

    virtual CNebulaRouteLine GetTaskLinePointByFileName(std::string &strID, std::string strUAVID);

    virtual CNebulaMissionPlatformVec GetAllMissionPlatform();

    virtual CNebulaMissionPlatform GetMissionPlatform(const std::string &strID);

    virtual CNebulaMissionPlatformVec GetMissionPlatformByMode(std::string &strID);

    virtual CNebulaMissionPlatformVec GetMissionPlatformByMissionId(std::string &strID);

    virtual bool SetMissionPlatform(CNebulaMissionPlatform &data);

    virtual bool RmoveMissionPlatform(const std::string &strID);

    virtual bool RmoveAllMissionPlatform();

    virtual bool IsMissionPlatformExisted(std::string &strID);

    virtual bool LoadMissionPlatformData();

    virtual CNebulaMissionPlatformLoadVec GetAllMissionPlatformLoad();

    virtual CNebulaMissionPlatformLoad GetMissionPlatformLoad(const std::string &strID);

    virtual bool GetMissionPlatformLoad(const QString &strMode, CNebulaMissionPlatformLoadVec &loadVec);

    virtual bool SetMissionPlatformLoad(CNebulaMissionPlatformLoad &data);

    virtual bool RmoveMissionPlatformLoad(const std::string &strID);

    virtual bool RmoveAllMissionPlatformLoad();

    virtual bool IsMissionPlatformLoadExisted(const std::string &strID);

    virtual bool LoadMissionPlatformLoadData();

    virtual CNeubulaActionVec GetAllNebulaAction();

    virtual CNeubulaAction GetNebulaAction(int &nID);

    virtual bool SetNebulaAction(CNeubulaAction &data);

    virtual bool RmoveNebulaAction(int &nID);

    virtual bool RmoveAllNebulaAction();

    virtual bool IsNebulaActionExisted(int &nID);

    virtual bool SetActivatingActionID(int &nID);

    virtual int GetActivatingActionID();

    virtual int GetActivatingActionIDFormDB();

    virtual int GetSelectedActivatingActionIDLocal();

    virtual void ReLoadRelationMarkDB();

    virtual CRelationActionMarkVec GetAllRelationMark();

    virtual CRelationActionMarkVec GetRelationMark(const int &nActionID);

    virtual bool RmoveRelationMark(std::vector<CRelationActionMark> &data);

    virtual bool RmoveRelationMark(CRelationActionMark &data);

    virtual bool AddRelationMark(std::vector< std::string > strMark);

    virtual bool AddRelationMark(std::string strMark);

    virtual bool IsRelationMarkExisted(CRelationActionMark &data);

    virtual void ReLoadRelationMissionMarkDB();

    virtual CRelationMissionMarkVec GetAllRelationMissionMark();

    virtual CRelationMissionMarkVec GetRelationMissionMark(const std::string &strID);

    virtual bool RmoveRelationMissionMark(const CRelationMissionMark &data);
    virtual bool RmoveRelationMissionMark(const std::string &strMissionID);

    virtual bool AddRelationMissionMark(const CRelationMissionMark &data);

    virtual bool IsRelationMissionMarkExisted(const CRelationMissionMark &data);

    virtual CRelationActionTaskVec GetAllRelationTask();

    virtual CRelationActionTaskVec GetAllRelationTaskByAction(const int nActionID);

    virtual	int GetRelationTaskMaxID();

    virtual bool AddRelationTask(const CRelationActionTask stRelation);

    virtual bool EditRelationTask(const CRelationActionTask stRelation);

    virtual bool RmoveRelationTask(const int nID);

    virtual bool RmoveRelationTaskLine(const int nActionID, const std::string strtaskID, const std::string strLineID, const std::string strNewLineID);

    virtual bool IsRelationTaskExisted(const int nID);

    virtual CNebulaAcitonRelationVec GetActivatingShowData();

    virtual CCodeSet_Load_DB_Vec GetAllCodeSet_Load();

    virtual CCodeSet_Load_DB GetCodeSet_Load(const int nID);
    virtual CCodeSet_Load_DB_Vec GetCodeSet_Load(const std::string strTaskID, const std::string strPlatformID);
    virtual CCodeSet_Load_DB_Vec GetCodeSet_Load(const std::string strTaskID, const std::string strPlatformID, const int nPointID);
    virtual CCodeSet_Load_DB_Vec GetCodeSet_Load(const std::string strLineID);

    virtual bool RmoveCodeSet_Load(const int nID);
    virtual bool RmoveCodeSet_Load(const std::string strTaskID, const std::string strPlatformID, const std::string strLoad, const int nPoint);

    virtual bool RmoveCodeSet_Load(const std::string strTaskID, const std::string strPlatformID, const int nPoint);
    virtual bool RmoveCodeSet_Load(const std::string strTaskID, const std::string strPlatformID, const int nPoint, const int Order);

    virtual bool SetCodeSet_Load(const CCodeSet_Load_DB data);
    virtual bool SetCodeSet_Load1(const CCodeSet_Load_DB data);

    virtual bool IsCodeSet_LoadExisted(const int nID);

    virtual int GetCodeSet_Load_MaxID();

    virtual void LoadMissionLine();

    virtual CNeubulaMissionLineVec GetAllMissionLine();

    virtual CNeubulaMissionLineVec GetMissionLinesByTask(const std::string &taskID);

    virtual int GetMissionLineMaxID();

    virtual int GetMissionLinePointMaxID();

    virtual int GetMissionLinePointMaxID(int nLineID);

    virtual CNeubulaMissionLinePointVec getMissionLinePointsFornLineID(const int nLineID);

    virtual CNeubulaMissionLine GetMissionLine(const int nLineID);
    virtual int GetMissionLineID(const std::string strTaskID, const std::string strPlatform);

    virtual bool AddMissionLine(const CNeubulaMissionLine stLine);

    virtual bool EditMissionLine(const CNeubulaMissionLine stLine);

    virtual bool RemoveMissionLine(const int nID);

    virtual bool IsMissionLineExited(const int nID);

    virtual CNeubulaMissionLinePointVec GetMissionLinePoints(const int nID);
    virtual CNeubulaMissionLinePoint GetMissionLinePoint(const int nID, const int nPoint);

    virtual bool AddMissionLinePoint(const CNeubulaMissionLinePoint stPoint);

    virtual bool RemoveMissionLinePoints(const int nLineID);
    virtual bool RemoveMissionLinePoint(const int nLineID, const int nPoint);

    virtual bool InsertMissionLinePoint(const int nNo, const CNeubulaMissionLinePoint stPoint);

    virtual bool IsMissionLinePointExited(const int nLineID, const int nPoint);

    virtual void SetMissionLiseShowStates(const int nLineID, const bool bShow);

    virtual void updatePointsIndexFormNow(const int nowIndex, const int nLineID, const int disNum);

    virtual NebulaResourcesVec GetSensorResourceFmDB();

    virtual NebulaResourcesVec GetPlatFormResourceFmDB();

    virtual NebulaResourcesVec GetAmmoResourceFmDB();

    virtual CRelationActionResourceVec GetAllRelationPt();
    virtual CRelationActionResourceVec GetAllRelationSensor();
    virtual CRelationActionResourceVec GetAllRelationAmmo();

    virtual CRelationActionResourceVec GetRelationPtResource(const int &nActionID);
    virtual CRelationActionResourceVec GetRelationSensorResource(const int &nActionID);
    virtual CRelationActionResourceVec GetRelationAmmoResource(const int &nActionID);

    virtual bool RmoveRelationPt(CRelationActionResource &data);
    virtual bool RmoveRelationSensor(CRelationActionResource &data);
    virtual bool RmoveRelationAmmo(CRelationActionResource &data);

    virtual bool AddRelationPt(CRelationActionResource &data);
    virtual bool AddRelationSensor(CRelationActionResource &data);
    virtual bool AddRelationAmmo(CRelationActionResource &data);

    virtual bool EditRelationPt(const CRelationActionResource &data);
    virtual bool EditRelationSensor(const CRelationActionResource &data);
    virtual bool EditRelationAmmo(const CRelationActionResource &data);

    virtual	int GetRelationPtMaxID();
    virtual	int GetRelationSensorMaxID();
    virtual	int GetRelationAmmoMaxID();
    virtual int GetRelationTaskPtMaxID();

    virtual CActionMarkRankVec GetActionMarkRankList(const int &nActionID);

    virtual	int GetActionMarkRankListMaxID();

    virtual bool AddActionMarkRankList(CActionMarkRank &data);

    virtual void LoadRelationMissionPlatDB();

    virtual CRelationMissionResourceVec GetAllRelationMissionPt();

    virtual CRelationMissionResourceVec GetRelationMissionPtResource(const std::string &strMissionID);

    virtual bool RmoveMissionRelationPt(const std::string &strMissionID);

    virtual bool AddMissionRelationPt(CRelationMissionResource &data);

    virtual	int GetRelationMissionPtMaxID();

    virtual bool IsRelationMissionPtExisted(const CRelationMissionResource &data);

    virtual bool AddPlatFormMatch(std::string PlatTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName);

    virtual std::vector<std::string> readPlatFormMatch(std::string PlatTypeName, std::string MissionID);

    virtual std::vector<std::string> readPlatCtrlByMissionIDPlatName(std::string PlatTypeName, std::string MissionID);

    virtual std::vector<std::string> readPlatFormMatch(std::string MissionID);

    virtual std::string readPlatFormMatchZBXH(std::string WeaponID, std::string MissionID);

    virtual std::string readPlatFormMatchKZRY(std::string WeaponID, std::string MissionID);

    virtual bool isPlatFormMatchExisted(std::string PlatTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName);

    virtual bool delPlatFormMatch(std::string PlatTypeName, std::string MissionID);

    virtual std::vector<std::string> getRouteLineNames();

    virtual std::vector<QVector3D> getRouteLinePointsForName(std::string routeName);

    virtual std::map<std::string, int> GetUserNameAndRole();

    virtual CNebulaSchemeVec GetAllScheme();
    virtual CNebulaSchemePointVec GetAllSchemePoint();
    virtual CNebulaSchemeAmmoVec GetAllSchemeAmmo();
    virtual CNebulaSchemeSensorVec GetAllSchemeSensor();
    virtual CNebulaSchemePointFAVec GetAllSchemePointFA();
    virtual CNebulaActionSchemeGLVec GetAllActionSchemeGL();

    virtual NebulaScheme GetSchemeByID(const std::string strSchemeID);
    virtual CNebulaSchemePointVec GetPointsByPointID(const std::string strPointID);
    virtual CNebulaSchemeAmmoVec GetAmmoBySchemeID(const std::string strSchemeID);
    virtual CNebulaSchemeSensorVec GetSensorBySchemeID(const std::string strSchemeID);

    virtual CNebulaSchemePointFAVec GetPointsByFAPointID(const std::string strSchemeID);

    virtual CNebulaSchemeVec GetSchemeByActionID(const int strActionID);

    virtual CNebulaSchemeVec GetSchemeByMarkID(const std::string strMarkID);

    virtual bool RmoveSchemeByID(NebulaScheme &data);
    virtual bool RmovePointsByPointID(NebulaSchemePoint &data);
    virtual bool RmoveSchemeGLPointsByData(NebulaSchemePointFA &data);
    virtual bool RmoveActionGLSchemeByData(NebulaActionSchemeGL &data);
    virtual bool RmoveAmmoBySchemeID(NebulaSchemeAmmo &data);
    virtual bool RmoveSensorBySchemeID(NebulaSchemeSensor &data);

    virtual bool AddScheme(NebulaScheme &data);
    virtual bool AddSchemePoint(NebulaSchemePoint &data);
    virtual bool AddSchemeFAPoint(NebulaSchemePointFA &data);
    virtual bool AddActionSchemeGL(NebulaActionSchemeGL &data);
    virtual bool AddAmmoBySchemeID(NebulaSchemeAmmo &data);
    virtual bool AddSensorBySchemeID(NebulaSchemeSensor &data);

    virtual bool EditScheme(const NebulaScheme &data);
    virtual bool EditSchemePoint(const NebulaSchemePoint &data);
    virtual bool EditSchemeAmmo(const NebulaSchemeAmmo &data);
    virtual bool EditSchemeSensor(const NebulaSchemeSensor &data);

    virtual	int GetSchemeAmmoMaxID();
    virtual	int GetSchemeSensorMaxID();

    virtual int GetActiveSchemeMaxID();
    virtual int GetSchemeLocationMaxID();

    virtual void LoadMissionAmmo();

    virtual bool AddMissionAmmo(const CRelationMissionAmmo &data);

    virtual bool RemoveMissionIDAmmo(std::string sMissionID);

    virtual CRelationMissionAmmoVec GetAllissionAmmoVec();

    virtual CRelationMissionAmmoVec GetMissionIDByMissionAmmo(const std::string sMissionID);

    time_t Wdrecv_StringToTime_t(std::string strTime);

    virtual std::vector<MessageRecv> getMessageVecForActiveID(const int activeID, const char *recvTime = NULL);

    virtual bool setMessageToFalseByActionIDAndTime(const int actiiveID, const char *recvTime, const int sendID, const char *messageType);

    virtual std::vector<MessageRecv> getMessageVecForActiveID(const int activeID, const string recvTime = NULL);

    virtual bool setMessageToFalseByActionIDAndTime(const int actiiveID, const string recvTime, const int sendID, const string messageType);

    virtual bool saveMessageRecv(MessageRecv message);

    virtual std::string getLoginName();

    virtual CNebulaPlatNatureVec GetPlatNatureData();

    virtual CNebulaPlatNature GetPlatNatureInfoByPlatName(const std::string strPlatName);

    virtual CNebulaAmmoNatureVec GetAmmoNatureData();

    virtual CNebulaAmmoNature GetAmmoNatureInfoByAmmoName(const std::string strAmmoName);

    virtual CNebulaPodNatureVec GetPodNatureData();

    virtual CNebulaPodNature GetPodNatureInfoByPodName(const std::string strPodName);

    virtual void LoadMissionRelation_PlatRoute();

    virtual CNebulaMissionRelation_PlatRouteVec GetMissionRelation_PlatRouteData();

    virtual bool AddMissionRelation_PlatRoute(const CNebulaMissionRelation_PlatRoute &data);

    virtual bool RemoveMissionRelation_PlatRoute(std::string sMissionID);

    virtual bool RemoveRowMissionRelation_PlatRoute(std::string sMissionID, int nRouteID);

    virtual CNebulaMissionRelation_PlatRouteVec GetMissionIDByPlatRoute(const std::string sMissionID);

    virtual int GetRelation_PlatRouteNumber();

    virtual CResourceCarryingVec GetResourceCarryingPlanInfo();

    virtual bool AddCarryingResource(const CResourceCarrying &data);

    virtual bool DeleteCarryingResource();

    virtual int GetCarryingPlanMaxBH();

    virtual CNeubulaOperationalInformationVec GetOperationalInformation();

    virtual bool AddOperationalInformation(const CNeubulaOperationalInformation &data);

    virtual bool DeleteOperationalInformation(int ntype, std::string sMissionID);

    virtual bool DeleteOperationalInformation(int ntype, std::string sMissionID, std::string sMarkID);

};
}
}

#endif
