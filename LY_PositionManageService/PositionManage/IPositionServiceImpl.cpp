#include "IPositionServiceImpl.h"

#include <QDir>
#include <QFile>
#include <QTextCodec>
#include <QMessageBox>
#include <QDebug>

#define ID_POSITION_BEGIN    10001
#define ID_POSITION_END      90000

using namespace qnzkna::PositionManage;

IPositionServiceImpl::IPositionServiceImpl()
    : m_XmlReaderPositionManage()
    , m_strPositionManageXMLPath("")
    , m_mutex()
    , m_nMaxCapacity(255)
    , m_mapPositions()
{
    QMutexLocker locker(&m_mutex);
    m_mapPositions.clear();
    locker.unlock();
    m_strPositionManageXMLPath ="../data/PositionManageFile.xml";
    loadPositionManage(QString::fromStdString(m_strPositionManageXMLPath));
}

IPositionServiceImpl::~IPositionServiceImpl()
{
    QMutexLocker locker(&m_mutex);
    m_mapPositions.clear();
    locker.unlock();
}

bool IPositionServiceImpl::loadPositionManage(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(nullptr, QObject::tr("QXmlStream ProjectManageFile"),
                             QObject::tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    if (!readPositionManage(&file))
    {
        QMessageBox::warning(nullptr, QObject::tr("QXmlStream ProjectManage"),
                             QObject::tr("Parse error in file %1:\n\n%2")
                             .arg(QDir::toNativeSeparators(fileName),
                                  errorString(m_XmlReaderPositionManage)));
    }
    else
    {

    }

    file.close();
    return true;
}

bool IPositionServiceImpl::readPositionManage(QIODevice *device)
{
    m_XmlReaderPositionManage.setDevice(device);

    if (m_XmlReaderPositionManage.readNextStartElement() && m_XmlReaderPositionManage.isStartElement())
    {
        if (m_XmlReaderPositionManage.name() == ("PositionManage") && m_XmlReaderPositionManage.attributes().value(versionAttribute()) == QLatin1String("1.0"))
        {
            qDebug() << m_XmlReaderPositionManage.attributes().value("PositionCapacity") << m_XmlReaderPositionManage.attributes().value("version");

            m_nMaxCapacity = m_XmlReaderPositionManage.attributes().value(QLatin1String("PositionCapacity")).toUInt();
            readPositionManage();
        }
        else
        {
            m_XmlReaderPositionManage.raiseError(QObject::tr("The file is not an xml version 1.0 file."));
        }
    }

    return !m_XmlReaderPositionManage.error();
}

QString IPositionServiceImpl::errorString(const QXmlStreamReader &xmlReader) const
{
    return QObject::tr("%1\nLine %2, column %3").arg(xmlReader.errorString()).arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber());
}

void IPositionServiceImpl::readPositionManage()
{
    QMutexLocker locker(&m_mutex);
    m_mapPositions.clear();
    locker.unlock();

    while (m_XmlReaderPositionManage.readNextStartElement())
    {
        if (m_XmlReaderPositionManage.name() == QLatin1String("Position") && m_XmlReaderPositionManage.isStartElement())
        {
            qDebug() << m_XmlReaderPositionManage.attributes().value(QLatin1String("id")) << m_XmlReaderPositionManage.attributes().value(QLatin1String("type"))
                     << m_XmlReaderPositionManage.attributes().value(QLatin1String("name")) << m_XmlReaderPositionManage.attributes().value(QLatin1String("note"))
                     << m_XmlReaderPositionManage.attributes().value(QLatin1String("mark")) << m_XmlReaderPositionManage.attributes().value(QLatin1String("area"))
                     << m_XmlReaderPositionManage.attributes().value(QLatin1String("team")) << m_XmlReaderPositionManage.attributes().value(QLatin1String("dispose"));

            const QStringRef id       = m_XmlReaderPositionManage.attributes().value(QLatin1String("id"));
            const unsigned int type   = m_XmlReaderPositionManage.attributes().value(QLatin1String("type")).toUInt();
            const QStringRef name     = m_XmlReaderPositionManage.attributes().value(QLatin1String("name"));
            const QStringRef note     = m_XmlReaderPositionManage.attributes().value(QLatin1String("note"));
            const QStringRef mark     = m_XmlReaderPositionManage.attributes().value(QLatin1String("mark"));
            const double area         = m_XmlReaderPositionManage.attributes().value(QLatin1String("area")).toDouble();
            const unsigned int team   = m_XmlReaderPositionManage.attributes().value(QLatin1String("team")).toUInt();
            const bool dispose        = (m_XmlReaderPositionManage.attributes().value(QLatin1String("dispose")).toUInt() == 1);

            Position position;
            position.setPositionID(id.toString().toStdString());
            position.setType(type);
            position.setName(name.toString().toStdString());
            position.setNote(note.toString().toStdString());
            position.setMark(mark.toString().toStdString());
            position.setArea(area);
            position.setTeam(NodeManage::TeamPropertyFromNumber(team));
            position.SetActivity(dispose);

            readPosition(position);

            QMutexLocker locker(&m_mutex);
            m_mapPositions.insert(make_pair(position.getPositionID(), position));
            locker.unlock();
        }
        else
        {
            m_XmlReaderPositionManage.skipCurrentElement();
        }
    }
}

