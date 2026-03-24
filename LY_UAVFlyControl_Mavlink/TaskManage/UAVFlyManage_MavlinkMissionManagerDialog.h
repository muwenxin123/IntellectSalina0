#ifndef UAVFLYMANAGE_MAVLINKMISSIONMANAGERDIALOG_H
#define UAVFLYMANAGE_MAVLINKMISSIONMANAGERDIALOG_H

#include <QWidget>
#include <QList>
#include <QTableWidget>
#include <QPixmap>

#include "QGCMAVLink.h"
#include "SimpleMissionItem.h"

#include "map3d/IGraphics.h"
#include "map3d/IEarth.h"

namespace Ui {
class UAVFlyManage_MavlinkMissionManagerDialog;
}

class Vehicle;
class MissionItem;
class FactMetaData;
class TaskManageMissionCommandTree;
class MavlinkMissionManagerMissionCommandList;

class IProj;

typedef std::vector<IUserMesh1*>    IUserMesh1Vec;
typedef std::vector<ILabel*>        ILabelVec;

struct UAV3D
{
    std::string     strID;
    ARGB            colorLine;
    IUserMesh1Vec   pMesh1Vec;
    ILabelVec       pLabelVec;
};

typedef UAV3D      UAV3DList;

class UAVFlyManage_MavlinkMissionManagerDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkMissionManagerDialog(
            QWidget *parent = nullptr,
            Vehicle *vehicle = nullptr,
            int vehicleId = -1,
            unsigned int color = 0x00000000);

    ~UAVFlyManage_MavlinkMissionManagerDialog();

    void DrawWayPointImage(QPainter &painter, const QPoint& pos, const bool currentSelect, const bool Selected);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e, IProj *pProj);
    bool OnMouseButtonRelease(QMouseEvent *e, IProj *pProj);
    bool OnMouseDoubleClick(QMouseEvent *e, IProj *pProj);
    bool OnMouseMove(QMouseEvent *e, IProj *pProj);
    void Draw3DLinesWeaponID();
    void Clear3DlinesWeaponID();

    QList<MissionItem *> getMissionItems() const;
    void startMissionPlan();
    void stopMissionPlan();
    void stopMissionPlanNoStopLoadOrWriteVehicleMission();
    void clearMissionItems();
    void removeMissionItem(int index);
    void removeSelectedMissionItem();
    void appendMissionItem(MissionItem* missionItem);
    void insertMissionItem(int index, MissionItem* missionItem);
    void updateMissionItemFromVehicle();

    void appendSimpleMissionItemWayPoint(const double lat, const double lon, const double alt);
    void insertSimpleMissionItemWayPoint(int index, const double lat, const double lon, const double alt);
    void updateSimpleMissionItemWayPoint(int index);

	int GetProgressBarValue();
	void AutoScriptToClose();

    MissionItem* getSelecteMissionItem();

    Vehicle *getVehicle() const;

    QString getVehicleInfo() const;

    bool getSelected() const;
    void setSelected(bool Selected);

    unsigned int getColor() const;
    void setColor(unsigned int Color);

    bool MissionLineShow() const;
    void setMissionLineShow(bool newMissionLineShow);

    int getSequenceNumberSelected() const;
    void setSequenceNumberSelected(int sequenceNumberSelected);

    MAV_FRAME getMavFrameDefault() const;

    void loadFromVehicle            (void) ;
    void sendToVehicle              (void) ;
    void removeAll                  (void) ;
    void removeAllFromVehicle       (void) ;

    static const QString& getTaskFileDir();
    static const QString getCreateTaskFileDir();
    void saveToFile(const QString &filename, QString &errorString);
    void saveToTaskFileDir(const QString &taskFileDir, QString &errorString);
    QJsonDocument saveToJson();
    void save(QJsonObject &json);
    bool loadJsonFile(QFile &file, QString &errorString);
    void loadFromTaskFileDir(const QString &taskFileDir, QString &errorString);
    void loadFromFile(const QString &filename, QString &errorString);
    bool _loadItemsFromJson(const QJsonObject &json, QList<MissionItem*> *visualItems, QString &errorString);
    bool _loadJsonMissionFileV2(const QJsonObject &json, QList<MissionItem *> *visualItems, QString &errorString);
    bool _loadJsonMissionFileV1(const QJsonObject &json, QList<MissionItem *> *visualItems, QString &errorString);
    bool load(const QJsonObject &json, QString &errorString);

signals:
    void syncInProgressChanged              (bool syncInProgress);
    void progressPctChanged                 (double progressPct);
    void resumeMissionIndexChanged          (void);
    void resumeMissionReady                 (void);
    void resumeMissionUploadFail            (void);

protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private:
    struct SupportMissionMavCmd{
        MAV_CMD mav_cmd;
        QString mav_cmd_rawName;
        QString mav_cmd_friendlyName;
    };

    void MissionManagerTableWidgetDisconnect();
    void MissionManagerTableWidgetConnect();
    void MissionManagerTableWidgetUpdate();
    void appendMissionManagerTableWidget();
    void MissionManagerTableWidgetTableWidgetItem(QTableWidget *tableWidget, int row);
    void UpdateMissionManagerTableWidgetTableHorizontalHeader(int row);

    QList<SupportMissionMavCmd>& SupportMissionMavCmdList();
    bool HasSupportMissionMavCmd(MAV_CMD command);
    QString SupportMissionMavCmdFriendlyName(MAV_CMD command) const;
    QList<SimpleMissionItem::EnumInfo_s>& SupportMissionMavFrameInfoList();
    bool HasSupportMissionMavFrameInfo(MAV_FRAME frame);

    static void sendItemsToVehicle           (Vehicle* vehicle, QList<MissionItem*>* visualMissionItems);
    static bool _convertToMissionItems       (QList<MissionItem*>* visualMissionItems, QList<MissionItem*>& rgMissionItems, QObject* missionItemParent);

