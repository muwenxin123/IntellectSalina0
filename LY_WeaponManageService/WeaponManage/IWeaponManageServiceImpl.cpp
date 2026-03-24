#include "IWeaponManageServiceImpl.h"

#include <QDir>
#include <QFile>
#include <QTextCodec>
#include <QMessageBox>
#include <QDebug>

using namespace qnzkna::WeaponManage;

IWeaponManageServiceImpl::IWeaponManageServiceImpl()
: m_XmlReaderWeaponManage()
, m_XmlReaderWeaponComparison()
, m_strWeaponManageXMLPath("")
, m_strComparisonXMLPath("")
, m_mutex()
, m_mapWeaponSystem()
, m_mapModelCategoryComparisonInfo()
, m_mapSystemCategoryComparisonInfo()
{
    QMutexLocker locker(&m_mutex);
    m_mapWeaponSystem.clear();
    m_mapModelCategoryComparisonInfo.clear();
    m_mapSystemCategoryComparisonInfo.clear();
    locker.unlock();
    m_strWeaponManageXMLPath = "../data/WeaponManageFile.xml";
    m_strComparisonXMLPath   = "../data/WeaponComponentComparisonFile.xml";
    loadWeaponManage(QString::fromStdString(m_strWeaponManageXMLPath));
    loadWeaponComparison(QString::fromStdString(m_strComparisonXMLPath));
}

bool IWeaponManageServiceImpl::loadWeaponManage( const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(nullptr, QObject::tr("QXmlStream ProjectManageFile"),
        QObject::tr("Cannot read file %1:\n%2.")
        .arg(QDir::toNativeSeparators(fileName),
        file.errorString()));
        return false;
    }

    if (!readWeaponManage(&file)) {
        QMessageBox::warning(nullptr, QObject::tr("QXmlStream ProjectManage"),
        QObject::tr("Parse error in file %1:\n\n%2")
        .arg(QDir::toNativeSeparators(fileName),
        errorString(m_XmlReaderWeaponManage)));
    } else {

    }

    file.close();
    return true;
}

bool IWeaponManageServiceImpl::readWeaponManage(QIODevice *device)
{
    m_XmlReaderWeaponManage.setDevice(device);

    if (m_XmlReaderWeaponManage.readNextStartElement() && m_XmlReaderWeaponManage.isStartElement()) {
        if (m_XmlReaderWeaponManage.name() == ("WeaponManage") && m_XmlReaderWeaponManage.attributes().value(versionAttribute()) == QLatin1String("1.0")) {
            readWeaponSystemList();
        } else {
            m_XmlReaderWeaponManage.raiseError(QObject::tr("The file is not an xml version 1.0 file."));
        }
    }

    return !m_XmlReaderWeaponManage.error();
}

QString IWeaponManageServiceImpl::errorString(const QXmlStreamReader& xmlReader) const
{
    return QObject::tr("%1\nLine %2, column %3").arg(xmlReader.errorString()).arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber());
}

void IWeaponManageServiceImpl::readWeaponSystemList(){
    while (m_XmlReaderWeaponManage.readNextStartElement()) {
        if (m_XmlReaderWeaponManage.name() == QLatin1String("WeaponSystemList") && m_XmlReaderWeaponManage.isStartElement()){
            readWeaponSystem();

            m_XmlReaderWeaponManage.skipCurrentElement();

            break;
        }
        else if (m_XmlReaderWeaponManage.name() == QLatin1String("WeaponSystemList1") && m_XmlReaderWeaponManage.isStartElement()){
            m_XmlReaderWeaponManage.skipCurrentElement();
        }
        else if (m_XmlReaderWeaponManage.name() == QLatin1String("WeaponSystemList2") && m_XmlReaderWeaponManage.isStartElement()){
            m_XmlReaderWeaponManage.skipCurrentElement();
        }
        else{
            m_XmlReaderWeaponManage.skipCurrentElement();
        }
    }
}

void IWeaponManageServiceImpl::readWeaponSystem(){
    while (m_XmlReaderWeaponManage.readNextStartElement()) {
        if (m_XmlReaderWeaponManage.name() == QLatin1String("WeaponSystem") && m_XmlReaderWeaponManage.isStartElement()){
            qDebug()<< m_XmlReaderWeaponManage.attributes().value( QLatin1String("type"))<< m_XmlReaderWeaponManage.attributes().value( QLatin1String("model"))
            << m_XmlReaderWeaponManage.attributes().value( QLatin1String("sysid"))<< m_XmlReaderWeaponManage.attributes().value( QLatin1String("name"))
            << m_XmlReaderWeaponManage.attributes().value( QLatin1String("remark")) ;

            const QStringRef sysid       = m_XmlReaderWeaponManage.attributes().value( QLatin1String("sysid"));
            const QStringRef name        = m_XmlReaderWeaponManage.attributes().value( QLatin1String("name"));
            const QStringRef remark      = m_XmlReaderWeaponManage.attributes().value( QLatin1String("remark"));
            const unsigned int sysType   = m_XmlReaderWeaponManage.attributes().value( QLatin1String("type")).toUInt(nullptr, 16);
            const unsigned int sysModel  = m_XmlReaderWeaponManage.attributes().value( QLatin1String("model")).toUInt(nullptr, 16);

            QMutexLocker locker(&m_mutex);
            auto iter = m_mapWeaponSystem.insert(std::pair<std::string, WeaponSystem>(sysid.toString().toStdString(), qnzkna::WeaponManage::WeaponSystem(WeaponSystemTypeFromNumber(sysType), WeaponSystemModelFromNumber(sysModel), sysid.toString().toStdString(), name.toString().toStdString(), remark.toString().toStdString())));
            readWeaponComponent(iter.first->second);
            locker.unlock();
        }
        else{
            m_XmlReaderWeaponManage.skipCurrentElement();
        }
    }
}

