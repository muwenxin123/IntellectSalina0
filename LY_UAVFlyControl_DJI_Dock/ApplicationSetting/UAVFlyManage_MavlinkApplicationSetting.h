#ifndef UAVFLYMANAGE_MAVLINKAPPLICATIONSETTING_H
#define UAVFLYMANAGE_MAVLINKAPPLICATIONSETTING_H

#include <QWidget>

namespace Ui {
class UAVFlyManage_MavlinkApplicationSetting;
}

class UAVFlyManage_MavlinkApplicationSetting : public QWidget
{
    Q_OBJECT

public:
    explicit UAVFlyManage_MavlinkApplicationSetting(QWidget *parent = nullptr);
    ~UAVFlyManage_MavlinkApplicationSetting() override;

protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private:
    Ui::UAVFlyManage_MavlinkApplicationSetting *ui;
};

#endif
