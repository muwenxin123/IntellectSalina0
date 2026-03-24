#ifndef AGRILEGENDPOPUP_H
#define AGRILEGENDPOPUP_H

#include <QWidget>
#include <QWidget>
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include "IAgriFarmManagerActivator.h"
#include "DataManage/AgriFarm/AgriHomePageDecl.h"
#include <QPainter>

namespace Ui {
class AgriLegendPopup;
}

class AgriLegendPopup : public QWidget
{
    Q_OBJECT


public:
    explicit AgriLegendPopup(QWidget *parent = nullptr);
    ~AgriLegendPopup();

    void updateFieldInfo(AgriFieldInfo &info);

protected:
	// 方法2：重写paintEvent
	void paintEvent(QPaintEvent *event);

private:
    Ui::AgriLegendPopup *ui;
};

#endif // AGRILEGENDPOPUP_H
