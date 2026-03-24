#ifndef MISSIONLOADDIALOG_H
#define MISSIONLOADDIALOG_H

#include <QWidget>
#include "LyDialog.h"
#include "IZoneManageService.h"

namespace Ui {
class MissionLoadDialog;
}

class MissionLoadDialog : public LyDialog
{
    Q_OBJECT

public:
    explicit MissionLoadDialog(QWidget *parent = nullptr);
    ~MissionLoadDialog();
	void init(QString missionName = "");
	QString m_missionName;
	Ui::MissionLoadDialog *ui;
	qnzkna::ZoneManage::IZoneManageService* m_pIZoneManageService;
private slots:
	void on_pbtn_OK_clicked();

	void on_pbtn_Cancel_clicked();
};

#endif
