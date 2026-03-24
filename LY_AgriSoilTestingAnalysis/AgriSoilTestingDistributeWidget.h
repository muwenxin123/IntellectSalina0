#ifndef AGRISOILTESTINGDISTRIBUTEWIDGET_h__
#define AGRISOILTESTINGDISTRIBUTEWIDGET_h__

#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QComboBox>
#include <QStackedLayout>

#include "AgriSoilAnalysisPredictUtil.h"
#include "KrigingInterPolator.h"

namespace Ui
{
	class AgriSoilTestingDistributeWidget;
}

struct DisMapPropDisplayVaild
{
	DisMapPropDisplayVaild() {};
	DisMapPropDisplayVaild(QString propName, QString displayName, int vaildIndex)
		: prop_name(propName),
		display_name(displayName),
		vaild_index(vaildIndex)
	{};
	QString		prop_name;
	QString		display_name;
	int			vaild_index;
};

typedef	std::shared_ptr<DisMapPropDisplayVaild>			DisMapPropDisplayVaildPtr;
typedef std::vector<DisMapPropDisplayVaildPtr>			DisMapPropDisplayVaildPtrVec;

Q_DECLARE_METATYPE(DisMapPropDisplayVaild)


class AgriSoilTestingDataSetDialog;
class AgriSoilTestingDistributeWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AgriSoilTestingDistributeWidget(QWidget *parent = nullptr);
	~AgriSoilTestingDistributeWidget();

public:
	static QPair<bool, QString> getSoilFertilityDistribute(
		const AgriSoilFertilityDistributeMapPtr inDistributeMap,
		AgriSoilTestingFertilityPointPtrVec & outSoilFertilityPointVec,
		std::vector<QPointF> boundPoings = std::vector<QPointF>(),
		QString propName = "");

public:
	void draw2DGrid(QPainter &painter, IProj *pProj);
	void setCurrentDataset(const AgriSoilTestingDatasetPtr& soilTestingDataset);
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
	void UpdateDisplayCombox();
	void UpdateDistributeBasicInfo();
	void UpdateDistributeMapInfo();
	void UpdateGridDisplayStatistic(DiffusionLayer difLayer, const std::vector<int>& statisticNmbers);
	void UpdateGridDisplayLegend(const AgriSoilFertilityDistributeMapPtr pDistributeMap);
	void UpdateGridDisplayLegendTable(const QList<QPair<QString, std::string>>& gridData, const QStringList& legendList, const std::vector<int>& difCount);

private:
	QWidget* createLegendGroupBox(const QList<QPair<QString, std::string>>& legendData, QWidget *parent = nullptr);
	QWidget* createGridWidget(const QList<QPair<QString, std::string>>& gridData, QWidget *parent = nullptr);
	void cleanUpGridDisplay();
	void cleanUpMap3D();
	void cleanUpGridDisplayLegend();

private:
	static void GetActiveFieldBoundPoings(std::vector<QPointF>& boundPoings);
	static void GetZoneBoundPoints(QString datasetId, std::vector<QPointF>& boundPoings);
	static QString GetMatchPropName(int validIdentify);
	static double GetMatchValue(AgriSoilTestingCollectionPtr collect, QString elePropName);
	static double CalcMatchLayer(QString elePropName, double elePropValue);
	static void GetMatchLegend(QString elePropName, QStringList& headerList, QStringList& legendList);

	struct LegendEntry {
		double max_val;
		double min_val;
		double avg_val;
		std::vector<QString> rangeText;
	};
	static std::vector<LegendEntry> GenerateLegendFromData(const std::vector<double>& vecData, int groupCount);

	static QString JoinStatisticByHeatMap3D(HeatMap3D *heatMap3D, const QString &sep = ",");

	public slots:
	void on_btn_generate_clicked();
	void on_btn_save_clicked();
	void on_btn_show_control_clicked();
	void on_checkBox_coordinate_stateChanged(int nStates);
	void on_checkBox_grid_stateChanged(int nStates);
	void on_comboBox_dismap_type_currentTextChanged(int index);

private:
	Ui::AgriSoilTestingDistributeWidget												*ui;
	std::mutex																			m_mutex;
	bool																				m_show = false;
	AgriSoilTestingDatasetPtr															m_soilTestingDataset = nullptr;
	QStackedLayout																		*m_stackedLayout = nullptr;
	HeatMap3D																			*m_heatMap3D = nullptr;
	bool																				m_bMap3DShow = true;
	AgriSoilFertilityDistributeMapPtr													m_distributeMap = nullptr;
	std::unordered_map<DiffusionLayer, QList<QPair<QString, std::string>>>				m_mDiffusionLayerHexColor;
	std::unordered_map<DiffusionLayer, std::vector<osgEarth::Symbology::Color>>			m_mDiffusionLayerOsgColor;
	double																				m_dGridSpace = 1.0;// µ¥Î»£¨Ã×£©
	int																					m_nDisCount = 1;
	int																					m_nEqualCount = 0;
	std::unordered_map<DistributeMapType, DisMapPropDisplayVaildPtrVec>					m_mDistributeMapBitIndex;

private:
	static QStringList N_List, P_List, K_List;
	static QStringList N_Headers, P_Headers, K_Headers;
};
#endif // AGRISOILTESTINGDISTRIBUTEWIDGET_h__
