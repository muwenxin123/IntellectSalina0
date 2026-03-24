#ifndef ISALTDETAILEDINFORMATION_H
#define ISALTDETAILEDINFORMATION_H

#include <QWidget>
#include "LyDialog.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
namespace Ui {
class ISaltDetailedInformation;
}

class ISaltDetailedInformation : public LyDialog
{
    Q_OBJECT

public:
    explicit ISaltDetailedInformation(QWidget *parent = nullptr);
    ~ISaltDetailedInformation();

	void Init(double width = 640,double height = 640);

	void resizeEvent(QResizeEvent * event) override;

	void LeftButtonDown(double x,double y);

	void RightButtonDown(double x, double y);

	void MouseRolling(double multiple);

protected:
	void SetGraphicsSceneUI();
private:
    Ui::ISaltDetailedInformation *ui;

private:
	QGraphicsScene *scene;
	QGraphicsPixmapItem *backgroundPixmapItem;
	QAction *openAction;
	QAction *zoomInAction;
	QAction *zoomOutAction;
	QAction *resetAction;
	QPixmap currentPixmap;
	double currentScale;

};

#endif // ISALTDETAILEDINFORMATION_H
