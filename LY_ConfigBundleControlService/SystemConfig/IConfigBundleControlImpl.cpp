#include "IConfigBundleControlImpl.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

using namespace qnzkna::SystemConfig;

IConfigBundleControlImpl::IConfigBundleControlImpl()
    : m_xmlReader()
    , m_strFilePath("")
    , m_vecMessageInfo()
    , m_vecServiceInfo()
    , m_vecBundleInfo()
{
    m_vecMessageInfo.clear();
    m_vecServiceInfo.clear();
    m_vecBundleInfo.clear();
    m_strFilePath = "../data/PluginConfigFile.xml";
    load(QString::fromStdString(m_strFilePath));
}

bool IConfigBundleControlImpl::load(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(nullptr, QObject::tr("QXmlStream PluginConfigFile"),
                             QObject::tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    if (!read(&file))
    {
        QMessageBox::warning(nullptr, QObject::tr("QXmlStream PluginConfig"),
                             QObject::tr("Parse error in file %1:\n\n%2")
                             .arg(QDir::toNativeSeparators(fileName),
                                  errorString()));
    }
    else
    {

    }

    file.close();
    return true;
}

bool IConfigBundleControlImpl::read(QIODevice *device)
{
    m_xmlReader.setDevice(device);

    if (m_xmlReader.readNextStartElement() && m_xmlReader.isStartElement())
    {
        if (m_xmlReader.name() == QLatin1String("PluginConfigInfo") && m_xmlReader.attributes().value(versionAttribute()) == QLatin1String("1.0"))
        {
            readPluginConfig();
        }
        else
        {
            m_xmlReader.raiseError(QObject::tr("The file is not an xml version 1.0 file."));
        }
    }

    return !m_xmlReader.error();
}

QString IConfigBundleControlImpl::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3").arg(m_xmlReader.errorString()).arg(m_xmlReader.lineNumber()).arg(m_xmlReader.columnNumber());
}

void IConfigBundleControlImpl::readPluginConfig()
{
    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == QLatin1String("Services") && m_xmlReader.isStartElement())
        {
            readServices();
        }
        else if (m_xmlReader.name() == QLatin1String("Messages") && m_xmlReader.isStartElement())
        {
            readMessages();
        }
        else if (m_xmlReader.name() == QLatin1String("Bundles") && m_xmlReader.isStartElement())
        {
            readBundles();
        }
        else
        {
            m_xmlReader.skipCurrentElement();
        }
    }
}

void IConfigBundleControlImpl::readServices()
{
    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == QLatin1String("Service") && m_xmlReader.isStartElement())
        {

            qDebug() << m_xmlReader.attributes().value(QLatin1String("name")) << m_xmlReader.attributes().value(QLatin1String("remark")) ;

            const QStringRef name   = m_xmlReader.attributes().value(QLatin1String("name"));
            const QStringRef remark = m_xmlReader.attributes().value(QLatin1String("remark"));

            m_vecServiceInfo.push_back(SystemBundle::BundleService());
            SystemBundle::BundleService &bundleService = m_vecServiceInfo.back();
            bundleService.m_strName        = name.toString().toStdString();
            bundleService.m_strDescription = remark.toString().toStdString();

            m_xmlReader.skipCurrentElement();
        }
        else
        {
            m_xmlReader.skipCurrentElement();
        }
    }
}

