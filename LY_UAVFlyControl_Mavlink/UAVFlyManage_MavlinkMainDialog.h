#ifndef UAVFLYMANAGE_MAVLINKMAINDIALOG_H
#define UAVFLYMANAGE_MAVLINKMAINDIALOG_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class UAVFlyManage_MavlinkMainDialog;
}

class IProj;
class UAVFlyManage_MavlinkApplicationSetting;
class UAVFlyManage_MavlinkVehicleManageDialog;

class UAVFlyManage_MavlinkMainDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkMainDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_MavlinkMainDialog() override;

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
    void on_listWidget_UAVFlyManage_MavlinkMainDialog_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::UAVFlyManage_MavlinkMainDialog               *ui;
    UAVFlyManage_MavlinkApplicationSetting*           m_pUAVFlyManage_MavlinkApplicationSetting;
    UAVFlyManage_MavlinkVehicleManageDialog*          m_pUAVFlyManage_MavlinkVehicleManageDialog;
};

#endif
