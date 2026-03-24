#ifndef AGRIPESTANDDISEASEDATASETDIALOG_H_
#define AGRIPESTANDDISEASEDATASETDIALOG_H_
 
#include <QWidget>
#include <QTableWidget>
#include <QContextMenuEvent>

#include "LyDialog.h"
#include "AgriDataAnalysisPredictDecl.h"

namespace Ui {
	class AgriPestAndDiseaseDataSetDialog;
}

class AgriPestAndDiseaseDataCollectDialog;
class AgriPestAndDiseaseDataSetDialog : public LyDialog
{
	Q_OBJECT

public:
	explicit AgriPestAndDiseaseDataSetDialog(DataSetHandleMode handleMode, const AgriFieldInfoPtr& fieldInfo, AgriPestDataset datasetInfo, QWidget *parent = nullptr);
	~AgriPestAndDiseaseDataSetDialog();

	AgriPestAndDiseaseDataCollectDialog* getPestAndDiseaseDataCollectDialog();

protected:
	void closeEvent(QCloseEvent* event) override;
	void contextMenuEvent(QContextMenuEvent *event) override;

private:
	void Init();
	void InitDisplayTable();
	void InitDisplayCombox();
	void AddDataSet(AgriPestDatasetPtr pestDataset);
	void UpdateDataSet(AgriPestDatasetPtr pestDataset);

	void HandleDataCollectionAddFinish(AgriPestCollectionPtr pestCollection);
	void HandleDataCollectionUpdateFinish(AgriPestCollectionPtr pestCollection);

	void UpdateTableData();
	void AddOneRowToTable(AgriPestCollectionPtr pestCollection);
	void UpdateRowToTable(AgriPestCollectionPtr pestCollection);
	void LoadTablePagedData();
	void FillTableRowContents(AgriPestCollectionPtr pestCollection, int nRow);

signals:
	void addFinish(AgriPestDatasetPtr pestDataset);
	void updateFinish(AgriPestDatasetPtr pestDataset);
	void updateCollection(DataCollectHandle handle, AgriPestCollectionPtr pestCollection);

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

private:
	QString									m_DataSetUid = "";
	AgriPestAndDiseaseDataCollectDialog		*m_dataCollectDialog = nullptr;
	AgriFieldInfoPtr						m_pFieldInfo;
	AgriPestDataset							m_pDatasetInfo;
	DataSetHandleMode						m_DataSetHandleMode;
	int										m_currentPage = 1;
	int										m_pageSize = 10;
	int										m_totalPages = 0;

private:
	Ui::AgriPestAndDiseaseDataSetDialog		*ui;
	QPushButton								*prevButton;
	QPushButton								*nextButton;
	QLabel									*pageLabel;
};
#endif  // AgriPestAndDiseaseDataSetDialog_H