void IPositionServiceImpl::readPosition(Position &position)
{
    while (m_XmlReaderPositionManage.readNextStartElement())
    {
        if (m_XmlReaderPositionManage.name() == QLatin1String("PositionSeatSet") && m_XmlReaderPositionManage.isStartElement())
        {
            readSeat(position);
        }
        else if (m_XmlReaderPositionManage.name() == QLatin1String("SystemTypeSet") && m_XmlReaderPositionManage.isStartElement())
        {
            readWeaponType(position);
        }
        else
        {
            m_XmlReaderPositionManage.skipCurrentElement();
        }
    }
}

void IPositionServiceImpl::readSeat(Position &position)
{
    while (m_XmlReaderPositionManage.readNextStartElement())
    {
        if (m_XmlReaderPositionManage.name() == QLatin1String("Seat") && m_XmlReaderPositionManage.isStartElement())
        {
            qDebug() << m_XmlReaderPositionManage.attributes().value(QLatin1String("disposition")) << m_XmlReaderPositionManage.attributes().value(QLatin1String("longitude"))
                     << m_XmlReaderPositionManage.attributes().value(QLatin1String("latitude")) << m_XmlReaderPositionManage.attributes().value(QLatin1String("altitude"));

            const unsigned int disposition = m_XmlReaderPositionManage.attributes().value(QLatin1String("disposition")).toUInt();
            const double longitude         = m_XmlReaderPositionManage.attributes().value(QLatin1String("longitude")).toDouble();
            const double latitude          = m_XmlReaderPositionManage.attributes().value(QLatin1String("latitude")).toDouble();
            const double altitude          = m_XmlReaderPositionManage.attributes().value(QLatin1String("altitude")).toDouble();

            WeaponSeat weaponSeat;
            weaponSeat.setIndex(disposition);
            weaponSeat.setLongitude(longitude);
            weaponSeat.setLatitude(latitude);
            weaponSeat.setAltitude(altitude);

            readHideAngleSet(weaponSeat);

            position.SetWeaponSeat(weaponSeat);
        }
        else
        {
            m_XmlReaderPositionManage.skipCurrentElement();
        }
    }
}

void IPositionServiceImpl::readHideAngleSet(WeaponSeat &weaponSeat)
{
    while (m_XmlReaderPositionManage.readNextStartElement())
    {
        if (m_XmlReaderPositionManage.name() == QLatin1String("HideAngleSet") && m_XmlReaderPositionManage.isStartElement())
        {
            readHideAngle(weaponSeat);
        }
        else
        {
            m_XmlReaderPositionManage.skipCurrentElement();
        }
    }
}

void IPositionServiceImpl::readHideAngle(WeaponSeat &weaponSeat)
{
    while (m_XmlReaderPositionManage.readNextStartElement())
    {
        if (m_XmlReaderPositionManage.name() == QLatin1String("HideAngle") && m_XmlReaderPositionManage.isStartElement())
        {
            qDebug() << m_XmlReaderPositionManage.attributes().value(QLatin1String("orient")) << m_XmlReaderPositionManage.attributes().value(QLatin1String("pitch"));

            const double orient          = m_XmlReaderPositionManage.attributes().value(QLatin1String("orient")).toDouble();
            const double pitch           = m_XmlReaderPositionManage.attributes().value(QLatin1String("pitch")).toDouble();

            unsigned long long int size = weaponSeat.GetPtrHideAngleMap()->size();
            weaponSeat.GetPtrHideAngleMap()->insert(std::make_pair(size, HideAngle(orient, pitch)));

            m_XmlReaderPositionManage.skipCurrentElement();
        }
        else
        {
            m_XmlReaderPositionManage.skipCurrentElement();
        }
    }
}

