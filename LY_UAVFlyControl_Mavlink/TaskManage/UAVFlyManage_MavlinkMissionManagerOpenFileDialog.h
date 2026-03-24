#ifndef UAVFLYMANAGE_MAVLINKMISSIONMANAGEROPENFILEDIALOG_H
#define UAVFLYMANAGE_MAVLINKMISSIONMANAGEROPENFILEDIALOG_H

#include <LyDialog.h>

namespace Ui {
class UAVFlyManage_MavlinkMissionManagerOpenFileDialog;
}

class UAVFlyManage_MavlinkMissionManagerOpenFileDialog : public LyDialog
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkMissionManagerOpenFileDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_MavlinkMissionManagerOpenFileDialog();

    QString TaskFileDir() const;

protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private slots:
    void on_pushButton_MavlinkMissionManagerOpenFileDialog_Open_clicked();

private:
    Ui::UAVFlyManage_MavlinkMissionManagerOpenFileDialog *ui;
    const QString                                         m_TaskFileDir;
};

#endif
