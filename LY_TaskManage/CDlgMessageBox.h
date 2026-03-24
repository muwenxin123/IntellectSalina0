#pragma once

#include <QWidget>
#include "ui_CDlgMessageBox.h"
#include "LyDialog.h"
class CDlgMessageBox : public LyDialog
{
	Q_OBJECT

public:
	explicit CDlgMessageBox(QWidget *parent = Q_NULLPTR);
	~CDlgMessageBox();

	CDlgMessageBox(const CDlgMessageBox&) = delete;
	CDlgMessageBox& operator=(const CDlgMessageBox&) = delete;
	static CDlgMessageBox* single;
private slots:
void on_btnok_clicked();
public:
	static CDlgMessageBox* GetInit()
	{
		if (single == nullptr)
		{
			single = new CDlgMessageBox();
			single->show();
		}
		return single;
	}
private:

	Ui::CDlgMessageBox* ui;
};