void IWeaponManageServiceImpl::readWeaponComponent(WeaponSystem& weaponSystem){
    while (m_XmlReaderWeaponManage.readNextStartElement()) {
        if (m_XmlReaderWeaponManage.name() == QLatin1String("WeaponComponent") && m_XmlReaderWeaponManage.isStartElement()){
            qDebug()<< m_XmlReaderWeaponManage.attributes().value( QLatin1String("index")) << m_XmlReaderWeaponManage.attributes().value( QLatin1String("type"))
            << m_XmlReaderWeaponManage.attributes().value( QLatin1String("model")) << m_XmlReaderWeaponManage.attributes().value( QLatin1String("protocol"))
            << m_XmlReaderWeaponManage.attributes().value( QLatin1String("name")) << m_XmlReaderWeaponManage.attributes().value( QLatin1String("position"))
            << m_XmlReaderWeaponManage.attributes().value( QLatin1String("disposition")) << m_XmlReaderWeaponManage.attributes().value( QLatin1String("show"))
            << m_XmlReaderWeaponManage.attributes().value( QLatin1String("sdImage")) << m_XmlReaderWeaponManage.attributes().value( QLatin1String("tdImage"))
            << m_XmlReaderWeaponManage.attributes().value( QLatin1String("tdModel")) << m_XmlReaderWeaponManage.attributes().value( QLatin1String("draw"))
            << m_XmlReaderWeaponManage.attributes().value( QLatin1String("remark"));

            const unsigned int nComponentIndex = m_XmlReaderWeaponManage.attributes().value( QLatin1String("index")).toUInt(nullptr, 16);
            const unsigned int componentType   = m_XmlReaderWeaponManage.attributes().value( QLatin1String("type")).toUInt(nullptr, 16);
            const unsigned int componentModel  = m_XmlReaderWeaponManage.attributes().value( QLatin1String("model")).toUInt(nullptr, 16);
            const QStringRef   protocol        = m_XmlReaderWeaponManage.attributes().value( QLatin1String("protocol"));
            const QStringRef   name            = m_XmlReaderWeaponManage.attributes().value( QLatin1String("name"));
            const QStringRef   position        = m_XmlReaderWeaponManage.attributes().value( QLatin1String("position"));
            const unsigned int seatIndex       = m_XmlReaderWeaponManage.attributes().value( QLatin1String("disposition")).toUInt(nullptr, 16);
            const unsigned int color           = (m_XmlReaderWeaponManage.attributes().hasAttribute("color") ? m_XmlReaderWeaponManage.attributes().value( QLatin1String("color")).toUInt(nullptr, 16) : 0xFF0000FF);
            const QStringRef   sdImage         = m_XmlReaderWeaponManage.attributes().value( QLatin1String("sdImage"));
            const QStringRef   tdImage         = m_XmlReaderWeaponManage.attributes().value( QLatin1String("tdImage"));
            const QStringRef   tdModel         = m_XmlReaderWeaponManage.attributes().value( QLatin1String("tdModel"));
            const bool         draw            = (m_XmlReaderWeaponManage.attributes().value( QLatin1String("draw")).toUInt() == 1);
            const bool         show            = (m_XmlReaderWeaponManage.attributes().value( QLatin1String("show")).toUInt() == 1);
            const QStringRef   remark          = m_XmlReaderWeaponManage.attributes().value( QLatin1String("remark"));
            const QStringRef   controlid       = m_XmlReaderWeaponManage.attributes().value( QLatin1String("controlid"));

            weaponSystem.AddWeaponComponent(WeaponComponent(weaponSystem.getSystemType(), weaponSystem.getSystemModel(), nComponentIndex, "", WeaponComponent::FromProtocolIndex(protocol.toString().toStdString()),
            name.toString().toStdString(), remark.toString().toStdString(), position.toString().toStdString(),
            seatIndex, color, sdImage.toString().toStdString(), tdImage.toString().toStdString(), tdModel.toString().toStdString(),
            draw, show, controlid.toString().toStdString(),
            Int2WeaponComponentType(componentType), WeaponComponentModelFromNumber(componentModel)));

            m_XmlReaderWeaponManage.skipCurrentElement();
        }
        else {
            m_XmlReaderWeaponManage.skipCurrentElement();
        }
    }
}

