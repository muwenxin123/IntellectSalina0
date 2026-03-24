#ifndef AGRIPESTANDDISEASEANALYSISWIDGET_H_
#define AGRIPESTANDDISEASEANALYSISWIDGET_H_
 
#include <QWidget>
#include <Protocol/XygsJL/sharedFile.h>

#include "AgriPestAndDiseaseDistributeWidget.h"
#include "AgriPestAndDiseaseDataCollectDialog.h"
#include "AgriDataAnalysisPredictUtil.h"

#include "IProj.h"
#include "map3d/IEarth.h"
#include "AgrisprayComboBox.h"

namespace Ui
{
	class AgriPestAndDiseaseAnalysisWidget;
}

class AgriPestAndDiseaseDataSetDialog;
class AgriPestAndDiseaseAnalysisWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AgriPestAndDiseaseAnalysisWidget(QWidget *parent = nullptr);
	~AgriPestAndDiseaseAnalysisWidget();

	AgriPestAndDiseaseDataCollectDialog* getPestAndDiseaseDataCollectDialog();
	AgriPestAndDiseaseDistributeWidget* getPestAndDiseaseDataCollectWidget();
	AgrisprayComboBox* getAgrisprayComboBox();

	qnzkna::ZoneManage::CZoneBase GetActiveFieldZone();
	void UpdataCurActiveFarm();
	void DrawDynamic(QPainter &painter, IProj *pProj);
	void On2d3dChanged(int wParam);

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void Init();
	void InitDisplayTable();
	void InitDisplayCombox();
	void CreateDataSetDialog(DataSetHandleMode handleMode);
	void HandleDatasetAddFinish(AgriPestDatasetPtr pestDataset);
	void HandleDatasetUpdateFinish(AgriPestDatasetPtr pestDataset);

private:
	void UpdateTableData();
	void AddOneRowTotable(AgriPestDatasetPtr pestDataset);
	void UpdateOneRowTotable(AgriPestDatasetPtr pestDataset);
	void Update3DIconDraw(const AgriPestCollectionPtrVec& pestCollectVec, bool isDraw);

private slots :
	void on_ptn_view_dataset_clicked();
	void on_ptn_add_dataset_clicked();
	void on_ptn_delete_dataset_clicked();
	void on_ptn_update_dataset_clicked();
	void on_ptn_export_dataset_clicked();

	void onCellClicked(int row, int column);
	void onCellDoubleClicked(int row, int column);
	void onCheckBoxToggled(int row, bool checked);

private:
	Ui::AgriPestAndDiseaseAnalysisWidget						*ui;
	bool														m_show = false;
	AgriPestAndDiseaseDataSetDialog								*m_pestAndDiseaseDataSetDialog = nullptr;
	int															m_nSelectedIndex = -1;
	AgriFieldInfoPtr											m_pFieldInfo;
	ThreadSafeMap<AgriPestCollectionPtrVec>						m_pestDrawMap;
	QString														m_sIconPath;
	std::vector<ILabel*>										m_3DPestCollectIconVec;
	AgrisprayComboBox* m_pAgrisprayComboBox = nullptr;

};

#endif  // AgriPestAndDiseaseAnalysisWidget_H