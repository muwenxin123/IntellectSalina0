#ifndef AGRISOILTESTINGDATASETDIALOG_h__
#define AGRISOILTESTINGDATASETDIALOG_h__

#include <mutex>

#include <QWidget>
#include <QTableWidget>
#include <QContextMenuEvent>

#include "LyDialog.h"
#include "KrigingInterPolator.h"
#include "IAgriSoilAnalysisPredictActivator.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"

namespace Ui {
	class AgriSoilTestingDataSetDialog;
}

class AgriPestAndDiseaseDataCollectDialog;
class AgriSoilTestingDataSetDialog : public LyDialog
{
	Q_OBJECT

public:
	explicit AgriSoilTestingDataSetDialog(DataSetHandleMode handleMode, const AgriFieldInfoPtr& fieldInfo, AgriSoilTestingDataset datasetInfo, QWidget *parent = nullptr);
	~AgriSoilTestingDataSetDialog();

	void draw2DGrid(QPainter &painter, IProj *pProj);

protected:
	void closeEvent(QCloseEvent* event) override;
	void contextMenuEvent(QContextMenuEvent *event) override;

private:
	void Init();
	void InitDisplayTable();
	void InitDisplayComboBox();
	void InitDisplaySlider();
	void InitCheckbox();

	void AddDataSet(AgriSoilTestingDatasetPtr dataset);
	void UpdateDataSet(AgriSoilTestingDatasetPtr dataset);

	void HandleDataCollectionAddFinish(AgriSoilTestingCollectionPtr soiltestingCollection);
	void HandleDataCollectionUpdateFinish(AgriSoilTestingCollectionPtr soiltestingCollection);

	void UpdateTableData();
	void AddOneRowToTable(AgriSoilTestingCollectionPtr soiltestingCollection);
	void UpdateRowToTable(AgriSoilTestingCollectionPtr soiltestingCollection);
	void LoadTablePagedData();
	void FillTableRowContents(AgriSoilTestingCollectionPtr soiltestingCollection, int nRow);

	void cleanUpMap3D();

private:
	quint32 encodeCheckboxes();
	void decodeCheckboxes(quint32 value);
signals:
	void addFinish(AgriSoilTestingDatasetPtr soilTestingDataset);
	void updateFinish(AgriSoilTestingDatasetPtr soilTestingDataset);
	void updateCollection(DataCollectHandle handle, AgriSoilTestingCollectionPtr soiltestingCollection);

	private slots:
	void addData();
	void updateData();
	void deleteData();
	void viewData();
	void viewTableData(QTableWidgetItem *item);

	void on_btnOK_clicked();
	void on_btnCancel_clicked();

	void onPrevPage();
	void onNextPage();

	void onCellDoubleClicked(int row, int column);

	void on_comboBox_sample_type_currentTextChanged(int index);
	void on_comboBox_sample_mode_currentTextChanged(int index);

	void on_btn_generate_grid_clicked();
	void on_ptn_export_points_clicked();
	void on_ptn_generate_samples_clicked();
	void on_ptn_save_samples_clicked();
	void on_ptn_import_samples_clicked();
	void on_ptn_show_samples_clicked();

private:
	void updateDataToTable(const AgriSoilTestingCollectionPtrVec& soilTestingCollectionVec);
	AgriSoilTestingCollectionPtrVec SimulateGetSoilTestingCollectionPagedData(int nPage, int nPageSize) const;

private:
	std::mutex								m_mutex;
	QString									m_DataSetUid = "";
	AgriFieldInfoPtr						m_pFieldInfo;
	AgriSoilTestingDataset					m_pDatasetInfo;
	DataSetHandleMode						m_DataSetHandleMode;
	int										m_currentPage = 1;
	int										m_pageSize = 10;
	int										m_totalPages = 0;

	HeatMap3D								*m_heatMap3D = nullptr;
	bool									m_bMap3DShow = true;
	bool									m_bPointIdShow = true;
	double									m_spacing = 20.0; // µ¥Î»£¨Ã×£©
	int										m_equalCount = 0;
	int										m_disCount = 1;

	AgriSoilTestingCollectionPtrVec			m_soilTestingCollectionVec;

private:
	Ui::AgriSoilTestingDataSetDialog		*ui;
	QPushButton								*prevButton;
	QPushButton								*nextButton;
	QLabel									*pageLabel;
	QVector<QCheckBox*>						checkboxes;
};
#endif // AGRISOILTESTINGDATASETDIALOG_h__