bool IWeaponManageServiceImpl::loadWeaponComparison( const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(nullptr, QObject::tr("QXmlStream ProjectManageFile"),
        QObject::tr("Cannot read file %1:\n%2.")
        .arg(QDir::toNativeSeparators(fileName),
        file.errorString()));
        return false;
    }

    if (!readWeaponComparison(&file)) {
        QMessageBox::warning(nullptr, QObject::tr("QXmlStream ProjectManage"),
        QObject::tr("Parse error in file %1:\n\n%2")
        .arg(QDir::toNativeSeparators(fileName),
        errorString(m_XmlReaderWeaponComparison)));
    } else {

    }

    file.close();
    return true;
}

bool IWeaponManageServiceImpl::readWeaponComparison(QIODevice *device)
{
    m_XmlReaderWeaponComparison.setDevice(device);

    if (m_XmlReaderWeaponComparison.readNextStartElement() && m_XmlReaderWeaponComparison.isStartElement()) {
        if (m_XmlReaderWeaponComparison.name() == ("WeaponSystemComponentComparisonManage") && m_XmlReaderWeaponComparison.attributes().value(versionAttribute()) == QLatin1String("1.0")) {
            readWeaponSystemComponentComparisonManage();
        } else {
            m_XmlReaderWeaponComparison.raiseError(QObject::tr("The file is not an xml version 1.0 file."));
        }
    }

    return !m_XmlReaderWeaponComparison.error();
}

void IWeaponManageServiceImpl::readWeaponSystemComponentComparisonManage(){
    while (m_XmlReaderWeaponComparison.readNextStartElement()) {
        if (m_XmlReaderWeaponComparison.name() == QLatin1String("WeaponSystemComparisonList") && m_XmlReaderWeaponComparison.isStartElement()){
            readWeaponSystemComparisonList();
        }
        else if (m_XmlReaderWeaponComparison.name() == QLatin1String("WeaponComponentComparisonList") && m_XmlReaderWeaponComparison.isStartElement()){
            readWeaponComponentComparisonList();
        }
        else{
            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
    }
}

void IWeaponManageServiceImpl::readWeaponSystemComparisonList(){
    while (m_XmlReaderWeaponComparison.readNextStartElement()) {
        if (m_XmlReaderWeaponComparison.name() == QLatin1String("WeaponSystemType") && m_XmlReaderWeaponComparison.isStartElement()){
            qDebug()<< m_XmlReaderWeaponComparison.attributes().value( QLatin1String("type")) << m_XmlReaderWeaponComparison.attributes().value( QLatin1String("name")) << m_XmlReaderWeaponComparison.attributes().value( QLatin1String("remark"));

            const unsigned int type  = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("type")).toUInt(nullptr, 16);
            const QStringRef   name  = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("name"));
            const QStringRef   sysid = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("sysid"));

            QMutexLocker locker(&m_mutex);
            auto iter = m_mapSystemCategoryComparisonInfo.insert(std::pair<std::string, WeaponSystemTypeComparisonInfo>(sysid.toString().toStdString() , WeaponSystemTypeComparisonInfo(WeaponSystem_Type(type), name.toString().toStdString())) );
            readWeaponSystemTypeList(iter.first->second);
            locker.unlock();
        }
        else{
            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
    }
}

void IWeaponManageServiceImpl::readWeaponSystemTypeList(WeaponSystemTypeComparisonInfo& weaponSystemTypeComparisonInfo)
{
    while (m_XmlReaderWeaponComparison.readNextStartElement()) {
        if (m_XmlReaderWeaponComparison.name() == QLatin1String("SystemModel") && m_XmlReaderWeaponComparison.isStartElement()){
            qDebug()<< m_XmlReaderWeaponComparison.attributes().value( QLatin1String("model")) << m_XmlReaderWeaponComparison.attributes().value( QLatin1String("remark"));

            const unsigned int model  = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("model")).toUInt(nullptr, 16);
            const QStringRef   name   = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("name"));
            const QStringRef   remark = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("remark"));

            WeaponSystemModelComparisonInfoMap* weaponSystemModelComparisonInfoMap = weaponSystemTypeComparisonInfo.GetModelComparisonInfoMap();
            auto iter = weaponSystemModelComparisonInfoMap->insert(std::pair<unsigned int, WeaponSystemModelComparisonInfo>(model, WeaponSystemModelComparisonInfo(weaponSystemTypeComparisonInfo.getWeaponSystemType() , WeaponSystemModelFromNumber(model), name.toString().toStdString()) ));
            readSystemModel(iter.first->second);
        }
        else if (m_XmlReaderWeaponComparison.name() == QLatin1String("ComponentList") && m_XmlReaderWeaponComparison.isStartElement()){
            readComponentList(weaponSystemTypeComparisonInfo);
        }
        else{
            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
    }
}

void IWeaponManageServiceImpl::readSystemModel(WeaponSystemModelComparisonInfo& weaponSystemModelComparisonInfo)
{
    while (m_XmlReaderWeaponComparison.readNextStartElement()) {
        if (m_XmlReaderWeaponComparison.name() == QLatin1String("parameter") && m_XmlReaderWeaponComparison.isStartElement()){
            qDebug()<< m_XmlReaderWeaponComparison.attributes().value( QLatin1String("name")) << m_XmlReaderWeaponComparison.attributes().value( QLatin1String("value"));
            const QStringRef   name  = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("name"));
            const QStringRef   value = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("value"));

            weaponSystemModelComparisonInfo.AddParameter( name.toString().toStdString(),  value.toString().toStdString());

            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
        else{
            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
    }
}