void IConfigBundleControlImpl::readMessages()
{
    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == QLatin1String("Message") && m_xmlReader.isStartElement())
        {

            qDebug() << m_xmlReader.attributes().value(QLatin1String("group")) << m_xmlReader.attributes().value(QLatin1String("subgroup")) << m_xmlReader.attributes().value(QLatin1String("remark")) ;

            const QStringRef group   = m_xmlReader.attributes().value(QLatin1String("group"));
            const QStringRef subgroup   = m_xmlReader.attributes().value(QLatin1String("subgroup"));
            const QStringRef remark = m_xmlReader.attributes().value(QLatin1String("remark"));

            m_vecMessageInfo.push_back(SystemBundle::BundleMessage());
            SystemBundle::BundleMessage &bundleMessage = m_vecMessageInfo.back();
            bundleMessage.m_strGroup        = group.toString().toStdString();
            bundleMessage.m_strSubGroup     = subgroup.toString().toStdString();
            bundleMessage.m_strDescription  = remark.toString().toStdString();

            m_xmlReader.skipCurrentElement();
        }
        else
        {
            m_xmlReader.skipCurrentElement();
        }
    }
}

void IConfigBundleControlImpl::readBundles()
{
    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == QLatin1String("Bundle") && m_xmlReader.isStartElement())
        {

            qDebug() << m_xmlReader.attributes().value(QLatin1String("name")) << m_xmlReader.attributes().value(QLatin1String("type"))
                     << m_xmlReader.attributes().value(QLatin1String("classname")) << m_xmlReader.attributes().value(QLatin1String("releaseinfo"))
                     << m_xmlReader.attributes().value(QLatin1String("debuginfo")) << m_xmlReader.attributes().value(QLatin1String("remark"));

            const QStringRef name        = m_xmlReader.attributes().value(QLatin1String("name"));
            const QStringRef type        = m_xmlReader.attributes().value(QLatin1String("type"));
            const QStringRef classname   = m_xmlReader.attributes().value(QLatin1String("classname"));
            const QStringRef releaseinfo = m_xmlReader.attributes().value(QLatin1String("releaseinfo"));
            const QStringRef debuginfo   = m_xmlReader.attributes().value(QLatin1String("debuginfo"));
            const QStringRef remark      = m_xmlReader.attributes().value(QLatin1String("remark"));

            m_vecBundleInfo.push_back(SystemBundle());
            SystemBundle &systemBundle = m_vecBundleInfo.back();

            systemBundle.setBundleName(name.toString().toStdString());
            systemBundle.SetTypeByString(type.toString().toLower().toStdString());
            systemBundle.setClassName(classname.toString().toStdString());
            systemBundle.setLibNameRelease(releaseinfo.toString().toStdString());
            systemBundle.setLibNameDebug(debuginfo.toString().toStdString());
            systemBundle.setNote(remark.toString().toStdString());

            while (m_xmlReader.readNextStartElement())
            {
                if (m_xmlReader.name() == QLatin1String("ServiceSubSet") && m_xmlReader.isStartElement())
                {
                    readServiceSubSet(systemBundle.getServiceOrderVec());
                }
                else if (m_xmlReader.name() == QLatin1String("ServiceRegSet") && m_xmlReader.isStartElement())
                {
                    readServiceRegSet(systemBundle.getServiceRegistVec());
                }
                else if (m_xmlReader.name() == QLatin1String("MessageSubSet") && m_xmlReader.isStartElement())
                {
                    readMessageSubSet(systemBundle.getMessageOrderVec());
                }
                else if (m_xmlReader.name() == QLatin1String("MessagePubSet") && m_xmlReader.isStartElement())
                {
                    readMessagePubSet(systemBundle.getMessageDispatchVec());
                }
                else
                {
                    m_xmlReader.skipCurrentElement();
                }
            }

        }
        else
        {
            m_xmlReader.skipCurrentElement();
        }
    }
}

void IConfigBundleControlImpl::readServiceSubSet(std::vector<SystemBundle::BundleService>  &serviceOrderVec)
{
    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == QLatin1String("Service") && m_xmlReader.isStartElement())
        {

            qDebug() << m_xmlReader.attributes().value(QLatin1String("name")) << m_xmlReader.attributes().value(QLatin1String("remark")) ;

            const QStringRef name    = m_xmlReader.attributes().value(QLatin1String("name"));
            const QStringRef remark  = m_xmlReader.attributes().value(QLatin1String("remark"));

            serviceOrderVec.push_back(SystemBundle::BundleService());
            serviceOrderVec.back().m_strName        = name.toString().toStdString();
            serviceOrderVec.back().m_strDescription = remark.toString().toStdString();

            m_xmlReader.skipCurrentElement();
        }
        else
        {
            m_xmlReader.skipCurrentElement();
        }
    }
}

