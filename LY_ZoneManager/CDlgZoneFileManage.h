#ifndef CDLGZONEFILEMANAGE_H
#define CDLGZONEFILEMANAGE_H

#include <QWidget>
#include "LyDialog.h"
#include "IZoneManageService.h"

namespace Ui {
class CDlgZoneFileManage;
}

class CDlgZoneFileManage : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgZoneFileManage(QWidget *parent = nullptr);
    ~CDlgZoneFileManage();

public:

	void CreateListHeader();

	void UpdatelistData();

private slots:

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:

	void showEvent(QShowEvent *event);

private:

	Ui::CDlgZoneFileManage*									ui;

	qnzkna::ZoneManage::IZoneManageService*					m_pIZoneManageService = nullptr;

};

#endif
