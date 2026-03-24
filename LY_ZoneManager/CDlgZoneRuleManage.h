#ifndef CDLGZONERULEMANAGE_H
#define CDLGZONERULEMANAGE_H

#include <QWidget>
#include "LyDialog.h"
#include "IZoneManageService.h"

namespace Ui {
class CDlgZoneRuleManage;
}

class CDlgZoneRuleManage : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgZoneRuleManage(QWidget *parent = nullptr);
    ~CDlgZoneRuleManage();

public:
	void InitData();

	void UpdateRuleData();

	void CreateListHeader();

	void UpdatelistData();

	void UpdateTablewidget(int row,std::vector<QString>selectRule);

private slots:

    void on_pushButton_clicked();

	void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

	void on_tableWidget_doubleclicked();

private:

	void showEvent(QShowEvent *event);

private:

    Ui::CDlgZoneRuleManage*									ui;

	qnzkna::ZoneManage::IZoneManageService*					m_pIZoneManageService = nullptr;

};

#endif