void IPositionServiceImpl::readWeaponType(Position &position)
{
    while (m_XmlReaderPositionManage.readNextStartElement())
    {
        if (m_XmlReaderPositionManage.name() == QLatin1String("WeaponType") && m_XmlReaderPositionManage.isStartElement())
        {
            qDebug() << m_XmlReaderPositionManage.attributes().value(QLatin1String("systemtype")) << m_XmlReaderPositionManage.attributes().value(QLatin1String("systemmodel"));

            const unsigned int systemtype   = m_XmlReaderPositionManage.attributes().value(QLatin1String("systemtype")).toUInt();
            const unsigned int systemmodel   = m_XmlReaderPositionManage.attributes().value(QLatin1String("systemmodel")).toUInt();

            position.SetWeaponType(Position::SWeaponInfo(systemtype, systemmodel));

            m_XmlReaderPositionManage.skipCurrentElement();
        }
        else
        {
            m_XmlReaderPositionManage.skipCurrentElement();
        }
    }
}

Position *IPositionServiceImpl::GetPositionRef(const std::string &strPositionID)
{
    PositionMap::iterator itrMap = m_mapPositions.find(strPositionID);
    if (itrMap != m_mapPositions.end())
    {
        return &(itrMap->second);
    }
    return nullptr;
}

const Position *IPositionServiceImpl::GetPosition(const std::string &strPositionID) const
{
    PositionMap::const_iterator itrMap = m_mapPositions.find(strPositionID);
    if (itrMap != m_mapPositions.end())
    {
        return &(itrMap->second);
    }
    return nullptr;
}

bool IPositionServiceImpl::GetAllPositionInfos(PositionMap *pMap) const
{
    bool bRet = false;
    if (pMap)
    {
        QMutexLocker locker(&m_mutex);
        *pMap = m_mapPositions;
        bRet  = true;
    }
    return bRet;
}

bool IPositionServiceImpl::GetVisualAllPositionInfos(PositionMap *pMap) const
{
    if (!pMap)
    {
        return false;
    }

    pMap->clear();
    QMutexLocker locker(&m_mutex);
    PositionMap::const_iterator itrPositionMap = m_mapPositions.begin();
    for (; itrPositionMap != m_mapPositions.end(); itrPositionMap ++)
    {
        if (itrPositionMap->second.getPositionVisFlg())
        {
            pMap->insert(std::make_pair(itrPositionMap->first, itrPositionMap->second));
        }
    }
    return true;
}

int IPositionServiceImpl::GetPosition(Position &pos, const std::string &strPositionID) const
{
    int nRet = -1;
    QMutexLocker locker(&m_mutex);
    PositionMap::const_iterator itrMapPosition = m_mapPositions.find(strPositionID);
    if (itrMapPosition != m_mapPositions.end())
    {
        const Position *const pTemp = &(itrMapPosition->second);
        if (pTemp)
        {
            pos = *pTemp;
            nRet = 0;
        }
    }

    return nRet;
}

bool IPositionServiceImpl::GetPositionGeo(double &dLongitude, double &dLatitude, double &dAltitude, const std::string &strPositionId) const
{
    if ("" == strPositionId)
    {
        return false;
    }
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    PositionMap::const_iterator itrMap = m_mapPositions.find(strPositionId);
    if (itrMap != m_mapPositions.end())
    {
        const Position *pPosition = &(itrMap->second);
        if (pPosition && pPosition->GetLongitude(dLongitude) && pPosition->GetLatitude(dLatitude) && pPosition->GetAltitude(dAltitude))
        {
            bRet = true;
        }
    }

    return bRet;
}