void IConfigBundleControlImpl::readServiceRegSet(std::vector<SystemBundle::BundleService>  &serviceRegistVec)
{
    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == QLatin1String("Service") && m_xmlReader.isStartElement())
        {

            qDebug() << m_xmlReader.attributes().value(QLatin1String("name")) << m_xmlReader.attributes().value(QLatin1String("remark")) ;

            const QStringRef name    = m_xmlReader.attributes().value(QLatin1String("name"));
            const QStringRef remark  = m_xmlReader.attributes().value(QLatin1String("remark"));

            serviceRegistVec.push_back(SystemBundle::BundleService());
            serviceRegistVec.back().m_strName        = name.toString().toStdString();
            serviceRegistVec.back().m_strDescription = remark.toString().toStdString();

            m_xmlReader.skipCurrentElement();
        }
        else
        {
            m_xmlReader.skipCurrentElement();
        }
    }
}

void IConfigBundleControlImpl::readMessageSubSet(std::vector<SystemBundle::BundleMessage>  &messageOrderVec)
{
    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == QLatin1String("Message") && m_xmlReader.isStartElement())
        {

            qDebug() << m_xmlReader.attributes().value(QLatin1String("groupname")) << m_xmlReader.attributes().value(QLatin1String("subgroupname")) ;

            const QStringRef groupname    = m_xmlReader.attributes().value(QLatin1String("groupname"));
            const QStringRef subgroupname = m_xmlReader.attributes().value(QLatin1String("subgroupname"));
            const QStringRef remark       = m_xmlReader.attributes().value(QLatin1String("remark"));

            messageOrderVec.push_back(SystemBundle::BundleMessage());
            messageOrderVec.back().m_strGroup        = groupname.toString().toStdString();
            messageOrderVec.back().m_strSubGroup     = subgroupname.toString().toStdString();
            messageOrderVec.back().m_strDescription  = remark.toString().toStdString();

            m_xmlReader.skipCurrentElement();
        }
        else
        {
            m_xmlReader.skipCurrentElement();
        }
    }
}

void IConfigBundleControlImpl::readMessagePubSet(std::vector<SystemBundle::BundleMessage>  &messageDispatchVec)
{
    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == QLatin1String("Message") && m_xmlReader.isStartElement())
        {

            qDebug() << m_xmlReader.attributes().value(QLatin1String("groupname")) << m_xmlReader.attributes().value(QLatin1String("subgroupname")) ;

            const QStringRef groupname    = m_xmlReader.attributes().value(QLatin1String("groupname"));
            const QStringRef subgroupname = m_xmlReader.attributes().value(QLatin1String("subgroupname"));
            const QStringRef remark       = m_xmlReader.attributes().value(QLatin1String("remark"));

            messageDispatchVec.push_back(SystemBundle::BundleMessage());
            messageDispatchVec.back().m_strGroup        = groupname.toString().toStdString();
            messageDispatchVec.back().m_strSubGroup     = subgroupname.toString().toStdString();
            messageDispatchVec.back().m_strDescription  = remark.toString().toStdString();

            m_xmlReader.skipCurrentElement();
        }
        else
        {
            m_xmlReader.skipCurrentElement();
        }
    }
}

const std::vector<SystemBundle> &IConfigBundleControlImpl::getVecBundleInfo() const
{
    return m_vecBundleInfo;
}

void IConfigBundleControlImpl::setVecBundleInfo(const std::vector<SystemBundle> &vecBundleInfo)
{
    m_vecBundleInfo = vecBundleInfo;
}

