#ifndef IPOSITIONSERVICEIMPL_H
#define IPOSITIONSERVICEIMPL_H

#include <string>

#include <QMutex>
#include <QXmlStreamReader>

#include "DataManage/IPositionManageService.h"

namespace qnzkna
{
namespace PositionManage
{

class IPositionServiceImpl: public IPositionManageService
{
public:
    IPositionServiceImpl();

    virtual ~IPositionServiceImpl();

    std::string strFilePath() const;
    void setStrFilePath(const std::string &strFilePath);

    void SendMsgPositionUpdate()
    {
        do
        {

        }
        while (false);
    }

    virtual bool GetAllPositionInfos(PositionMap *pMap) const;
    virtual bool GetVisualAllPositionInfos(PositionMap *pMap) const;
    virtual int GetPosition(Position &pos, const std::string &strPositionID) const;
    virtual bool GetPositionGeo(double &dLongitude, double &dLatitude, double &dAltitude, const std::string &strPositionId) const;
    virtual bool SetPositionGeo(const std::string &strPositionId, double dLongitude, double dLatitude, double dAltitude);
    virtual bool GetWeaponSeatGeo(double &dLongitude, double &dLatitude, double &dAltitude, double &dGpsHeight, double &dAzimuth, double &dPitch, double &dRoll, const std::string &strPositionID, unsigned int nSeatIndex) const;
    virtual bool SetWeaponSeatGeo(const std::string &strPositionID, unsigned int nSeatIndex, double dLongitude, double dLatitude, double dAltitude, double dGpsHeight, double dAzimuth, double dPitch, double dRoll);
    virtual bool GetPositionName(std::string &strPositionName, const std::string &strPositionId) const;
    virtual int SetPosition(const Position &position);
    virtual bool ModifyPosition(const std::string &strPositionID, double dLongitude, double dLantitude);
    virtual int RemovePosition(const std::string &strPositionID);
    virtual int GetWeaponSeat(WeaponSeat &weaponSeat, const std::string &strPositionID, const unsigned int nSeatIndex) const ;
    virtual bool SetWeaponSeat(const WeaponSeat &pWeaponSeat, const std::string &strPositionID);
    virtual bool GetWeaponSeatGeo(double &dLongitude, double &dLatitude, double &dAltitude, const std::string &strPositionID, unsigned int nSeatIndex) const ;
    virtual bool SetWeaponSeatGeo(const std::string &strPositionID, unsigned int nSeatIndex, double dLongitude, double dLatitude, double dAltitude);
    virtual bool SetPositionVisual(const std::string &strID, bool bShow);
    virtual bool GetPositionVisual(const std::string &strID, bool &bShow) const ;
    virtual void SetAllPositionVisFlg(bool bShow);
    virtual void SetTeamPositionVisFlg(NodeManage::TeamProperty teamProperty, bool bShow);
    virtual bool IsExistPositionID(const std::string &strID) const ;
    virtual bool AddPosition(const Position &PositionInfo);
    virtual bool ChangePosition(const std::string &strID, const Position &positionInfo);
    virtual bool DelPosition(const std::string &strID);
    virtual int SavePositionData();
    virtual void DisableAllPositionActivity();
    virtual std::string CreatePositionID() const ;
    virtual POSITION_NUM_UINT GetMaxPositionNum() const ;
    virtual POSITION_NUM_UINT GetPositionNum() const ;

private:
    const Position *GetPosition(const std::string &strPositionID) const;
    Position *GetPositionRef(const std::string &strPositionID);

private:
    bool loadPositionManage(const QString &fileName);
    bool readPositionManage(QIODevice *device);

    QString errorString(const QXmlStreamReader &xmlReader) const;

    static inline QString versionAttribute()
    {
        return QStringLiteral("version");
    }
    static inline QString hrefAttribute()
    {
        return QStringLiteral("href");
    }

private:
    void readPositionManage();
    void readPosition(Position &position);
    void readSeat(Position &position);
    void readHideAngleSet(WeaponSeat &weaponSeat);
    void readHideAngle(WeaponSeat &weaponSeat);
    void readWeaponType(Position &position);

private:
    QXmlStreamReader                     m_XmlReaderPositionManage;
    std::string                          m_strPositionManageXMLPath;
    mutable QMutex                       m_mutex;
    unsigned int                         m_nMaxCapacity;
    PositionMap                          m_mapPositions;
};

}
}

#endif