bool IPositionServiceImpl::SetPositionGeo(const std::string &strPositionId, double dLongitude, double dLatitude, double dAltitude)
{
    if ("" == strPositionId)
    {
        return false;
    }
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    PositionMap::iterator itrMap = m_mapPositions.find(strPositionId);
    if (itrMap != m_mapPositions.end())
    {
        Position *pPosition = &(itrMap->second);
        if (pPosition)
        {
            pPosition->SetLongitude(dLongitude);
            pPosition->SetLatitude(dLatitude);
            pPosition->SetAltitude(dAltitude);
            bRet = true;
        }
    }
    return bRet;
}

bool IPositionServiceImpl::GetWeaponSeatGeo(double &dLongitude, double &dLatitude, double &dAltitude, double &dGpsHeight, double &dAzimuth, double &dPitch, double &dRoll, const std::string &strPositionID, unsigned int nSeatIndex) const
{
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    const Position *pPosition = GetPosition(strPositionID);
    if (pPosition)
    {
        const WeaponSeat *pWeaponSeat = pPosition->GetWeaponSeat(nSeatIndex);
        if (pWeaponSeat)
        {
            dLongitude = pWeaponSeat->getLongitude();
            dLatitude  = pWeaponSeat->getLatitude();
            dAltitude  = pWeaponSeat->getAltitude();
            dGpsHeight = pWeaponSeat->getGpsHeight();
            dAzimuth   = pWeaponSeat->getAzimuth();
            dPitch     = pWeaponSeat->getPitch();
            dRoll      = pWeaponSeat->getRoll();
            bRet       = true;
        }
    }

    return bRet;
}

bool IPositionServiceImpl::SetWeaponSeatGeo(const std::string &strPositionID, unsigned int nSeatIndex, double dLongitude, double dLatitude, double dAltitude, double dGpsHeight, double dAzimuth, double dPitch, double dRoll)
{
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    Position *pPosition = GetPositionRef(strPositionID);
    if (pPosition)
    {
        WeaponSeat *pWeaponSeat = pPosition->GetWeaponSeatRef(nSeatIndex);
        if (pWeaponSeat)
        {
            pWeaponSeat->setLongitude(dLongitude);
            pWeaponSeat->setLatitude(dLatitude);
            pWeaponSeat->setAltitude(dAltitude);
            pWeaponSeat->setGpsHeight(dGpsHeight);
            pWeaponSeat->setAzimuth(dAzimuth);
            pWeaponSeat->setPitch(dPitch);
            pWeaponSeat->setRoll(dRoll);
            bRet = true;
        }
    }
    return bRet;
}

bool IPositionServiceImpl::GetPositionName(std::string &strPositionName, const std::string &strPositionId) const
{
    if ("" == strPositionId)
    {
        return false;
    }
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    PositionMap::const_iterator itrMap = m_mapPositions.find(strPositionId);
    if (itrMap != m_mapPositions.end())
    {
        const Position *pPosition = &(itrMap->second);
        if (pPosition)
        {
            strPositionName = pPosition->getName();
            bRet = true;
        }
    }

    return bRet;
}

int IPositionServiceImpl::SetPosition(const Position &position)
{
    int nRet = -1;
    QMutexLocker locker(&m_mutex);
    std::string strPositionID = position.getPositionID();
    PositionMap::iterator itrMapPos = m_mapPositions.find(strPositionID);
    if (itrMapPos != m_mapPositions.end())
    {
        Position *pPosData = &(itrMapPos->second);
        *pPosData = position;
        nRet = 0;
    }
    else
    {
        if (m_mapPositions.size() < m_nMaxCapacity)
        {
            m_mapPositions.insert(make_pair(position.getPositionID(), position));
            nRet = 0;
        }
        else
        {
            nRet = 1;
        }
    }
    locker.unlock();

    SendMsgPositionUpdate();

    return nRet;
}

bool IPositionServiceImpl::ModifyPosition(const std::string &strPositionID, double dLongitude, double dLantitude)
{
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    PositionMap::iterator itrMap = m_mapPositions.find(strPositionID);
    if (itrMap != m_mapPositions.end())
    {
        Position *pPosInfo = &(itrMap->second);
        pPosInfo->SetLongitude(dLongitude);
        pPosInfo->SetLatitude(dLantitude);
        bRet = true;
    }
    locker.unlock();

    SendMsgPositionUpdate();

    return bRet;
}

