#ifndef AGRIPESTANDDISEASEDISTRIBUTEWIDGET_H_
#define AGRIPESTANDDISEASEDISTRIBUTEWIDGET_H_
 
#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QComboBox>
#include <QStackedLayout>

#include "AgriDataAnalysisPredictUtil.h"
#include "KrigingInterPolator.h"

namespace Ui
{
	class AgriPestAndDiseaseDistributeWidget;
}

class AgriPestAndDiseaseDataSetDialog;
class AgriPestAndDiseaseDistributeWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AgriPestAndDiseaseDistributeWidget(QWidget *parent = nullptr);
	~AgriPestAndDiseaseDistributeWidget();

public:
	void draw2DGrid(QPainter &painter, IProj *pProj);
	void setCurrentDataset(const AgriPestDatasetPtr& pestDataset);
	inline double getGridSpace() { return m_dGridSpace; };
	void cleanUp();

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void Init();
	void InitDisplayCombox();
	void InitGridDisplayLegend();
	void UpdateGridDisplayStatistic(DiffusionLayer difLayer, const std::vector<int>& difCount);
	void UpdataDistributeBasicInfo();
	void UpdataDistributeMapInfo();

private:
	QWidget* createLegendGroupBox(const QList<QPair<QString, std::string>>& legendData, QWidget *parent = nullptr);
	QWidget* createGridWidget(const QList<QPair<QString, std::string>>& gridData, QWidget *parent = nullptr);
	void cleanUpGridDisplay();
	void cleanUpMap3D();

public slots:
	void on_btn_generate_clicked();
	void on_btn_save_clicked();
	void on_btn_show_control_clicked();
	void on_checkBox_coordinate_stateChanged(int nStates);
	void on_checkBox_grid_stateChanged(int nStates);
	void on_comboBox_gene_type_currentTextChanged(int nIndex);

private:
	Ui::AgriPestAndDiseaseDistributeWidget						*ui;
	std::mutex													m_mutex;
	bool														m_show = false;
	AgriPestDatasetPtr											m_pestDataset = nullptr;
	QStackedLayout												*m_stackedLayout = nullptr;
	HeatMap3D													*m_heatMap3D = nullptr;
	bool														m_bMap3DShow = true;
	AgriPestDistributeMapPtr									m_distributeMap = nullptr;
	std::unordered_map<DiffusionLayer, QList<QPair<QString, std::string>>>				m_mDiffusionLayerHexColor;
	std::unordered_map<DiffusionLayer, std::vector<osgEarth::Symbology::Color>>			m_mDiffusionLayerOsgColor;
	double														m_dGridSpace = 1.0 * (1.0 / 110000.0);
	int															m_nLayerCount = 1;
};
#endif  // AgriPestAndDiseaseDistributeWidget_H