private slots:
    void on_checkBox_VehicleUpDownMissionHide_stateChanged(int arg1);

    void on_pushButton_VehicleUpDownMissionWayPointAltitudeRelative_clicked();

    void on_pushButton_VehicleUpDownMissionWayPointAltitude_clicked();

    void on_pushButton_VehicleUpDownMissionWayPointSpeed_clicked();

    void on_pushButton_VehicleUpDownMissionWayPointRadius_clicked();

    void on_pushButton_VehicleUpDownMissionWayPointHoldTime_clicked();

    void on_pushButton_VehicleUpDownMissionLoiterRadius_clicked();

    bool _syncInProgress                     (void) const;
    void _clearAndDeleteWriteMissionItems    ();
    void _newMissionItemsAvailableFromVehicle(bool removeAllRequested);
    void _managerSendComplete                (bool error);
    void _managerRemoveAllComplete           (bool error);
    void _inProgressChanged                  (bool inProgress);
    void _progressPctChanged                 (double progressPct);
    void _currentMissionIndexChanged         (int sequenceNumber);
    void _resumeMissionIndexChanged          (void);
    void _resumeMissionReady                 (void);
    void _resumeMissionUploadFail            (void);
    void _error                              (int errorCode, const QString& errorMsg);
    void _updateTips                         (const QString& tips);
    void _updateMissionItemTips              (const QString& tips);
    void _sendCurrentMissionItemIndex        (int sendIndex);
    void _recvCurrentMissionItemIndex        (int recvIndex);

    void _resetVehicleUpDownMission();
    bool _isStandaloneCoordinate(MissionItem* item) const;
    bool _specifiesCoordinate(const MissionItem *item) const;
    QString _commandDescription(const MissionItem *item) const;
    QString _commandToolTipDescription(const MissionItem *item) const;
    QString _commandName(const MissionItem *item) const;
    void _calcPrevWaypointValues(const MissionItem *currentItem, const MissionItem *prevItem, double* azimuth, double* distance, double* altDifference);
    void _recalcMissionFlightStatus();
    void _setVehicleUpDownMissionProgress(double progress);
    void _setVehicleUpDownMissionProgressInfo(const QString& progressInfo);

    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void MissionManager_tableWidget_cellChanged(int row, int column);

    void MissionManagerTableWidgetComboxChanged(int index);
    void MissionManagerTableWidgetEditCellChanged(int row, int column);

private:
    Ui::UAVFlyManage_MavlinkMissionManagerDialog *ui;
    MavlinkMissionManagerMissionCommandList      *m_MavlinkMissionManagerMissionCommandList;
    bool                                         m_MavlinkMissionManagerMissionCommandListEnable;
    Vehicle*                                     m_vehicle;
    int                                          m_vehicleId;
    bool                                         m_sendHomePositionToVehicle;
    QGeoCoordinate                               m_homePosition;
    bool                                         m_MissionLineShow;
    bool                                         m_Selected;
    unsigned int                                 m_Color;
    int                                          m_SequenceNumberSelected;
    MAV_FRAME                                    m_MavFrameDefault;
    QList<SupportMissionMavCmd>                  m_SupportMavCmdList;
    QList<SimpleMissionItem::EnumInfo_s>         m_SupportMavFrameInfoList;
    QList<MissionItem*>                          m_MissionItems;
    QPixmap                                      m_WayPointImage;
    QPixmap                                      m_WayPointImageSelected;
    QPixmap                                      m_WayPointImage32;
    QPixmap                                      m_WayPointImage32Selected;
    bool                                         m_FlightTaskManageEditLineElevation;
    UAV3DList                                    m_UAV3D;

    double              _progressPct           =        0;

    bool                _firstItemsFromVehicle =        false;
    bool                _itemsRequested        =        false;
    bool                _inRecalcSequence      =        false;
	bool				m_startLoad			   =		false;

    QGroundControlQmlGlobal::AltMode _globalAltMode = QGroundControlQmlGlobal::AltitudeModeRelative;

    static QString                               m_TaskFileDir;

    static const int    _kPlanFileVersion;
    static const char*  _kPlanFileType;
    static const char*  _kJsonMissionObjectKey;
    static const char*  _kJsonGeoFenceObjectKey;
    static const char*  _kJsonRallyPointsObjectKey;

    static const char*  _jsonFileTypeValue;
    static const char*  _jsonFirmwareTypeKey;
    static const char*  _jsonVehicleTypeKey;
    static const char*  _jsonCruiseSpeedKey;
    static const char*  _jsonHoverSpeedKey;
    static const char*  _jsonItemsKey;
    static const char*  _jsonPlannedHomePositionKey;
    static const char*  _jsonParamsKey;
    static const char*  _jsonGlobalPlanAltitudeModeKey;

    static const char*  _jsonMavAutopilotKey;
    static const char*  _jsonComplexItemsKey;

    static const int    _missionFileVersion;

    static ILabel *     _pLabelMap3D;
};

#endif
