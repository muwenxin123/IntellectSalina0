#ifndef UAVFLYMANAGE_MAVLINKTASKMANAGEDIALOG_H
#define UAVFLYMANAGE_MAVLINKTASKMANAGEDIALOG_H

#include <QMenu>
#include <QWidget>
#include "IUAVFlyControl_MavlinkActivator.h"
#include <QTableWidget>

#include "LyDialog.h"

namespace Ui {
class UAVFlyManage_MavlinkTaskManageDialog;
}

class UAVFlyManage_MavlinkMissionManagerDialog;
class IProj;

class FactMetaData;
class Vehicle;

class UAVFlyManage_MavlinkTaskManageDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkTaskManageDialog(QWidget *parent = nullptr);
    virtual ~UAVFlyManage_MavlinkTaskManageDialog() override;

	void switchTocurrentTask(const QString& strTask);

    void InitWeaponInfoList();
    void InitWeaponInfoListNoTask();
    void InitWeaponInfoListAdded(Vehicle* vehicleAdded);
    void InitWeaponInfoListRemoved(Vehicle* vehicleRemoved);
	void InitTaskListInfo();

    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e, IProj *pProj);
    bool OnMouseButtonRelease(QMouseEvent *e, IProj *pProj);
    bool OnMouseDoubleClick(QMouseEvent *e, IProj *pProj);
    bool OnMouseMove(QMouseEvent *e, IProj *pProj);

	void changeLineUpWidgetToStr_like(QString strName);

    typedef enum _TaskManageLineEditStates
    {
        TaskManageLinePickNone         = 0,
        TaskManageLinePickNormal       = 1,
        TaskManageLinePickSinglePoint,
        TaskManageLinePickLinePoint
    }TaskManageLineEditStates;

	bool GetUAVLinkMissionManager();
	void GetUAVLinkMissionHide();
	void AutoScriptToRoute();
	void AutoScriptToUpDown();
protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private slots:
    void on_tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles_customContextMenuRequested(const QPoint &pos);

    void on_tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles_currentChanged(int index);

    void on_comboBox_UAVFlyManage_MavlinkTaskManage_Task_currentChanged(const QString& arg1);

    void on_comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles_currentIndexChanged(const QString &arg1);

    void on_pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Save_clicked();

    void on_pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Open_clicked();

    void on_pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll_clicked();

    void on_pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesRemoveAll_clicked();

    void on_pushButton_UAVFlyManage_MavlinkTaskManage_UploadAllWayPoints_clicked();

    void on_pushButton_UAVFlyManage_MavlinkTaskManage_DownloadAllWayPoints_clicked();

	void on_pushButton_UAVFlyManage_MavlinkTaskManage_SaveAllPoint_clicked();

private:
    void startMissionPlan();
    void stopMissionPlan();

    void DrawWayPointImage(QPainter &painter, const QPoint &pos, const bool Selected = false);
private:

    Ui::UAVFlyManage_MavlinkTaskManageDialog          *ui;
    TaskManageLineEditStates                           m_TaskManageLineEditStates;
    QMenu                                             *m_TabWidgetMenu;

	QAction*										   m_actionMenuStart;
	QAction*										   m_actionMenuUpdate;
	QAction*										   m_actionMenuUpdown;
    QList<UAVFlyManage_MavlinkMissionManagerDialog*>   m_UAVFlyManage_MavlinkMissionManagerDialogList;

    double                                             m_defaultAltitude;
    unsigned int                                       m_Color[25];
    QPixmap                                            m_WayPointImage;
    QPixmap                                            m_WayPointImageSelected;

    std::map<long long, QString>                       m_MapVehicle;

	bool											   b_CloseDialog = false;

};

#endif
