#ifndef MAVLINKMISSIONMANAGERMISSIONCOMMANDLIST_H
#define MAVLINKMISSIONMANAGERMISSIONCOMMANDLIST_H

#include <QObject>

#include "QGCMAVLink.h"
#include "QGCLoggingCategory.h"

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonValue>

class MissionCommandUIInfo;

class MavlinkMissionManagerMissionCommandList : public QObject
{
    Q_OBJECT

public:

    MavlinkMissionManagerMissionCommandList(const QString& jsonFilename, bool baseCommandList, QObject* parent = nullptr);

    QStringList& categories(void) { return _categories; }

    MissionCommandUIInfo* getUIInfo(MAV_CMD command) const;

    const QList<MAV_CMD>& commandIds(void) const { return _ids; }

    static const char* qgcFileType;

private:
    void _loadMavCmdInfoJson(const QString& jsonFilename, bool baseCommandList);

    QMap<MAV_CMD, MissionCommandUIInfo*>    _infoMap;
    QList<MAV_CMD>                          _ids;
    QStringList                             _categories;

    static const char* _versionJsonKey;
    static const char* _mavCmdInfoJsonKey;
};

#endif
