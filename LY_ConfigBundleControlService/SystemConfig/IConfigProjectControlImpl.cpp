#include "IConfigProjectControlImpl.h"
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
using namespace qnzkna::SystemConfig;

IConfigProjectControlImpl::IConfigProjectControlImpl()
    : m_XmlReader()
    , m_strFilePath("")
    , m_systemSeatInfo()
    , m_strCurProject("")
    , m_vecProjectInfo()
    , m_vecCommunicateGroups()
{
    m_vecProjectInfo.clear();
    m_vecCommunicateGroups.clear();
    m_strFilePath ="../data/ProjectManageFile.xml";
    load(QString::fromStdString(m_strFilePath));
}

std::string IConfigProjectControlImpl::strFilePath() const
{
    return m_strFilePath;
}

void IConfigProjectControlImpl::setStrFilePath(const std::string &strFilePath)
{
    m_strFilePath = strFilePath;
}

const std::set<std::string> &IConfigProjectControlImpl::vecCommunicateGroups() const
{
    return m_vecCommunicateGroups;
}

const std::vector<SystemProject> &IConfigProjectControlImpl::vecProjectInfo() const
{
    return m_vecProjectInfo;
}

std::string IConfigProjectControlImpl::strCurProject() const
{
    return m_strCurProject;
}

void IConfigProjectControlImpl::setStrCurProject(const std::string &strCurProject)
{
    m_strCurProject = strCurProject;
}

const SystemSeatInfo &IConfigProjectControlImpl::systemSeatInfo() const
{
    return m_systemSeatInfo;
}

