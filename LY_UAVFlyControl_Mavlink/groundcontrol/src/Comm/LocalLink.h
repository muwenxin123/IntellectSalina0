#ifndef LOCALLINK_H
#define LOCALLINK_H

#include <QObject>

#include "LinkConfiguration.h"
#include "LinkInterface.h"

class LocalConfiguration;


namespace qnzkna
{
namespace framework
{


class MessageReference;

}
}

class LocalLink : public LinkInterface
{
    Q_OBJECT

public:
    LocalLink(SharedLinkConfigurationPtr& config);
    virtual ~LocalLink() override;

    // LinkInterface overrides
    bool isConnected(void) const override;
    void disconnect (void) override;

    // QThread overrides
    void run(void) override;

    bool dealingMessage(const char* const recvMsg, const unsigned int msgLength, const unsigned long long weaponId);

public slots:
    void readBytes();

private slots:
    // LinkInterface overrides
    void _writeBytes(unsigned long long int weaponId, const QByteArray data) override;

private:
    // LinkInterface overrides
    bool _connect(void) override;
    void _writeDataGram     (unsigned long long weaponId, const QByteArray data);

    bool                _running;
    LocalConfiguration* _udpConfig;
    bool                _connectState;
};

class LocalConfiguration : public LinkConfiguration
{
    Q_OBJECT
public:

    LocalConfiguration(const QString& name);
    LocalConfiguration(LocalConfiguration* source);
    ~LocalConfiguration() override;


    /// LinkConfiguration overrides
    LinkType    type                 (void) override                                        { return LinkConfiguration::TypeLocal; }
    void        copyFrom             (LinkConfiguration* source) override;
    void        loadSettings         (QSettings& settings, const QString& root) override;
    void        saveSettings         (QSettings& settings, const QString& root) override;
    QString     settingsURL          (void) override                                        { return "LocalSettings.qml"; }
    QString     settingsTitle        (void) override                                        { return tr("Local Link Settings"); }
};


#endif // LOCALLINK_H