void IWeaponManageServiceImpl::readComponentList(WeaponSystemTypeComparisonInfo& weaponSystemTypeComparisonInfo)
{
    while (m_XmlReaderWeaponComparison.readNextStartElement()) {
        if (m_XmlReaderWeaponComparison.name() == QLatin1String("component") && m_XmlReaderWeaponComparison.isStartElement()){
            qDebug()<< m_XmlReaderWeaponComparison.attributes().value( QLatin1String("componentType")) << m_XmlReaderWeaponComparison.attributes().value( QLatin1String("componentModel"));

            const QStringRef   componentType  = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("componentType"));
            const QStringRef   componentModel = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("componentModel"));

            weaponSystemTypeComparisonInfo.SetWeaponType( std::pair<std::string, std::string>(componentType.toString().toStdString(),  componentModel.toString().toStdString()));

            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
        else{
            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
    }
}

void IWeaponManageServiceImpl::readWeaponComponentComparisonList(){
    while (m_XmlReaderWeaponComparison.readNextStartElement()) {
        if (m_XmlReaderWeaponComparison.name() == QLatin1String("ComponentType") && m_XmlReaderWeaponComparison.isStartElement()){
            qDebug()<< m_XmlReaderWeaponComparison.attributes().value( QLatin1String("type")) << m_XmlReaderWeaponComparison.attributes().value( QLatin1String("remark"));

            const QStringRef   type  = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("type"));
            const QStringRef   name = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("name"));
            const unsigned int typeUInt = type.toUInt(nullptr, 16);

            QMutexLocker locker(&m_mutex);
            auto iter = m_mapModelCategoryComparisonInfo.insert(std::pair<unsigned int, WeaponComponentTypeComparisonInfo>(typeUInt , WeaponComponentTypeComparisonInfo(Int2WeaponComponentType(typeUInt), name.toString().toStdString())) );
            readComponentType(iter.first->second);

            locker.unlock();
        }
        else{
            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
    }
}

void IWeaponManageServiceImpl::readComponentType(WeaponComponentTypeComparisonInfo& weaponComponentTypeComparisonInfo)
{
    while (m_XmlReaderWeaponComparison.readNextStartElement()) {
        if (m_XmlReaderWeaponComparison.name() == QLatin1String("ComponentModel") && m_XmlReaderWeaponComparison.isStartElement()){
            qDebug()<< m_XmlReaderWeaponComparison.attributes().value( QLatin1String("model")) << m_XmlReaderWeaponComparison.attributes().value( QLatin1String("remark"));

            const QStringRef   model  = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("model"));
            const QStringRef   name   = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("name"));
            const unsigned int modelUInt = model.toUInt(nullptr, 16);

            auto iter = weaponComponentTypeComparisonInfo.getMapModelTypeMapRef().insert(std::pair<unsigned int, WeaponComponentModelComparisonInfo>(modelUInt , WeaponComponentModelComparisonInfo(weaponComponentTypeComparisonInfo.getWeaponComponentType(), WeaponComponentModelFromNumber(modelUInt), name.toString().toStdString())) );

            readComponentModel(iter.first->second);
        }
        else{
            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
    }
}

void IWeaponManageServiceImpl::readComponentModel(WeaponComponentModelComparisonInfo& weaponComponentModelComparisonInfo)
{
    while (m_XmlReaderWeaponComparison.readNextStartElement()) {
        if (m_XmlReaderWeaponComparison.name() == QLatin1String("parameter") && m_XmlReaderWeaponComparison.isStartElement()){
            qDebug()<< m_XmlReaderWeaponComparison.attributes().value( QLatin1String("name")) << m_XmlReaderWeaponComparison.attributes().value( QLatin1String("value"));
            const QStringRef   name  = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("name"));
            const QStringRef   value = m_XmlReaderWeaponComparison.attributes().value( QLatin1String("value"));

            weaponComponentModelComparisonInfo.GetParameterMap()->insert(std::pair<std::string, std::string>(name.toString().toStdString(),  value.toString().toStdString()));

            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
        else{
            m_XmlReaderWeaponComparison.skipCurrentElement();
        }
    }
}

std::string IWeaponManageServiceImpl::CreateWeaponSystemID() const
{
    long long nWeaponSysId = WEAPONSYS_ID_RANGE_BEGIN;
    while (IsExistWeaponSystem(QString("%1").arg(nWeaponSysId, 0, 16).toStdString())) {
        nWeaponSysId ++;
        if (nWeaponSysId > WEAPONSYS_ID_RANGE_END){
            nWeaponSysId = WEAPONSYS_ID_RANGE_BEGIN;
            break;
        }
    }
    return QString("%1").arg(nWeaponSysId, 0, 16).toStdString();
}