bool IConfigProjectControlImpl::load(const QString &fileName)
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

    if (!read(&file))
    {
        QMessageBox::warning(nullptr, QObject::tr("QXmlStream ProjectManage"),
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

bool IConfigProjectControlImpl::read(QIODevice *device)
{
    m_XmlReader.setDevice(device);

    if (m_XmlReader.readNextStartElement() && m_XmlReader.isStartElement())
    {
        if (m_XmlReader.name() == QLatin1String("ProjectManage") && m_XmlReader.attributes().value(versionAttribute()) == QLatin1String("1.0"))
        {
            readPluginConfig();
        }
        else
        {
            m_XmlReader.raiseError(QObject::tr("The file is not an xml version 1.0 file."));
        }
    }

    return !m_XmlReader.error();
}

QString IConfigProjectControlImpl::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3").arg(m_XmlReader.errorString()).arg(m_XmlReader.lineNumber()).arg(m_XmlReader.columnNumber());
}

void IConfigProjectControlImpl::readPluginConfig()
{
    while (m_XmlReader.readNextStartElement())
    {
        if (m_XmlReader.name() == QLatin1String("CommunicateGroups") && m_XmlReader.isStartElement())
        {
            readCommunicateGroups();
        }
        else if (m_XmlReader.name() == QLatin1String("SeatInfo") && m_XmlReader.isStartElement())
        {
            readSeatInfo();
        }
        else if (m_XmlReader.name() == QLatin1String("Projects") && m_XmlReader.isStartElement())
        {
            readProjects();
        }
        else
        {
            m_XmlReader.skipCurrentElement();
        }
    }
}

void IConfigProjectControlImpl::readCommunicateGroups()
{
    while (m_XmlReader.readNextStartElement())
    {
        if (m_XmlReader.name() == QLatin1String("CommunicateGroup") && m_XmlReader.isStartElement())
        {
            qDebug() << m_XmlReader.attributes().value(QLatin1String("name")) << m_XmlReader.attributes().value(QLatin1String("remark")) ;
            const QStringRef strGrpName = m_XmlReader.attributes().value(QLatin1String("name"));
            m_vecCommunicateGroups.insert(strGrpName.toString().toStdString());
            m_XmlReader.skipCurrentElement();
        }
        else
        {
            m_XmlReader.skipCurrentElement();
        }
    }
}

void IConfigProjectControlImpl::readSeatInfo()
{
    qDebug() << m_XmlReader.attributes().value(QLatin1String("type")) << m_XmlReader.attributes().value(QLatin1String("code"))
             << m_XmlReader.attributes().value(QLatin1String("category")) << m_XmlReader.attributes().value(QLatin1String("group"))
             << m_XmlReader.attributes().value(QLatin1String("state")) ;

    const QStringRef type     = m_XmlReader.attributes().value(QLatin1String("type"));
    const QStringRef code     = m_XmlReader.attributes().value(QLatin1String("code"));
    const QStringRef category = m_XmlReader.attributes().value(QLatin1String("category"));
    const QStringRef group    = m_XmlReader.attributes().value(QLatin1String("group"));
    const QStringRef state    = m_XmlReader.attributes().value(QLatin1String("state"));

    SystemSeat_Type seatType = SystemSeat_Type::SystemSeat_Type_Null;
    if (category.compare(QStringLiteral("command"), Qt::CaseInsensitive) == 0)
    {
        seatType = SystemSeat_Type::SystemSeat_Type_Command;
    }
    else if (category.compare(QStringLiteral("operation"), Qt::CaseInsensitive) == 0)
    {
        seatType = SystemSeat_Type::SystemSeat_Type_Operation;
    }
    else if (category.compare(QStringLiteral("intelligence"), Qt::CaseInsensitive) == 0)
    {
        seatType = SystemSeat_Type::SystemSeat_Type_Intelligence;
    }
    else if (category.compare(QStringLiteral("coordination"), Qt::CaseInsensitive) == 0)
    {
        seatType = SystemSeat_Type::SystemSeat_Type_Coordination;
    }
    else if (category.compare(QStringLiteral("individual"), Qt::CaseInsensitive) == 0)
    {
        seatType = SystemSeat_Type::SystemSeat_Type_Individual;
    }

    SystemSeat_State seatState = SystemSeat_State::SystemSeatState_Null;
    if (state.compare(QStringLiteral("activity"), Qt::CaseInsensitive) == 0)
    {
        seatState = SystemSeat_State::SystemSeatState_Activity;
    }
    else if (state.compare(QStringLiteral("reserve"), Qt::CaseInsensitive) == 0)
    {
        seatState = SystemSeat_State::SystemSeatState_Reserve;
    }

    m_systemSeatInfo = SystemSeatInfo(seatType,  seatState, 1, static_cast<SystemNode_Type>(type.toInt()), code.toString().toStdString(),  group.toString().toStdString());

    m_XmlReader.skipCurrentElement();
}

void IConfigProjectControlImpl::readProjects()
{
    while (m_XmlReader.readNextStartElement())
    {
        if (m_XmlReader.name() == QLatin1String("Project") && m_XmlReader.isStartElement())
        {

            qDebug() << m_XmlReader.attributes().value(QLatin1String("name")) << m_XmlReader.attributes().value(QLatin1String("remark"));

            const QStringRef name   = m_XmlReader.attributes().value(QLatin1String("name"));
            const QStringRef remark = m_XmlReader.attributes().value(QLatin1String("remark"));
            SystemProject systemProject;
            systemProject.setStrProjectName(name.toString().toStdString());
            systemProject.setStrProjectRemark(remark.toString().toStdString());

            m_vecProjectInfo.push_back(systemProject);

            readProject(m_vecProjectInfo.back().GetProjectBundleVector());

        }
        else
        {
            m_XmlReader.skipCurrentElement();
        }
    }
}

void IConfigProjectControlImpl::readProject(std::vector<SystemProject::ProjectBundle> &projectBundle)
{
    while (m_XmlReader.readNextStartElement())
    {
        if (m_XmlReader.name() == QLatin1String("Bundles") && m_XmlReader.isStartElement())
        {

            qDebug() << m_XmlReader.attributes().value(QLatin1String("name")) << m_XmlReader.attributes().value(QLatin1String("remark")) ;
            readBundles(projectBundle);
        }
        else
        {
            m_XmlReader.skipCurrentElement();
        }
    }
}

void IConfigProjectControlImpl::readBundles(std::vector<SystemProject::ProjectBundle> &projectBundle)
{
    while (m_XmlReader.readNextStartElement())
    {
        if (m_XmlReader.name() == QLatin1String("Bundle") && m_XmlReader.isStartElement())
        {
            qDebug() << m_XmlReader.attributes().value(QLatin1String("name")) << m_XmlReader.attributes().value(QLatin1String("enable")) ;

            const QStringRef name             = m_XmlReader.attributes().value(QLatin1String("name"));
            const QStringRef used             = m_XmlReader.attributes().value(QLatin1String("enable"));
            const QStringRef windowPos        = m_XmlReader.attributes().value(QLatin1String("windowPos"));
            const QStringRef windowName       = m_XmlReader.attributes().value(QLatin1String("windowName"));
            const QStringRef windowShowName   = m_XmlReader.attributes().value(QLatin1String("windowShowName"));

            SystemProject::ProjectBundle bundle;
            bundle.m_strName            = name.toString().toStdString();
            bundle.m_bUsed              = (used.toString().compare("true", Qt::CaseInsensitive)  == 0);
            bundle.m_WindowPos          = windowPos.toInt();
            bundle.m_strWindowName      = windowName.toString().toStdString();
            bundle.m_strWindowShowName  = windowShowName.toString().toStdString();

            projectBundle.push_back(bundle);

            m_XmlReader.skipCurrentElement();
        }
        else
        {
            m_XmlReader.skipCurrentElement();
        }
    }
}
