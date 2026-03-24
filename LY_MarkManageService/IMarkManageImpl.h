#ifndef IMARKMANAGEIMPL_H
#define IMARKMANAGEIMPL_H

#include <QMutex>
#include "DataManage/IMarkManageService.h"
#include "QSqlDatabase"
#include "QSqlQuery"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "Protocol/XygsSystemControl/SystemControl.h"

namespace qnzkna
{
namespace MarkManage
{
class IMarkManageImpl:
    public IMarkManageService
{
public:
    IMarkManageImpl();
    ~IMarkManageImpl();
private:
	mutable QMutex  			sMutex;
	mutable QMutex  			sMutexAirSapce;
	double						m_dTSLon = 0;
	double						m_dTSLat = 0;
	double						m_dTSAlt = 0;
	double						m_dTSTakeOffLon = 0;
	double						m_dTSTakeOffLat = 0;
	double						m_dTSTakeOffAlt = 0;
	std::string					loginName;
	int							m_nIconID;
	CSystemMarkInfoVec		m_MarkListVec;
	CSystemMarkInfoVec		m_MarkPosListVec;
	QSqlDatabase				db;
	CZoneInfoVec			m_ZoneInfoVec;
	QStringList					m_ThreatenSequenceIDList;

private:
    void InitDataBase();
    void LoadPosDataBase();

    bool EditDBData(SystemMarkInfo &data);
    bool EditDBData(SystemMarkInfo &data, std::string strOldID);
    bool AddDBData(SystemMarkInfo &data);
    bool RemoveDBData(std::string strID);
    bool IsIDExisted(std::string strID);
    int GenerateIconID();
    bool JudeActionMark(CRelationActionMarkVec &Rvec, const std::string &strID);

    bool EditMissionDBData(SystemMarkInfo &data);
    bool RemoveMissionDBData(std::string strID);
    bool IsMissionIDExisted(std::string strID);

    bool EditZoneDBData(CZoneInfo &data);
    bool EditZoneShowDatesDBData(CZoneInfo &data);
    bool AddZoneBData(CZoneInfo &data);
    bool RemoveZoneDBData(std::string strID);
    bool IsZoneIDExisted(std::string strID);
    bool AddZoneBData_Polygon(CZoneInfo &data);
    bool AddZoneBData_Ciecle(CZoneInfo &data);
    bool AddZoneBData_Sector(CZoneInfo &data);
    bool RemoveZoneDBData_Polygon(std::string strID);
    bool RemoveZoneDBData_Ciecle(std::string strID);
    bool RemoveZoneDBData_Sector(std::string strID);

    bool AddPosDBData(SystemMarkInfo &data);
    bool RemovePosDBData(std::string strID);

    bool WQPTLBisMBID1Existed(std::string strID);
    bool WQPTLBisMBID2Existed(std::string strID);
    bool WQPTLBsetNullForMarkID(std::string targetId);

    bool RWLBisMB1IDExisted(std::string strID);
    bool RWLBisMB2IDExisted(std::string strID);
    bool RWLBsetNullForMarkID(std::string strID);

    void loadLoginName(const std::string &strLoginFile = "../data/Login.xml");

    int sendSystemUpdate(ESystemUpdateType systemUpdateType, ESystemOperatorType systemOperatorType, const char *objectID);

public:
    bool ISExisted(std::string strID);
    void InitMapIcon();
    void LoadDBData();
    void LoadMissionTypeDBData();
    void LoadZoneDBData();
    void LoadZoneDBData_Polygon(CZoneInfo &data);
    void LoadZoneDBData_Ciecle(CZoneInfo &data);
    void LoadZoneDBData_Sector(CZoneInfo &data);
public:
    CSystemMarkInfoVec GetMarkList() const;
    CSystemMarkInfoVec GetMarkPosList() const;
    CSystemMarkInfoVec GetMarkPosList(const std::string &strI) const;
    SystemMarkInfo GetMark(const std::string &strID) const;
    std::vector<QString> GetMarkIDListFromDB() const;
    std::vector<QString> GetMarkIDListFromDB_ActivatingAction() const;
    SystemMarkInfo GetMarkFromDB(const QString &strID) const;
    bool SetTaregt(SystemMarkInfo &data);
    bool SetTaregt(SystemMarkInfo &data, std::string strOldID);
    bool SetMovingTaregt(SystemMarkInfo &data);
    bool UpdateTaregtState(SystemMarkInfo &data);
    bool RmoveTartget(std::string &strID);
    bool MergeMarkAToB(int ActionId, std::string targetAId, std::string targetBId);
    bool RmoveAllTartget();
    bool WakeUpMarkWindow(SystemMarkInfo &data);
    CZoneInfoVec GetZoneList() const;
    bool SetZone(CZoneInfo &data);
    bool RmoveZone(const std::string &strID);
    bool SetZoneShowStates(const std::string &strID, bool bShow);
    CZoneInfo GeAirApaceByID(const std::string &strID);
    CZoneInfoVec GeAirApaceByMarkID(std::string &strID);
    virtual bool SetMarkImage(const std::string &strID, const QString &strImgPath);
    virtual bool SetMarkImage(const std::string &strID, const QPixmap &InImg);
    virtual bool GetMarkImageNamesList(const std::string &strID, std::vector<std::string> *ImageList);
    virtual bool GetMarkImageData(const std::string &imageName, QPixmap *OutImage);
    virtual bool RemoveMarkImageData(const std::string &imageName);

    virtual bool SetMarkVideo(const std::string &strID, const QString &strVideoPath);
    virtual bool SetMarkVideo(const std::string &strID, const QByteArray &InVideo);
    virtual bool GetMarkVideoNamesList(const std::string &strID, std::vector<std::string> *VideoList);
    virtual bool GetMarkVideoData(const std::string &videoName, QByteArray *ImageData);

	virtual bool SetThreatenSequencePosition(const double& dLon, const double& dLat, const double& dAlt);
	virtual bool GetThreatenSequencePosition(double& dLon, double& dLat, double& dAlt);

	virtual bool SetThreatenSequenceTakeOffpoint(const double& dLon, const double& dLat, const double& dAlt);
	virtual bool GetThreatenSequenceTakeOffpoint(double& dLon, double& dLat, double& dAlt);

	virtual bool SetThreatenSequenceIDList(QStringList& strIDList);
	virtual bool GetThreatenSequenceIDList(QStringList& strIDList);
public:
    static IMarkManageImpl &Get();
};
}
}
#endif
