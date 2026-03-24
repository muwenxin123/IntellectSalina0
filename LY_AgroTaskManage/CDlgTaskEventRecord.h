#ifndef CDLGTASKEVENTRECORD_H
#define CDLGTASKEVENTRECORD_H

#include <QWidget>
#include "LyDialog.h"
#include "IAgroTaskService.h"

namespace Ui {
class CDlgTaskEventRecord;
}

class CDlgTaskEventRecord : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgTaskEventRecord(QWidget *parent = nullptr);
    ~CDlgTaskEventRecord();

	void initTableData();

public slots:
	//À—À˜
	void on_pushButton_clicked();

	//÷ÿ÷√
	void on_pushButton_2_clicked();

	//πÿ±’
	void on_pushButton_3_clicked();

private:
    Ui::CDlgTaskEventRecord *ui;

	qnzkna::AgroTask::IAgroTaskService *m_pIAgroTaskService;

};

#endif // CDLGTASKEVENTRECORD_H
