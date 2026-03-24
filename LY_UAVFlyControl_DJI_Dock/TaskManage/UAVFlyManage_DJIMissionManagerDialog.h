#ifndef UAVFLYMANAGE_DJIMISSIONMANAGERDIALOG_H
#define UAVFLYMANAGE_DJIMISSIONMANAGERDIALOG_H

#include <QWidget>

namespace Ui {
class UAVFlyManage_DJIMissionManagerDialog;
}

class UAVFlyManage_DJIMissionManagerDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_DJIMissionManagerDialog(QWidget *parent = nullptr);
    ~UAVFlyManage_DJIMissionManagerDialog();

private:
    Ui::UAVFlyManage_DJIMissionManagerDialog *ui;
};

#endif // UAVFLYMANAGE_DJIMISSIONMANAGERDIALOG_H
