#ifndef LY_AGRIFIELDINFOWIDGET_H
#define LY_AGRIFIELDINFOWIDGET_H

#include <QWidget>

namespace Ui {
class LY_AgriFieldInfoWidget;
}

class LY_AgriFieldInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_AgriFieldInfoWidget(QWidget *parent = nullptr);
    ~LY_AgriFieldInfoWidget();

private:
    Ui::LY_AgriFieldInfoWidget *ui;
};

#endif // LY_AGRIFIELDINFOWIDGET_H
