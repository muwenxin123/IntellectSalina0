#ifndef CDLGAGROTASKMANAGE_H
#define CDLGAGROTASKMANAGE_H

#include <QWidget>
#include "LyDialog.h"
#include "IAgroTaskService.h"
#include "Zone\AgroTaskInfo.h"

namespace Ui {
class CDlgAgroTaskManage;
}

class CDlgAgroTaskManage : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgAgroTaskManage(QWidget *parent = nullptr);
    ~CDlgAgroTaskManage();

	void initTableData();

public slots:
	//搜索
	void on_pushButton_clicked();

	//重置
	void on_pushButton_2_clicked();

	//创建
	void on_pushButton_4_clicked();
	void slot_add_info(qnzkna::AgroTask::FarmlandAttributeInfo info);

	//更新
	void on_pushButton_5_clicked();

	//删除
	void on_pushButton_6_clicked();

	//关闭
	void on_pushButton_3_clicked();

private:
    Ui::CDlgAgroTaskManage *ui;

	qnzkna::AgroTask::IAgroTaskService *m_pIAgroTaskService;

	int m_tableRowCount = 6;

};

#endif // CDLGAGROTASKMANAGE_H
