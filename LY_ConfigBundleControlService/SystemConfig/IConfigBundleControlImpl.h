#ifndef ICONFIGBUNDLECONTROLIMPL_H
#define ICONFIGBUNDLECONTROLIMPL_H

#include <QXmlStreamReader>

#include "SystemConfig/SystemBundle.h"
#include "SystemConfig/IConfigBundleControlService.h"

namespace qnzkna
{
namespace SystemConfig
{
class IConfigBundleControlImpl : public IConfigBundleControlService
{
public:
    IConfigBundleControlImpl();

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

    const std::vector<SystemBundle> &getVecBundleInfo() const;
    void setVecBundleInfo(const std::vector<SystemBundle> &vecBundleInfo);

private:
    void readPluginConfig();
    void readServices();
    void readMessages();
    void readBundles();
    void readServiceSubSet(std::vector<SystemBundle::BundleService>  &serviceOrderVec);
    void readServiceRegSet(std::vector<SystemBundle::BundleService>  &serviceRegistVec);
    void readMessageSubSet(std::vector<SystemBundle::BundleMessage>  &messageOrderVec);
    void readMessagePubSet(std::vector<SystemBundle::BundleMessage>  &messageDispatchVec);

private:
    QXmlStreamReader                          m_xmlReader;
    std::string                               m_strFilePath;
    std::vector<SystemBundle::BundleMessage>  m_vecMessageInfo;
    std::vector<SystemBundle::BundleService>  m_vecServiceInfo;
    std::vector<SystemBundle>                 m_vecBundleInfo;
};
}
}

#endif
