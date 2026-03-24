#ifndef AGRIFIELDIDENTIFYCALCULATE_H_
#define AGRIFIELDIDENTIFYCALCULATE_H_
 
#include <QWidget>

namespace Ui
{
	class AgriFieldIdentifyCalculate;
}

struct FeatureInfo_shp
{
	FeatureInfo_shp()
		:id("")
		, crop_type("none")
		, area_mu(0.0)
		, area_ha(0.0)
		, center_point(0.0, 0.0)
	{}
	std::string id;
	std::string crop_type;
	double area_mu;
	double area_ha;
	QPointF center_point;
	QVector<QPointF> point_list;
};

struct AgriFieldIdentifyCalculatePrivate;
class AgriFieldIdentifyCalculate : public QWidget
{
	Q_OBJECT

public:
	explicit AgriFieldIdentifyCalculate(QWidget *parent = nullptr);
	~AgriFieldIdentifyCalculate();

	void GetIndentifyAreaList(std::vector<FeatureInfo_shp> & areaList);
	bool CheckAreaUsable();

private:
	enum LineWrappingType
	{
		ADD_TYPE_NOLINE = 0,        // 不换行
		ADD_TYPE_LINE,              // 换行
		ADD_TYPE_REPLACE_LINE,      // 替换当前行
	};
	void AppendLogMessage(const QString& msg, const QString& color, LineWrappingType optType);
	void SetProcess(double dVal);

signals:
	void AreaIdentifyFinishied();

private slots:
	void on_BT_Play_clicked();
	void on_BT_Stop_clicked();

private:
	Ui::AgriFieldIdentifyCalculate		*ui;
	AgriFieldIdentifyCalculatePrivate	*_p;
	QVector<QPointF>					m_areaPoints;
};
#endif  // AgriFieldIdentifyCalculate_H