#ifndef LY_TASKHEADERWIDGET_H
#define LY_TASKHEADERWIDGET_H

#include <QWidget>

namespace Ui {
class LY_TaskHeaderWidget;
}

class LY_TaskHeaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_TaskHeaderWidget(QWidget *parent = nullptr);
    ~LY_TaskHeaderWidget();

	void  setTaskTypeName(const QString &name);
	void  setTaskProcess(const QString &name);
	void  setWorkArea(const QString &name);
	void  setFieldArea(const QString &name);

private:
    Ui::LY_TaskHeaderWidget *ui;
};

#endif // LY_TASKHEADERWIDGET_H
