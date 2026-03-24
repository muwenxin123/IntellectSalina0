#ifndef UAVFLYMANAGE_MAVLINKPARAMETERMANAGEEDITDIALOG_H
#define UAVFLYMANAGE_MAVLINKPARAMETERMANAGEEDITDIALOG_H

#include <QTreeWidget>
#include <QWidget>

namespace Ui {
class UAVFlyManage_MavlinkParameterManageEditDialog;
}

class Vehicle;

class UAVFlyManage_MavlinkParameterManageEditDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkParameterManageEditDialog(QWidget *parent = nullptr, Vehicle *vehicle = nullptr, int vehicleId = -1);
    ~UAVFlyManage_MavlinkParameterManageEditDialog() override;

    Vehicle *getVehicle() const;

    QString getVehicleInfo() const;

    bool getSelected() const;
    void setSelected(bool Selected);

    void refreshAllParameters();

private slots:
    void _parametersReady                   (bool parametersReady);
    void _loadProgressChanged               (float value);

    void on_tableWidget_UAVFlyManage_MavlinkParameterManageEdit_cellChanged(int row, int column);

    void on_pushButton_UAVFlyManage_MavlinkParameterManageEdit_RefreshAllParameters_clicked();

    void on_pushButton_UAVFlyManage_MavlinkParameterManageEdit_RefreshParameters_clicked();

    void on_pushButton_UAVFlyManage_MavlinkParameterManageEdit_EditParameters_clicked();

    void on_treeWidget_UAVFlyManage_MavlinkParameterManageEdit_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_lineEdit_UAVFlyManage_MavlinkParameterManageEdit_SearthParameters_textChanged(const QString &arg1);

    void on_pushButton_UAVFlyManage_MavlinkParameterManageEdit_SearthParameters_clicked();

private:
    void MissionManagerTableWidgetDisconnect();
    void MissionManagerTableWidgetConnect();

    void UpdateParametersTableWidget();
    void SearthParametersTableWidget();

protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private:
    Ui::UAVFlyManage_MavlinkParameterManageEditDialog *ui;
    bool                                               m_Show;

    Vehicle*                                           m_vehicle;
    int                                                m_vehicleId;
    bool                                               m_refreshParameter;
    bool                                               m_Selected;
};

#endif
