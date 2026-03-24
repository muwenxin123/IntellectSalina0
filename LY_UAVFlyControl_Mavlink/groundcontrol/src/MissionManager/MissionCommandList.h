#ifndef MISSIONCOMMANDLIST_H
#define MISSIONCOMMANDLIST_H

#include <QObject>

#include "QGCMAVLink.h"
#include "QGCLoggingCategory.h"

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonValue>

class MissionCommandUIInfo;

/// Maintains a list of MissionCommandUIInfo objects loaded from a json file.
class MissionCommandList : public QObject
{
    Q_OBJECT

public:
    /// @param baseCommandList true: bottomost level of mission command hierarchy (partial spec allowed), false: override level of hierarchy
    MissionCommandList(const QString& jsonFilename, bool baseCommandList, QObject* parent = nullptr);

    /// Returns list of categories in this list
    QStringList& categories(void) { return _categories; }

    /// Returns the ui info for specified command, NULL if command not found
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

#endif // MISSIONCOMMANDLIST_H
