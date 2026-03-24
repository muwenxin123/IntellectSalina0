#ifndef LY_UAVFLYLINEMARK_H
#define LY_UAVFLYLINEMARK_H

#include <QWidget>

namespace Ui {
class LY_UAVFlyLineMark;
}

class LY_UAVFlyLineMark : public QWidget
{
    Q_OBJECT

public:
    explicit LY_UAVFlyLineMark(QWidget *parent = nullptr);
    ~LY_UAVFlyLineMark();

private:
    Ui::LY_UAVFlyLineMark *ui;
};

#endif // LY_UAVFLYLINEMARK_H
