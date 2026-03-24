#ifndef UAVFLYMANAGE_MAVLINKVEHICLEGUIDEDMODEGOTOLOCATIONDIALOG_H
#define UAVFLYMANAGE_MAVLINKVEHICLEGUIDEDMODEGOTOLOCATIONDIALOG_H

#include <QWidget>

#include <functional>

#include "LyDialog.h"

namespace Ui {
class UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog;
}

class IProj;
class Vehicle;

class UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog() override;

    void DrawDynamic(QPainter &painter, IProj *pProj);
    void OnMouseButtonDown(QMouseEvent *e, IProj *pProj);
    void OnMouseButtonRelease(QMouseEvent *e, IProj *pProj);
    void OnMouseDoubleClick(QMouseEvent *e, IProj *pProj);
    void OnMouseMove(QMouseEvent *e, IProj *pProj);

signals:
    void needOnMouseDoubleClickEventSig(bool need);

protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private slots:
    void on_comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle_currentIndexChanged(const QString &arg1);

    void on_pushButton_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_ChangeAltitude_Altitude_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_MapGetPositions_clicked();

    void on_pushButton_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_FlyGoto_clicked();

private:
    struct Vehicle_GuidedMode_GotoLocation{
        Vehicle* vehicle;
        double   longitude_GuidedMode_GotoLocation;
        double   latitude_GuidedMode_GotoLocation;
        double   altitude_GuidedMode_GotoLocation;
    };

private:
    void InitWeaponInfoList();
    void InitWeaponInfoListAdded(Vehicle* vehicleAdded);
    void InitWeaponInfoListRemoved(Vehicle* vehicleRemoved);
    Vehicle *GetMainVehicle() const;
    bool GetListVehicle();
    void DisplayListVehicleInfo();
    void SendFlyCmdFun(std::function<void (Vehicle* vehicle)>& vehicleFun);

private:
    Ui::UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog *ui;
    bool                                                         m_Show;
    bool                                                         m_needOnMouseDoubleClickEvent;
    bool                                                         m_MapGetPosition;
    double                                                       m_Longitude_GuidedMode_GotoLocation;
    double                                                       m_Latitude_GuidedMode_GotoLocation;
    double                                                       m_Altitude_GuidedMode_GotoLocation;
    QList<Vehicle_GuidedMode_GotoLocation>                       m_Vehicle_GuidedMode_GotoLocationList;
};

#endif
