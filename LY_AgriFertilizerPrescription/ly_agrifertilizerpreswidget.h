#ifndef LY_AGRIFERTILIZERPRESWIDGET_H
#define LY_AGRIFERTILIZERPRESWIDGET_H

#include <QWidget>
#include "LY_AgriFertilizerPrescriptionActivator.h"
#include "ly_agrifertilizepescviewmodel.h"
#include "IProj.h"
#include <map>
#include <vector>
#include <QCString.h>
#include <QUuid>
#include <QMutex>
#include "LyDialog.h"
namespace Ui {
class LY_AgriFertilizerPresWidget;
}
enum  FertilizerPresConcolType
{
	FertilizerPresConcolType_ADD = 1,
	FertilizerPresConcolType_EDIT = 2,
	FertilizerPresConcolType_DELETE = 3,
	FertilizerPresConcolType_NONE = 4,
};


class LY_AgriFertilizerPresWidget : public LyDialog
{
    Q_OBJECT

public:
    explicit LY_AgriFertilizerPresWidget(QWidget *parent = nullptr);
    ~LY_AgriFertilizerPresWidget();

	void draw3DGridMap(QPainter &painter, IProj *pProj);
	void updataCurActiveFarm();
	void draw2DGridMap(QPainter &painter, IProj *pProj);
	
protected:

	void hideEvent(QHideEvent *event);
	void showEvent(QShowEvent *event);

private slots:

	//添加盐田
	void addField(QString &text, QString & field_id, QString &id);
	//添加处方图
	void on_prescription_add_click();
	//修改处方图
	void on_prescription_edit_click();
	//删除处方图
	void on_prescription_delete_click();
	//导出处方图
	void on_prescription_export_click();
	//生成处方图
	void on_prescription_generate_click();

	//显示/隐藏处方图
	void on_prescription_control_click();
	//数据集box
	void on_pescription_data_boxChanged(const int & index);
	//幅宽 box
	void pescription_width_boxChanged(const int & index);
	//算法类型
	void calculation_type_boxChanged(const int &);
	//算法
	void calculation_boxChanged(const int &);
	//处方图box
	void on_soilMap_boxChanged(const int &);

	void onCellClicked(int row, int column);
private:

	void setupUi();
	void setupData(QString &field_id);
	void setMapTableWidgetData(FertilizerPrescriptionVec vec);
	void setMapInfoTableWidgetData(FertilizerPrescriptionGridInfoVec vec);
	void  setComboxStatus(bool isEnable);
private:
	Ui::LY_AgriFertilizerPresWidget *ui;

	LY_AgriFertilizePescViewModel         *m_viewModel = nullptr;
	AgriSoilFertilityDistributeMapPtrVec  m_soilDistributeMapVec;
	AgriSoilFertilityDistributeMapPtr        m_currentDistributeMap = nullptr;

	FertilizerPrescriptionVec				m_listVec;
	FertilizerPrescriptionGridInfoVec   m_listInfoVec;
	QMutex    sMutex;
	FertilizerPresConcolType  m_controcol_type = FertilizerPresConcolType_NONE;

	QString m_currentFieldID = "";
	QString m_currentFieldName = "";
	QString mapInfoId = "";
	QString dataset_id = ""; //数据集id
	bool m_isGenerate = false;
	bool m_show = false;
	int m_selectedCheckBoxRow = -1;

	std::map<QString, std::vector<QString>> m_calculation_map = {
		{tr2("土壤植株测试推荐法"),{ tr2("氮素实时监控施肥技术"),  tr2("磷钾养分恒量监控施肥技术"), tr2("中、微量元素养分矫正施肥技术") } },
		{tr2("田间试验法"),{ tr2("肥料效应函数法"),  tr2("土壤养分丰缺指标法"), tr2("氮、磷、钾比例法") } },
		{tr2("目标产量法"),{ tr2("养分平衡法"),  tr2("地力差减法")} }
	};

	QString GenerateUuid()
	{
		const QByteArray &ba = QUuid::createUuid().toRfc4122();
		QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
		return uid.toString(QUuid::WithoutBraces);
	}

};

#endif // LY_AGRIFERTILIZERPRESWIDGET_H
