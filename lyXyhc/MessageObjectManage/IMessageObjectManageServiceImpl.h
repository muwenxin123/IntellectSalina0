#ifndef IMESSAGEOBJECTMANAGESERVICEIMPL_H
#define IMESSAGEOBJECTMANAGESERVICEIMPL_H

#include "DataManage/MessageObjectManage/IMessageObjectManageService.h"

#include <QMap>
#include <QMutex>

namespace qnzkna
{
namespace MessageObjectManage
{

class IMessageObjectManageServiceImpl : public IMessageObjectManageService
{
public:
    static IMessageObjectManageServiceImpl& Get();

private:
    explicit IMessageObjectManageServiceImpl();
    virtual ~IMessageObjectManageServiceImpl();

public:

    virtual QObject* GetMessageObject(const QString& messageObjectName) override;

    virtual bool AddMessageObject(const QString& messageObjectName, QObject* messageObject) override;

    virtual bool RemoveMessageObject(const QString& messageObjectName) override;
    virtual bool RemoveMessageObject(QObject* messageObject) override;

    virtual bool AddMessageObjectLinster(QObject* linsterObject, std::function<void (const QString& messageObjectName, QObject* messageObject)> fun) override;

private:
    void UpdateMessageObjectLinster(const QString &messageObjectName, QObject *messageObject);

private:
    QMap<QString , QObject*>                                                                           m_MessageObjectMap;
    QMap<QObject* , std::function<void (const QString& messageObjectName, QObject* messageObject)>>    m_LinsterObjectMap;
};

}
}

#endif