bool IWeaponManageServiceImpl::GetWeaponSystemMap(WeaponSystemMap& weaponSystemMap) const
{
    QMutexLocker locker(&m_mutex);
    weaponSystemMap = m_mapWeaponSystem;
    return true;
}

bool IWeaponManageServiceImpl::GetWeaponSystemMap(WeaponSystemMap* weaponSystemMap, WeaponSystem_Type weaponSysType, WeaponSystem_Model weaponModelType) const
{
    if (!weaponSystemMap) {
        return false;
    }
    weaponSystemMap->clear();

    QMutexLocker locker(&m_mutex);

    for (const auto& weaponSystemIte :  m_mapWeaponSystem) {
        if (weaponSystemIte.second.getSystemType() == weaponSysType && weaponSystemIte.second.getSystemModel() == weaponModelType) {
            weaponSystemMap->insert(std::make_pair( weaponSystemIte.first,  weaponSystemIte.second));
        }
    }
    return true;
}

bool IWeaponManageServiceImpl::SetWeaponSystemShowFlag(const std::string& strSystemID,bool bShowFlag)
{
	if ("" == strSystemID) return false;
	QMutexLocker locker(&m_mutex);
	const auto& iter = m_mapWeaponSystem.find(strSystemID);
	if (m_mapWeaponSystem.end() != iter)
	{
		WeaponSystem* const pTmpSys = &(iter->second);
		if (pTmpSys)
		{
			pTmpSys->SetSystemShowFlag(bShowFlag);
			return true;
		}
	}
    return false;
}

bool IWeaponManageServiceImpl::SetWeaponSystem(const WeaponSystem& weaponSys)
{
	if ("" == weaponSys.getWeaponSysID()) return false;
	QMutexLocker locker(&m_mutex);
	const auto& iter = m_mapWeaponSystem.find(weaponSys.getWeaponSysID());
	if (iter != m_mapWeaponSystem.end())
	{

	}
	else
	{
		m_mapWeaponSystem.insert(std::make_pair(weaponSys.getWeaponSysID(), weaponSys));
		return true;
	}

    return false;
}

