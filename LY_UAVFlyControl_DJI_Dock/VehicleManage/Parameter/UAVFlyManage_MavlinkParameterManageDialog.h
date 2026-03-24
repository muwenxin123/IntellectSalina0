#ifndef UAVFLYMANAGE_MAVLINKPARAMETERMANAGEDIALOG_H
#define UAVFLYMANAGE_MAVLINKPARAMETERMANAGEDIALOG_H

#include <QWidget>

namespace Ui {
class UAVFlyManage_MavlinkParameterManageDialog;
}

class UAVFlyManage_MavlinkParameterManageEditDialog;

class UAVFlyManage_MavlinkParameterManageDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkParameterManageDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_MavlinkParameterManageDialog();

    void InitWeaponInfoList();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles_currentChanged(int index);

    void on_comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles_currentTextChanged(const QString &arg1);

    void on_pushButton_UAVFlyManage_MavlinkParameterManage_RefreshAllVehicleParameters_clicked();

private:
    Ui::UAVFlyManage_MavlinkParameterManageDialog          *ui;

    QList<UAVFlyManage_MavlinkParameterManageEditDialog*>  m_UAVFlyManage_MavlinkParameterManageEditDialogList;
};

#endif
