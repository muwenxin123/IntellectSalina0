#ifndef UAVAREACHOOSEDIALOG_H
#define UAVAREACHOOSEDIALOG_H

#include <QWidget>
#include "LyDialog.h"
#include "IZoneManageService.h"

namespace Ui {
class UAVAreaChooseDialog;
}

class UAVAreaChooseDialog : public LyDialog
{
    Q_OBJECT

public:
    explicit UAVAreaChooseDialog(QWidget *parent = nullptr);
	~UAVAreaChooseDialog();
	void init();
	qnzkna::ZoneManage::IZoneManageService* m_pIZoneManageService;

private slots:
    void on_pbtn_OK_clicked();

    void on_pbtn_Cancel_clicked();

private:
    Ui::UAVAreaChooseDialog *ui;
};

#endif
