#ifndef LY_AGRISPRAYPRESCOMMANDWIDGET_H
#define LY_AGRISPRAYPRESCOMMANDWIDGET_H

#include <QWidget>

namespace Ui {
class LY_AgriSprayPresCommandWidget;
}

class LY_AgriSprayPresCommandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_AgriSprayPresCommandWidget(QWidget *parent = nullptr);
    ~LY_AgriSprayPresCommandWidget();

private:
    Ui::LY_AgriSprayPresCommandWidget *ui;
};

#endif // LY_AGRISPRAYPRESCOMMANDWIDGET_H
