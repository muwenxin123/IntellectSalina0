#ifndef IUAVFLYMANAGE_DJI_TAKEOFFTOPOINT_H
#define IUAVFLYMANAGE_DJI_TAKEOFFTOPOINT_H

#include <QWidget>

namespace Ui {
class IUAVFlyManage_DJI_TakeoffToPoint;
}

class IUAVFlyManage_DJI_TakeoffToPoint : public QWidget
{
    Q_OBJECT

public:
    explicit IUAVFlyManage_DJI_TakeoffToPoint(QWidget *parent = nullptr);
    ~IUAVFlyManage_DJI_TakeoffToPoint();

private:
    Ui::IUAVFlyManage_DJI_TakeoffToPoint *ui;
};

#endif // IUAVFLYMANAGE_DJI_TAKEOFFTOPOINT_H