bool IWeaponManageServiceImpl::RemoveWeaponSystem(const std::string& strSysID)
{
	if ("" == strSysID) return false;
    QMutexLocker locker(&m_mutex);
    const auto& iter = m_mapWeaponSystem.find(strSysID);
    if (iter != m_mapWeaponSystem.end())
    {
        m_mapWeaponSystem.erase(iter);
        return true;
    }
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponSystem(WeaponSystem* pWeaponSys, const std::string& strSysID) const
{
    if ("" != strSysID && pWeaponSys)
    {
        QMutexLocker locker(&m_mutex);
        const auto& iter = m_mapWeaponSystem.find(strSysID);
        if (m_mapWeaponSystem.end() != iter)
        {
            const WeaponSystem* const pTmpSys = &(iter->second);
            if (pTmpSys)
            {
                *pWeaponSys = *pTmpSys;
                return true;
            }
        }
    }
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponSystemName(std::string& strSysName, const std::string& strSysID) const
{
	if ("" == strSysID)  return false;
    QMutexLocker locker(&m_mutex);
    const auto& iter = m_mapWeaponSystem.find(strSysID);
    if(iter != m_mapWeaponSystem.end()) {
        const WeaponSystem* const pSysInfo = &(iter->second);
        if (pSysInfo) {
            strSysName = pSysInfo->getWeaponSysName();
            return true;
        }
    }
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponSystemFireRadius(double& dFireRadius, const std::string& strSysID) const
{
    if ("" == strSysID) return false;
    QMutexLocker locker(&m_mutex);
    const auto& iter = m_mapWeaponSystem.find(strSysID);
    if(iter != m_mapWeaponSystem.end()) {
        const WeaponSystem* const pSysInfo = &(iter->second);
        if (pSysInfo) {
            dFireRadius = pSysInfo->getFireRadius();
            return true;
        }
    }
    return false;
}

bool IWeaponManageServiceImpl::SetWeaponSystemFireRadius(const std::string& strSysID, double dFireRadius)
{
    if ("" == strSysID)  return false;
    QMutexLocker locker(&m_mutex);
    const auto& iter = m_mapWeaponSystem.find(strSysID);
    if(iter != m_mapWeaponSystem.end()) {
        WeaponSystem* const pSysInfo = &(iter->second);
        if (pSysInfo) {
            pSysInfo->setFireRadius(dFireRadius);
            return true;
        }
    }
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponSystemModel(WeaponSystem_Type& sysType, WeaponSystem_Model& sysModel, const std::string& strSysID) const
{
	if ("" == strSysID) return false;
	QMutexLocker locker(&m_mutex);
	const auto& iter = m_mapWeaponSystem.find(strSysID);
	if (iter != m_mapWeaponSystem.end()) {
		const WeaponSystem& pSysInfo = (iter->second);
		sysType = pSysInfo.getSystemType();
		sysModel = pSysInfo.getSystemModel();
		return true;
	}
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponSystemIdentify(WeaponSystemModelIdentifyInfo* pOutInfo, const std::string& strSysId)
{
	if ("" == strSysId) return false;
	QMutexLocker locker(&m_mutex);
	const auto& iter = m_mapWeaponSystem.find(strSysId);
	if (iter != m_mapWeaponSystem.end()) {
		const WeaponSystem& pSysInfo = (iter->second);
		WeaponSystemModelIdentifyInfo* pModelIdentifyInfo = GetWSModelIdentifyInfo(strSysId, pSysInfo.getSystemModel());
		if (pModelIdentifyInfo) {
			*pOutInfo = *pModelIdentifyInfo;
			return true;
		}
	}
    return false;
}

WeaponSystemModelIdentifyInfo* IWeaponManageServiceImpl::GetWSModelIdentifyInfo( const std::string& strSysId, const WeaponSystem_Model weaponSystemModel)
{
    const auto& iter = m_mapSystemCategoryComparisonInfo.find(strSysId);
    if (iter != m_mapSystemCategoryComparisonInfo.end())
    {
        return iter->second.GetModelComparisonInfo(weaponSystemModel);
    }
    return nullptr;
}

bool IWeaponManageServiceImpl::GetWeaponComponentName(std::string& strComponentName, const std::string& strComponentID) const
{
	if ("" == strComponentID) return false;
    std::string strSysID = "";
    unsigned int nModelIndex = 0;
    if (WeaponSystem::DecodeWeaponComponentID(strSysID, nModelIndex, strComponentID))
    {
        QMutexLocker locker(&m_mutex);
        const auto& iter = m_mapWeaponSystem.find(strSysID);
        if (iter != m_mapWeaponSystem.end())
        {
            const WeaponSystem& pSysInfo = (iter->second);
            const WeaponComponent* const pModelInfo = pSysInfo.GetWeaponComponent(nModelIndex);
            if (pModelInfo)
            {
                strComponentName = pModelInfo->getComponentName();
                return true;
            }

        }
    }
    return false;
}

const WeaponComponent* IWeaponManageServiceImpl::GetWeaponComponent(const std::string& strSystemID, unsigned int nModelIndex) const
{
    if ("" != strSystemID)
    {
        const auto& iter = m_mapWeaponSystem.find(strSystemID);
        if (m_mapWeaponSystem.end() != iter)
        {
            const WeaponSystem& pSysInfo = (iter->second);
            return pSysInfo.GetWeaponComponent(nModelIndex);
        }
    }
    return nullptr;
}

bool IWeaponManageServiceImpl::GetWeaponComponent(WeaponComponent* pWeaponModel, const std::string& strSysID, unsigned int nIndex) const
{
    if (pWeaponModel && "" != strSysID)
    {
        QMutexLocker locker(&m_mutex);
        const auto& iter = m_mapWeaponSystem.find(strSysID);
        if (m_mapWeaponSystem.end() != iter)
        {
            const WeaponSystem& pSysInfo = (iter->second);
            const WeaponComponent* pTmpModel = pSysInfo.GetWeaponComponent(nIndex);
            if (pTmpModel)
            {
                *pWeaponModel = *pTmpModel;
                return true;
            }
        }
    }
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponComponent(WeaponComponent* pWeaponModel, const std::string& strModelID) const
{
    std::string strSysID = "";
    unsigned int nIndex = 0;
    if (WeaponSystem::DecodeWeaponComponentID(strSysID, nIndex, strModelID))
    {
        return GetWeaponComponent(pWeaponModel, strSysID, nIndex);
    }
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponComponentSeat(std::string& strPosID, unsigned int& nSeatIndex, const std::string& strSysID, unsigned int nModelIndex) const
{
	if ("" == strSysID) return false;
	QMutexLocker locker(&m_mutex);
	const WeaponComponent* pWeaponModel = GetWeaponComponent(strSysID, nModelIndex);
	if (pWeaponModel) {
		return pWeaponModel->getPosition(strPosID, nSeatIndex);
	}
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponComponentSeat(std::string& strPosID, unsigned int& nSeatIndex, const std::string& strModelID) const
{
    bool bRet = false;
    std::string strSysID = "";
    unsigned int nIndex = 0;
    if (WeaponSystem::DecodeWeaponComponentID(strSysID, nIndex, strModelID))
    {
        bRet = GetWeaponComponentSeat(strPosID, nSeatIndex, strSysID, nIndex);
    }
    return bRet;
}

__declspec(deprecated("** this is a deprecated function: SetWeaponComponent **"))
bool IWeaponManageServiceImpl::SetWeaponComponent(const WeaponComponent& weaponModel)
{
    return AddWeaponComponent(weaponModel);
}

bool IWeaponManageServiceImpl::AddWeaponComponent(const WeaponComponent& weaponModel)
{
    std::string	 strModelID = weaponModel.getComponentID();
    std::string	 strSysID = "";
    unsigned int nModelIndex = 0;
    if (WeaponSystem::DecodeWeaponComponentID(strSysID, nModelIndex, strModelID))
    {
        QMutexLocker locker(&m_mutex);
        auto iter = m_mapWeaponSystem.find(strSysID);
        if (m_mapWeaponSystem.end() != iter)
        {
            WeaponSystem& pSysInfo = (iter->second);
            if (pSysInfo.IsExistWeaponComponentIndex(nModelIndex))
            {
                return false;
            }
            else
            {
                return pSysInfo.AddWeaponComponent(weaponModel);
            }
        }
    }
    return false;
}

bool IWeaponManageServiceImpl::SetWeaponComponentShowFlag(const std::string& strComponentID, const bool bShowFlag)
{
	if ("" == strComponentID) return false;
    std::string strSystemID = "";
    unsigned int nComponentIndex = 0;
    if (WeaponSystem::DecodeWeaponComponentID(strSystemID, nComponentIndex, strComponentID)) {
        QMutexLocker locker(&m_mutex);
        WeaponSystemMapItr itrMap = m_mapWeaponSystem.find(strSystemID);
        if (itrMap != m_mapWeaponSystem.end()) {
            WeaponSystem* pWeaponSystem = &(itrMap->second);
            if (pWeaponSystem) {
                return pWeaponSystem->SetWeaponComponentShowFlag(nComponentIndex, bShowFlag);
            }
        }
    }

    return false;
}

bool IWeaponManageServiceImpl::RemoveWeaponComponent(const std::string& strSysID, unsigned int nModelIndex)
{
	if ("" == strSysID) return false;
	QMutexLocker locker(&m_mutex);
	WeaponSystemMapItr itrMap = m_mapWeaponSystem.find(strSysID);
	if (m_mapWeaponSystem.end() != itrMap){
		WeaponSystem* pSystem = &(itrMap->second);
		if (pSystem) {
			return pSystem->RemoveWeaponComponent(nModelIndex);
		}
	}
    return false;
}

bool IWeaponManageServiceImpl::RemoveWeaponComponent(const std::string& strComponentID)
{
	if ("" == strComponentID) return false;
    std::string strSysID = "";
    unsigned int nModelIndex = 0;
    if (WeaponSystem::DecodeWeaponComponentID(strSysID, nModelIndex, strComponentID))
    {
        QMutexLocker locker(&m_mutex);
        WeaponSystemMapItr itrMap = m_mapWeaponSystem.find(strSysID);
        if (m_mapWeaponSystem.end() != itrMap)
        {
            WeaponSystem* pSystem = &(itrMap->second);
            if (pSystem)
            {
                return pSystem->RemoveWeaponComponent(nModelIndex);
            }
        }
    }
    return false;
}

bool IWeaponManageServiceImpl::IsExistWeaponSystem(const std::string& strSysID) const
{
	if ("" == strSysID) return false;
	QMutexLocker locker(&m_mutex);
	const auto& itrMap = m_mapWeaponSystem.find(strSysID);
	if (itrMap != m_mapWeaponSystem.end()) {
		return true;
	}
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponSystemCalcModelIndex(unsigned int& calcModeIndex, WeaponSystem_Type sysType, WeaponSystem_Model sysModel) const
{
    if (WeaponSystem_Type::WeaponSystem_Type_Unknow != sysType && WeaponSystem_Model::WeaponSystem_Model_Unknow != sysModel) {
        QMutexLocker locker(&m_mutex);
        const auto& itrCategoryMap = m_mapSystemCategoryComparisonInfo.find("sysType");
        if (itrCategoryMap != m_mapSystemCategoryComparisonInfo.end()) {
            const auto& pSystemTypeMap = (itrCategoryMap->second).getMapSysModelMap();
            const auto& itrSysTypeMap = pSystemTypeMap.find(sysModel);
            if (itrSysTypeMap != pSystemTypeMap.end()) {
                const auto* const pSysTypeInfo = &(itrSysTypeMap->second);
                std::string strValue = "";
                if (pSysTypeInfo->GetParameter(strValue, "计算模型索引")) {
                    calcModeIndex =  std::strtoul(strValue.c_str(), nullptr, 16);
                    return true;
                }
            }
        }
    }
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponComponentShowFlag(bool& bShowFlags, const std::string& strWaeaponComponentId) const
{
    if ("" == strWaeaponComponentId) return false;
    std::string strSysId;
    unsigned int nComponentIndex=0;
    WeaponSystem::DecodeWeaponComponentID(strSysId, nComponentIndex, strWaeaponComponentId);
    QMutexLocker locker(&m_mutex);
    const auto& itrMap = m_mapWeaponSystem.find(strSysId);
    if(itrMap != m_mapWeaponSystem.end()) {
        const WeaponSystem* const pSysInfo = &(itrMap->second);
        if (pSysInfo) {
            const WeaponComponent* const pComponent = pSysInfo->GetWeaponComponent(nComponentIndex);
            if (pComponent) {
                bShowFlags = pComponent->getShowFlag();
                return true;
            }
        }
    }
    return false;
}

bool IWeaponManageServiceImpl::GetWeaponSystemTypeComparisonInfoMap(WeaponSystemTypeComparisonInfoMap* pMap) const
{
	if (pMap) {
		QMutexLocker locker(&m_mutex);
		*pMap = m_mapSystemCategoryComparisonInfo;
		return true;
	}
	return false;
}

bool IWeaponManageServiceImpl::GetWeaponSystemTypeComparisonInfo(WeaponSystemTypeComparisonInfo* pInfo, WeaponSystem_Type nSystemType) const
{
	if (pInfo) {
		QMutexLocker locker(&m_mutex);
		std::string strSystemType = QString::number(nSystemType,10).toStdString();
		WeaponSystemTypeComparisonInfoMap::const_iterator itrMap = m_mapSystemCategoryComparisonInfo.find(strSystemType);
		if (itrMap != m_mapSystemCategoryComparisonInfo.end()) {
			const WeaponSystemTypeComparisonInfo* pSysInfo = &(itrMap->second);
			if (pSysInfo) {
				*pInfo = *pSysInfo;
				return true;
			}
		}
	}
	return false;
}

bool IWeaponManageServiceImpl::RemoveWeaponSystemTypeComparisonInfo(WeaponSystem_Type sysType)
{
	QMutexLocker locker(&m_mutex);
	std::string strSystemType = QString::number(sysType,10).toStdString();
	WeaponSystemTypeComparisonInfoMapItr itrMap = m_mapSystemCategoryComparisonInfo.find(strSystemType);
	if (itrMap != m_mapSystemCategoryComparisonInfo.end()) {
		m_mapSystemCategoryComparisonInfo.erase(itrMap);
		return true;
	}
	return false;
}

bool IWeaponManageServiceImpl::SetWeaponSystemTypeComparisonInfo(const WeaponSystemTypeComparisonInfo& SysCategoryInfo)
{
	if (WeaponSystem_Type_Unknow != SysCategoryInfo.getWeaponSystemType()) {
		QMutexLocker locker(&m_mutex);
		std::string strSystemType = QString::number(SysCategoryInfo.getWeaponSystemType(), 10).toStdString();
		WeaponSystemTypeComparisonInfoMapItr itrMap = m_mapSystemCategoryComparisonInfo.find(strSystemType);
		if (itrMap != m_mapSystemCategoryComparisonInfo.end()) {
			m_mapSystemCategoryComparisonInfo.erase(itrMap);
		}
		m_mapSystemCategoryComparisonInfo.insert(std::make_pair(strSystemType, SysCategoryInfo));
		return true;
	}
	return false;
}

bool IWeaponManageServiceImpl::GetWeaponComponentTypeComparisonInfoMap(WeaponComponentTypeComparisonInfoMap* pMap) const
{
	if (pMap) {
		QMutexLocker locker(&m_mutex);
		*pMap = m_mapModelCategoryComparisonInfo;
		return true;
	}
	return false;
}

bool IWeaponManageServiceImpl::GetWeaponComponentTypeComparisonInfo(WeaponComponentTypeComparisonInfo* pInfo, WeaponComponent_Type nComponentType) const
{
	if (pInfo) {
		QMutexLocker locker(&m_mutex);
		WeaponComponentTypeComparisonInfoMap::const_iterator itrMap = m_mapModelCategoryComparisonInfo.find(nComponentType);
		if (itrMap != m_mapModelCategoryComparisonInfo.end()) {
			const WeaponComponentTypeComparisonInfo* pModelInfo = &(itrMap->second);
			if (pModelInfo) {
				*pInfo = *pModelInfo;
				return true;
			}
		}
	}
	return false;
}

bool IWeaponManageServiceImpl::RemoveWeaponComponentTypeComparisonInfo(WeaponComponent_Type nWeaponType)
{
	if (WeaponComponent_Type_Unknow != nWeaponType) {
		QMutexLocker locker(&m_mutex);
		WeaponComponentTypeComparisonInfoMapItr itrMap = m_mapModelCategoryComparisonInfo.find(nWeaponType);
		if (itrMap != m_mapModelCategoryComparisonInfo.end()) {
			m_mapModelCategoryComparisonInfo.erase(itrMap);
			return true;
		}
	}
	return false;
}

bool IWeaponManageServiceImpl::SetWeaponComponentTypeComparisonInfo(const WeaponComponentTypeComparisonInfo& ModelCategoryInfo)
{
	if (WeaponComponent_Type_Unknow != ModelCategoryInfo.getWeaponComponentType()) {
		QMutexLocker locker(&m_mutex);
		WeaponComponentTypeComparisonInfoMapItr itrMap = m_mapModelCategoryComparisonInfo.find(ModelCategoryInfo.getWeaponComponentType());
		if (itrMap != m_mapModelCategoryComparisonInfo.end()) {
			m_mapModelCategoryComparisonInfo.erase(itrMap);
		}
		m_mapModelCategoryComparisonInfo.insert(std::make_pair(ModelCategoryInfo.getWeaponComponentType(), ModelCategoryInfo));
		return true;
	}
	return false;
}

void IWeaponManageServiceImpl::GetCWCTypeComMap(WeaponComponentTypeComparisonInfoMap& CWCTypeMap) const
{
	QMutexLocker locker(&m_mutex);
	CWCTypeMap = m_mapModelCategoryComparisonInfo;
}

void IWeaponManageServiceImpl::GetCWSTypeComMap(WeaponSystemTypeComparisonInfoMap& CWSTypeMap) const
{
	QMutexLocker locker(&m_mutex);
	CWSTypeMap = m_mapSystemCategoryComparisonInfo;
}
