#include "MavlinkMissionManagerMissionCommandList.h"

#include "JsonHelper.h"
#include "MissionCommandUIInfo.h"

#include <QStringList>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>
#include <QFile>

const char* MavlinkMissionManagerMissionCommandList::qgcFileType =           "MavCmdInfo";
const char* MavlinkMissionManagerMissionCommandList::_versionJsonKey =       "version";
const char* MavlinkMissionManagerMissionCommandList::_mavCmdInfoJsonKey =    "mavCmdInfo";

MavlinkMissionManagerMissionCommandList::MavlinkMissionManagerMissionCommandList(const QString& jsonFilename, bool baseCommandList, QObject* parent)
    : QObject(parent)
{
    _loadMavCmdInfoJson(jsonFilename, baseCommandList);
}

void MavlinkMissionManagerMissionCommandList::_loadMavCmdInfoJson(const QString& jsonFilename, bool baseCommandList)
{
    if (jsonFilename.isEmpty()) {
        return;
    }

    qCDebug(MissionCommandsLog) << "Loading" << jsonFilename;

    QString errorString;
    int version;
    QJsonObject jsonObject = JsonHelper::openInternalQGCJsonFile(jsonFilename, qgcFileType, 1, 1, version, errorString);
    if (!errorString.isEmpty()) {
        qWarning() << "Internal Error: " << errorString;
        return;
    }

    QJsonValue jsonValue = jsonObject.value(_mavCmdInfoJsonKey);
    if (!jsonValue.isArray()) {
        qWarning() << jsonFilename << "mavCmdInfo not array";
        return;
    }

    QJsonArray jsonArray = jsonValue.toArray();
    for(QJsonValue info: jsonArray) {
        if (!info.isObject()) {
            qWarning() << jsonFilename << "mavCmdArray should contain objects";
            return;
        }

        MissionCommandUIInfo* uiInfo = new MissionCommandUIInfo(this);

        QString errorString;
        if (!uiInfo->loadJsonInfo(info.toObject(), baseCommandList, errorString)) {
            uiInfo->deleteLater();
            qWarning() << jsonFilename << errorString;
            return;
        }

        QString newCategory = uiInfo->category();
        if (!newCategory.isEmpty() && !_categories.contains(newCategory)) {
            _categories.append(newCategory);
        }

        _infoMap[uiInfo->command()] = uiInfo;
    }

    for (MAV_CMD id: _infoMap.keys()) {
        _ids << id;
    }
}

MissionCommandUIInfo* MavlinkMissionManagerMissionCommandList::getUIInfo(MAV_CMD command) const
{
    if (!_infoMap.contains(command)) {
        return nullptr;
    }

    return _infoMap[command];
}
