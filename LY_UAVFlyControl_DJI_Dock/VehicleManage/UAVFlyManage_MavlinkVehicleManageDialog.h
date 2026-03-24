#ifndef UAVFLYMANAGE_MAVLINKVEHICLEMANAGEDIALOG_H
#define UAVFLYMANAGE_MAVLINKVEHICLEMANAGEDIALOG_H

#include <QWidget>

namespace Ui {
class UAVFlyManage_MavlinkVehicleManageDialog;
}

class IProj;

class UAVFlyManage_MavlinkVehicleDialog;

class Vehicle;

class UAVFlyManage_MavlinkVehicleManageDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkVehicleManageDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_MavlinkVehicleManageDialog() override;

    void DrawDynamic(QPainter &painter, IProj *pProj);
    void OnMouseButtonDown(QMouseEvent *e, IProj *pProj);
    void OnMouseButtonRelease(QMouseEvent *e, IProj *pProj);
    void OnMouseDoubleClick(QMouseEvent *e, IProj *pProj);
    void OnMouseMove(QMouseEvent *e, IProj *pProj);

protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private slots:
    void on_tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles_currentChanged(int index);

    void on_comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles_currentIndexChanged(const QString &arg1);

private:
    void InitWeaponInfoList();
    void InitWeaponInfoListAdded(Vehicle* vehicleAdded);
    void InitWeaponInfoListRemoved(Vehicle* vehicleRemoved);

private:
    Ui::UAVFlyManage_MavlinkVehicleManageDialog            *ui;
    bool                                                    m_needOnMouseDoubleClickEvent;
    QList<UAVFlyManage_MavlinkVehicleDialog*>               m_UAVFlyManage_MavlinkVehicleDialogList;
};

#endif
