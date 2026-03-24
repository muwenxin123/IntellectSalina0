#ifndef UAVFLYMANAGE_MAVLINKVEHICLECONTROLSDIALOG_H
#define UAVFLYMANAGE_MAVLINKVEHICLECONTROLSDIALOG_H

#include <QWidget>
#include <QEvent>

namespace Ui {
class UAVFlyManage_MavlinkVehicleControlsDialog;
}

class IProj;
class Vehicle;

class UAVFlyManage_MavlinkVehicleControlsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkVehicleControlsDialog(QWidget *parent = nullptr, Vehicle *vehicle = nullptr, int vehicleId = -1);
    ~UAVFlyManage_MavlinkVehicleControlsDialog() override;

    Vehicle *getVehicle() const;

    QString getVehicleInfo() const;

    bool getSelected() const;
    void setSelected(bool Selected);

    void DrawDynamic(QPainter &painter, IProj *pProj);
    void OnMouseButtonDown(QMouseEvent *e, IProj *pProj);
    void OnMouseButtonRelease(QMouseEvent *e, IProj *pProj);
    void OnMouseDoubleClick(QMouseEvent *e, IProj *pProj);
    void OnMouseMove(QMouseEvent *e, IProj *pProj);

signals:
    void needOnMouseDoubleClickEventSig(bool need);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private slots:
    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_SetMode_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_SetWaypoint_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Mission_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Manual_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_RTL_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Land_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Arm_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Disarm_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_EmergencyStop_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_ChangeAltitude_Altitude_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_MapGetPositions_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_FlyGoto_clicked();

private:
    void MavlinkVehicleConnect();
    void MavlinkVehicleDisconnect();

private:
    void InitVehicleControls();
    void InitVehicleControlsWayPoints();

private:
    Ui::UAVFlyManage_MavlinkVehicleControlsDialog     *ui;

    bool                                               m_Show;

    Vehicle*                                           m_vehicle;
    int                                                m_vehicleId;
    bool                                               m_Selected;
    bool                                               m_needOnMouseDoubleClickEvent;
    bool                                               m_MapGetPosition;
    unsigned int                                       m_Color;
    double                                             m_Longitude_GuidedMode_GotoLocation;
    double                                             m_Latitude_GuidedMode_GotoLocation;
    double                                             m_Altitude_GuidedMode_GotoLocation;

};

#endif