int IPositionServiceImpl::RemovePosition(const std::string &strPositionID)
{
    int nRet = -1;
    if ("" == strPositionID)
    {
        return nRet;
    }

    QMutexLocker locker(&m_mutex);
    PositionMap::iterator itrMapPos = m_mapPositions.find(strPositionID);
    if (itrMapPos != m_mapPositions.end())
    {
        m_mapPositions.erase(itrMapPos);
        nRet = 0;
    }
    locker.unlock();

    SendMsgPositionUpdate();
    return nRet;
}

int IPositionServiceImpl::GetWeaponSeat(WeaponSeat &weaponSeat, const std::string &strPositionID, const unsigned int nSeatIndex) const
{
    int nRet =  -1;
    QMutexLocker locker(&m_mutex);
    const Position *pos = GetPosition(strPositionID) ;
    if (nullptr != pos)
    {
        const WeaponSeatMap &pMap = pos->GetWeaponSeatMap();
        WeaponSeatMap::const_iterator itrMapSeat = pMap.find(nSeatIndex);
        if (itrMapSeat != pMap.end())
        {
            const WeaponSeat *pSeat = &(itrMapSeat->second);
            if (pSeat)
            {
                weaponSeat = *pSeat;
                nRet = 0;
            }
        }
    }

    return nRet;
}

bool IPositionServiceImpl::SetWeaponSeat(const WeaponSeat &pWeaponSeat, const std::string &strPositionID)
{
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    Position *pPosition = GetPositionRef(strPositionID);
    if (pPosition)
    {
        pPosition->SetWeaponSeat(pWeaponSeat);
        bRet = true;
    }
    locker.unlock();

    SendMsgPositionUpdate();
    return bRet;
}

bool IPositionServiceImpl::GetWeaponSeatGeo(double &dLongitude, double &dLatitude, double &dAltitude, const std::string &strPositionID, unsigned int nSeatIndex) const
{
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    const Position *pPosition = GetPosition(strPositionID);
    if (pPosition)
    {
        const WeaponSeat *pWeaponSeat = pPosition->GetWeaponSeat(nSeatIndex);
        if (pWeaponSeat)
        {
            dLongitude = pWeaponSeat->getLongitude();
            dLatitude  = pWeaponSeat->getLatitude();
            dAltitude  = pWeaponSeat->getAltitude();
            bRet = true;
        }
    }

    return bRet;
}

bool IPositionServiceImpl::SetWeaponSeatGeo(const std::string &strPositionID, unsigned int nSeatIndex, double dLongitude, double dLatitude, double dAltitude)
{
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    Position *pPosition = GetPositionRef(strPositionID);
    if (pPosition)
    {
        WeaponSeat *pWeaponSeat = pPosition->GetWeaponSeatRef(nSeatIndex);
        if (pWeaponSeat)
        {
            pWeaponSeat->setLongitude(dLongitude);
            pWeaponSeat->setLatitude(dLatitude);
            pWeaponSeat->setAltitude(dAltitude);
            bRet = true;
        }
    }

    return bRet;
}

bool IPositionServiceImpl::SetPositionVisual(const std::string &strID, bool bShow)
{
    bool bFlg = false;

    QMutexLocker locker(&m_mutex);
    PositionMap::iterator itrMap = m_mapPositions.find(strID);
    if (itrMap != m_mapPositions.end())
    {
        itrMap->second.setPositionVisFlg(bShow);
        bFlg = true;
    }
    locker.unlock();

    SendMsgPositionUpdate();
    return bFlg;
}

bool IPositionServiceImpl::GetPositionVisual(const std::string &strID, bool &bShow) const
{
    bool bFlg = false;
    QMutexLocker locker(&m_mutex);

    PositionMap::const_iterator itrMap = m_mapPositions.find(strID);
    if (itrMap != m_mapPositions.end())
    {
        bShow = itrMap->second.getPositionVisFlg();
        bFlg = true;
    }

    return bFlg;
}

void IPositionServiceImpl::SetAllPositionVisFlg(bool bShow)
{
    QMutexLocker locker(&m_mutex);
    PositionMap::iterator itrMap = m_mapPositions.begin();
    for (; itrMap != m_mapPositions.end(); itrMap++)
    {
        itrMap->second.setPositionVisFlg(bShow);
    }
}

