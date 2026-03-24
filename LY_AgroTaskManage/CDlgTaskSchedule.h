#ifndef CDLGTASKSCHEDULE_H
#define CDLGTASKSCHEDULE_H

#include <QWidget>
#include "LyDialog.h"
#include "IAgroTaskService.h"
#include "AgriFarm\AgriFarmManagerDecl.h"
#include "AgriFarm\AgriSprayPrescriptDecl.h"

namespace Ui {
class CDlgTaskSchedule;
}

class CDlgTaskSchedule : public  LyDialog
{
    Q_OBJECT

public:
    explicit CDlgTaskSchedule(QWidget *parent = nullptr);
    ~CDlgTaskSchedule();

public:

	void InitCtrl();

	void UpdateNongTianCombobox();

	void UpdateDrawFrame(const int& nComboboxCurrentIndex);

private slots:

	//盐田Combobox
    void on_comboBox_currentIndexChanged(const int &arg1);

	//显示方式Combobox
    void on_comboBox_2_currentIndexChanged(const int &arg1);

	//鼠标提示checkBox
    void on_checkBox_2_stateChanged(int arg1);

	//显示细节checkBox
    void on_checkBox_stateChanged(int arg1);

	//还原
    void on_pushButton_clicked();

	//退出
    void on_pushButton_2_clicked();

	void on_pushButton_3_clicked();

	void on_pushButton_4_clicked();

	void on_pushButton_5_clicked();

private:

	void showEvent(QShowEvent *event);

private:

    Ui::CDlgTaskSchedule*																ui;

	qnzkna::AgroTask::IAgroTaskService*											m_pIAgroTaskService = nullptr;

	qnzkna::AgroTask::CFarmlandAttributeInfoVec							m_FarmInfoVec;

	//盐田列表
	AgriFieldInfoVec																			m_vecFieldList;

	AgriFieldInfo																				m_FieldInfo;

};

#endif // CDLGTASKSCHEDULE_H
