#ifndef UAVFLYMANAGE_DJITASKMANAGEDIALOG_H
#define UAVFLYMANAGE_DJITASKMANAGEDIALOG_H

#include <QWidget>

namespace Ui {
class UAVFlyManage_DJITaskManageDialog;
}

class UAVFlyManage_DJITaskManageDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_DJITaskManageDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_DJITaskManageDialog();


protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private slots:
    void on_tabWidget_UAVFlyManage_DJITaskManage_Vehicles_customContextMenuRequested(const QPoint &pos);

    void on_comboBox_UAVFlyManage_DJITaskManage_Task_currentTextChanged(const QString &arg1);

    void on_comboBox_UAVFlyManage_DJITaskManage_Vehicles_currentTextChanged(const QString &arg1);

    void on_pushButton_UAVFlyManage_DJITaskManage_VehiclesHideAll_clicked();

    void on_pushButton_UAVFlyManage_DJITaskManage_VehiclesRemoveAll_clicked();

    void on_pushButton_UAVFlyManage_DJITaskManage_UploadAllWayPoints_clicked();

    void on_pushButton_UAVFlyManage_DJITaskManage_DownloadAllWayPoints_clicked();

    void on_pushButton_UAVFlyManage_DJITaskManage_SaveAllPoint_clicked();

    void on_pushButton_UAVFlyManage_DJITaskManage_Vehicles_Save_clicked();

    void on_pushButton_UAVFlyManage_DJITaskManage_Vehicles_Open_clicked();

private:
    Ui::UAVFlyManage_DJITaskManageDialog *ui;
};

#endif // UAVFLYMANAGE_DJITASKMANAGEDIALOG_H