void IPositionServiceImpl::SetTeamPositionVisFlg(qnzkna::NodeManage::TeamProperty teamProperty, bool bShow)
{
    QMutexLocker locker(&m_mutex);
    PositionMap::iterator itrMap = m_mapPositions.begin();
    for (; itrMap != m_mapPositions.end(); itrMap ++)
    {
        if (itrMap->second.getTeam() == teamProperty)
        {
            itrMap->second.setPositionVisFlg(bShow);
        }
    }
}

bool IPositionServiceImpl::IsExistPositionID(const std::string &strID) const
{
    bool bFlg = false;
    QMutexLocker locker(&m_mutex);

    PositionMap::const_iterator itrMap = m_mapPositions.find(strID);
    if (itrMap != m_mapPositions.end())
    {
        bFlg = true;
    }

    return bFlg;
}

bool IPositionServiceImpl::AddPosition(const Position &PositionInfo)
{
    bool bRet = false;
    QMutexLocker locker(&m_mutex);
    if ("" != PositionInfo.getPositionID())
    {
        if (m_mapPositions.size() < m_nMaxCapacity)
        {
            PositionMap::iterator itrMap = m_mapPositions.find(PositionInfo.getPositionID());
            if (itrMap == m_mapPositions.end())
            {
                m_mapPositions.insert(make_pair(PositionInfo.getPositionID(), PositionInfo));
                bRet = true;
            }
        }
    }
    locker.unlock();

    SendMsgPositionUpdate();
    return bRet;
}

bool IPositionServiceImpl::ChangePosition(const std::string &strID, const Position &positionInfo)
{
    bool bRet = false;

    QMutexLocker locker(&m_mutex);
    PositionMap::iterator iter = m_mapPositions.find(strID);
    if (iter != m_mapPositions.end())
    {
        iter->second = positionInfo;
        bRet = true;
    }
    locker.unlock();

    SendMsgPositionUpdate();
    return bRet;
}

bool IPositionServiceImpl::DelPosition(const std::string &strID)
{
    bool bRet = false;

    QMutexLocker locker(&m_mutex);
    PositionMap::iterator iter = m_mapPositions.find(strID);
    if (iter != m_mapPositions.end())
    {
        m_mapPositions.erase(iter);
        bRet = true;
    }
    locker.unlock();

    SendMsgPositionUpdate();
    return bRet;
}

int IPositionServiceImpl::SavePositionData()
{
    return 0;
}

void IPositionServiceImpl::DisableAllPositionActivity()
{
    QMutexLocker locker(&m_mutex);
    PositionMap::iterator itrMap = m_mapPositions.begin();
    for (; itrMap != m_mapPositions.end(); itrMap ++)
    {
        Position positionInfo = itrMap->second;
        positionInfo.SetActivity(false);
        WeaponSeatMap *pWeaponSeatMap = positionInfo.GetPtrWeaponSeatMap();
        if (pWeaponSeatMap)
        {
            WeaponSeatMap::iterator iterSet = pWeaponSeatMap->begin();
            for (; iterSet != pWeaponSeatMap->end(); iterSet++)
            {
                WeaponSeat weaponSet = iterSet->second;
                weaponSet.setActivity(false);
                positionInfo.SetWeaponSeat(weaponSet);
            }
        }

    }
}

std::string IPositionServiceImpl::CreatePositionID() const
{
    unsigned int nPositionID = ID_POSITION_BEGIN;
    std::string strID = std::to_string(nPositionID);
    QMutexLocker locker(&m_mutex);
    while (m_mapPositions.find(strID) != m_mapPositions.end())
    {
        nPositionID ++;
        strID = std::to_string(nPositionID);
        if (nPositionID > ID_POSITION_END)
        {
            break;
        }
    }

    return std::to_string(nPositionID++);
}

POSITION_NUM_UINT IPositionServiceImpl::GetMaxPositionNum() const
{
    return m_nMaxCapacity;
}

POSITION_NUM_UINT IPositionServiceImpl::GetPositionNum() const
{
    POSITION_NUM_UINT nNum;
    QMutexLocker locker(&m_mutex);
    nNum = m_mapPositions.size();
    return nNum;
}
