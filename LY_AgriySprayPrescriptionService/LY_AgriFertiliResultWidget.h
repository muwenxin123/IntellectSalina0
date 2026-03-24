#ifndef LY_AGRIFERTILIRESULTWIDGET_H
#define LY_AGRIFERTILIRESULTWIDGET_H

#include <QWidget>
#include <QMouseEvent>
namespace Ui {
class LY_AgriFertiliResultWidget;
}

class LY_AgriFertiliResultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_AgriFertiliResultWidget(QWidget *parent = nullptr);
    ~LY_AgriFertiliResultWidget();

protected:

//	virtual void hideEvent(QHideEvent *event);
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::LY_AgriFertiliResultWidget *ui;
	QPoint m_dragPosition;
	bool m_dragging = false;
};

#endif // LY_AGRIFERTILIRESULTWIDGET_H
