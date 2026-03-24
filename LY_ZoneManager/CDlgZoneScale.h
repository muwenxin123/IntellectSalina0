#ifndef CDLGZONESCALE_H
#define CDLGZONESCALE_H

#include <QWidget>
#include "LyDialog.h"
#include "IZoneManageService.h"

namespace Ui {
class CDlgZoneScale;
}

class CDlgZoneScale : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgZoneScale(QWidget *parent = nullptr);
    ~CDlgZoneScale();

public:

	void InitCtrl();

	void LoadIni();

private:

	void showEvent(QShowEvent *event);

public slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::CDlgZoneScale*										ui;
	qnzkna::ZoneManage::IZoneManageService*					m_pIZoneManageService = nullptr;

};

#endif
