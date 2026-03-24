#ifndef UAVARMSHOTALONELISTDIALOG_H
#define UAVARMSHOTALONELISTDIALOG_H

#include <QWidget>
#include "LyDialog.h"
#include "IUAVNavigationArmShotAlone54Dialog.h"
namespace Ui {
class UAVArmShotAloneListDialog;
}

class UAVArmShotAloneListDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVArmShotAloneListDialog(QWidget *parent = nullptr);
    ~UAVArmShotAloneListDialog();

	void InitDialog(std::list<IUAVNavigationArmShotAlone54Dialog *> m_pList);

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);
private:
    Ui::UAVArmShotAloneListDialog *ui;
	bool												m_show;

};

#endif // UAVARMSHOTALONELISTDIALOG_H
