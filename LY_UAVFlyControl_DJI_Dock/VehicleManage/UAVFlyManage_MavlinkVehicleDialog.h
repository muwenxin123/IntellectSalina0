#ifndef UAVFLYMANAGE_MAVLINKVEHICLEDIALOG_H
#define UAVFLYMANAGE_MAVLINKVEHICLEDIALOG_H

#include <QWidget>

namespace Ui {
class UAVFlyManage_MavlinkVehicleDialog;
}

class IProj;
class Vehicle;

class UAVFlyManage_MavlinkVehicleControlsDialog;
class UAVFlyManage_MavlinkVehicleStatusInfoDialog;
class UAVFlyManage_MavlinkParameterManageEditDialog;

class UAVFlyManage_MavlinkVehicleDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkVehicleDialog(QWidget *parent = nullptr, Vehicle *vehicle = nullptr, int vehicleId = -1);
    ~UAVFlyManage_MavlinkVehicleDialog() override;

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
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private:
    void InitVehicleInfoList();

private:
    Ui::UAVFlyManage_MavlinkVehicleDialog             *ui;
    bool                                               m_Show;

    Vehicle*                                           m_vehicle;
    int                                                m_vehicleId;
    bool                                               m_Selected;
    bool                                               m_needOnMouseDoubleClickEvent;
    UAVFlyManage_MavlinkVehicleControlsDialog*         m_pUAVFlyManage_MavlinkVehicleControlsDialog;
    UAVFlyManage_MavlinkVehicleStatusInfoDialog*       m_pUAVFlyManage_MavlinkVehicleStatusInfoDialog;
    UAVFlyManage_MavlinkParameterManageEditDialog*     m_pUAVFlyManage_MavlinkParameterManageEditDialog;
};

#endif
