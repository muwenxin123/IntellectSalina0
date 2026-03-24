#ifndef CDLGAGROMANUREPARASHOW_H
#define CDLGAGROMANUREPARASHOW_H

#include <QWidget>
#include "LyDialog.h"
#include "AgriFarm\AgriFarmManagerDecl.h"
#include "AgriFarm\AgriSprayPrescriptDecl.h"
#include "IProj.h"
#include "CDrawAxis.h"

//#include "ccommonstruct.h"

using namespace AgriSprayPrescriptionDecl;

namespace Ui {
class CDlgAgroManureParaShow;
}

class CDlgAgroManureParaShow : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgAgroManureParaShow(QWidget *parent = nullptr);
    ~CDlgAgroManureParaShow();

	void SetStringName(const QString& strTmp);

	void SetStringNYCS(const QString& strTmp);

	void InitUI();

	void InitCtrl();

	void SetAgriFieldInfo(const AgriFieldInfo& stInfo);

	void SetDrugRatioVec(const CDrugRatioVec& RatioVec);

	void draw2DPathLine(QPainter &painter, IProj *pProj);

	void localWeaponNameBindToId();

	void InitWeaponCombobox();

	//上传航线到服务
	void AddLineToServer(const std::vector<Poi>& LinePoints);

	//添加任务摘要
	void addTaskSummary();

	void SetWorkTime(const long long & llTime1, const long long & llTime2);

	void UpdateLineListDate();

	void GenerateLine();

protected:

	virtual void hideEvent(QHideEvent *event);

private:

	void GenerateSprayPrescriptionDiagram();

private slots:

	//关闭
    void on_pushButton_clicked();

	//显示/隐藏处方图
	void on_pushButton_4_clicked();

	//显示/隐藏航线
	void on_pushButton_3_clicked();

	//显示/隐藏指令
	void on_pushButton_6_clicked();

	//重新生成
	void on_pushButton_5_clicked();

	//航线上传
	void on_pushButton_2_clicked();

	//处方图Combobox
	void on_spray_name_comBox_2_currentIndexChanged(const int &arg1);

private:

    Ui::CDlgAgroManureParaShow *ui;

	AgriFieldInfo																				m_FieldInfo;

	//处方图列表
	AgriSprayPrescriptionDecl::AgriSprayPrescriptionVec				m_pescriptionVec;
	AgriSprayPrescriptionDecl::AgriSprayPrescriptionModel			m_pescription;

	//区域边界
	std::vector<QPointF>																	m_boundPoings;

	std::map<std::string, std::string>												m_SystemSysNameAndID;

	std::vector<Poi>																			m_flyPoints;

	//二维网格点
	QVector<QPoint>																		m_gridPoints;

	std::vector<AirInstructionPoi>													m_airInstructionPoints;

	//处方图显隐
	bool																								m_spayMapshow = false;

	//是否点击隐藏航线
	bool																								m_isGeneratingCourse = false;

	//是否显示指令状态
	bool																								m_isInstructionStatus = false;

	//药剂配比
	CDrugRatioVec																			m_DrugRatioVec;

	long long																						m_StartTime = 0;

	long long																						m_EndTIme = 0;
};

#endif // CDLGAGROMANUREPARASHOW_H
