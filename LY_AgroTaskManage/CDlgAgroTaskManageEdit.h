#ifndef CDLGAGROTASKMANAGEEDIT_H
#define CDLGAGROTASKMANAGEEDIT_H

#include <QWidget>
#include "LyDialog.h"
#include "IAgroTaskService.h"
#include "Zone\AgroTaskInfo.h"
#include "IZoneManageService.h"

namespace Ui {
class CDlgAgroTaskManageEdit;
}

class CDlgAgroTaskManageEdit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgAgroTaskManageEdit(QWidget *parent = nullptr);
    ~CDlgAgroTaskManageEdit();

public slots:
	void on_pushButton_clicked();
	void on_tablewidget_doubleclicked();
	void on_tablewidget_2_doubleclicked();

signals:
	void signalFarmlandAttributeInfo(qnzkna::AgroTask::FarmlandAttributeInfo info);

private:
	void initUI();

private:
    Ui::CDlgAgroTaskManageEdit *ui;

	qnzkna::AgroTask::IAgroTaskService *m_pIAgroTaskService;

	qnzkna::ZoneManage::IZoneManageService *m_pIZoneManageService;

};

#endif // CDLGAGROTASKMANAGEEDIT_H
