#ifndef UAVAGRIVIDEODIALOGLIST_H
#define UAVAGRIVIDEODIALOGLIST_H

#include <QWidget>
#include "camerawidget.h"
namespace Ui {
class UAVAgriVideoDialogList;
}

class UAVAgriVideoDialogList : public QWidget
{
    Q_OBJECT

public:
    explicit UAVAgriVideoDialogList(QWidget *parent = nullptr);
    ~UAVAgriVideoDialogList();

	void InitDialog(std::vector<CameraWidget *> m_pList);

	bool getCurrentTab(int& index);

	void tabWidgetChanged();

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
    Ui::UAVAgriVideoDialogList *ui;

	std::vector<CameraWidget *> m_ViewCameraVec;

	bool												m_show;
};

#endif // UAVAGRIVIDEODIALOGLIST_H
