#ifndef ICONFIGPROJECTCONTROLIMPL_H
#define ICONFIGPROJECTCONTROLIMPL_H

#include <set>
#include <string>

#include <QXmlStreamReader>

#include "SystemConfig/SystemSeatInfo.h"
#include "SystemConfig/SystemProject.h"
#include "SystemConfig/IConfigProjectControlService.h"

namespace qnzkna
{
namespace SystemConfig
{
class IConfigProjectControlImpl : public IConfigProjectControlService
{
public:
    IConfigProjectControlImpl();

    std::string strFilePath() const;
    void setStrFilePath(const std::string &strFilePath);

    virtual const SystemSeatInfo &systemSeatInfo() const;

    virtual std::string strCurProject() const;
    void setStrCurProject(const std::string &strCurProject);

    virtual const std::vector<SystemProject> &vecProjectInfo() const;

    virtual const std::set<std::string> &vecCommunicateGroups() const;

private:
    bool load(const QString &fileName);
    bool read(QIODevice *device);

    QString errorString() const;

    static inline QString versionAttribute()
    {
        return QStringLiteral("version");
    }
    static inline QString hrefAttribute()
    {
        return QStringLiteral("href");
    }

private:
    void readPluginConfig();
    void readCommunicateGroups();
    void readSeatInfo();
    void readProjects();
    void readProject(std::vector<SystemProject::ProjectBundle> &projectBundle);
    void readBundles(std::vector<SystemProject::ProjectBundle> &projectBundle);

private:
    QXmlStreamReader           m_XmlReader;
    std::string                m_strFilePath;
    SystemSeatInfo             m_systemSeatInfo;
    std::string                m_strCurProject;
    std::vector<SystemProject> m_vecProjectInfo;
    std::set<std::string>      m_vecCommunicateGroups;
};
}
}

#endif
