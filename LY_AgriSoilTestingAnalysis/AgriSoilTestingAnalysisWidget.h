#ifndef AGRISOILTESTINGANALYSISWIDGET_h__
#define AGRISOILTESTINGANALYSISWIDGET_h__

#include <QWidget>
#include <Protocol/XygsJL/sharedFile.h>

#include "IProj.h"
#include "map3d/IEarth.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "IAgriSoilAnalysisPredictActivator.h"
#include "AgriSoilTestingDistributeWidget.h"
#include "AgriFieldTree.h"


namespace Ui
{
	class AgriSoilTestingAnalysisWidget;
}

class AgriSoilTestingDataSetDialog;
class AgriSoilTestingAnalysisWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AgriSoilTestingAnalysisWidget(QWidget *parent = nullptr);
	~AgriSoilTestingAnalysisWidget();

	AgriSoilTestingDistributeWidget* getPestAndDiseaseDataCollectWidget();
	AgriFieldTree* getAgriFieldTree();

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
	void HandleDatasetAddFinish(AgriSoilTestingDatasetPtr soilTestingDataset);
	void HandleDatasetUpdateFinish(AgriSoilTestingDatasetPtr soilTestingDataset);

private:
	void UpdateTableData();
	void AddOneRowTotable(AgriSoilTestingDatasetPtr soilTestingDataset);
	void UpdateOneRowTotable(AgriSoilTestingDatasetPtr soilTestingDataset);
	void Update3DIconDraw(const AgriSoilTestingCollectionPtrVec& soilTestingCollectVec, bool isDraw);

	private slots :
	void on_ptn_view_dataset_clicked();
	void on_ptn_add_dataset_clicked();
	void on_ptn_delete_dataset_clicked();
	void on_ptn_update_dataset_clicked();
	void on_ptn_export_dataset_clicked();

	void onCellClicked(int row, int column);
	void onCellDoubleClicked(int row, int column);
	void onCheckBoxToggled(int row, bool checked);

	void on_comboBox_sample_type_currentTextChanged(int index);

private:
	Ui::AgriSoilTestingAnalysisWidget						*ui;
	bool														m_show = false;
	AgriSoilTestingDataSetDialog								*m_pestAndDiseaseDataSetDialog = nullptr;
	int															m_nSelectedIndex = -1;
	AgriFieldInfoPtr											m_pFieldInfo;
	ThreadSafeMap<AgriSoilTestingCollectionPtrVec>				m_soiltestingDrawMap;
	QString														m_sIconPath;
	std::vector<ILabel*>										m_3DPestCollectIconVec;
};

#endif // AGRISOILTESTINGANALYSISWIDGET_h__
