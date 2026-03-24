#ifndef LY_AGRICULHOMEPAGEWIDGET_H
#define LY_AGRICULHOMEPAGEWIDGET_H

#include <QWidget>

namespace Ui {
class LY_AgriculHomePageWidget;
}

class LY_AgriculHomePageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_AgriculHomePageWidget(QWidget *parent = nullptr);
    ~LY_AgriculHomePageWidget();

private:
    Ui::LY_AgriculHomePageWidget *ui;
};

#endif // LY_AGRICULHOMEPAGEWIDGET_H
