#pragma once

#include <QWidget>
#include "ui_LY_IntellectImageShangBoDialog.h"
#include "LyDialog.h"
#include "Bag_ShangBoHeader.h"
namespace Ui
{
	class LY_IntellectImageShangBoDialog;
}
class LY_IntellectImageShangBoDialog : public LyDialog
{
	Q_OBJECT

public:
	explicit LY_IntellectImageShangBoDialog(QWidget *parent = Q_NULLPTR);
	~LY_IntellectImageShangBoDialog();

	void Init();

	private slots:
	void on_pbtnUp_clicked();
	void on_pbtnRet_clicked();
	void on_pbtndown_clicked();
	void on_pbtnLeft_clicked();
	void on_pbtnstop_clicked();
	void on_pbtncentre_clicked();
	void on_pbtnoffdyname_clicked();
private:
	Ui::LY_IntellectImageShangBoDialog* ui;
};
