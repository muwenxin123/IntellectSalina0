#ifndef AGRIPATROLLEGEND_h__
#define AGRIPATROLLEGEND_h__

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QTableWidget>

#include <unordered_map>
#include "QCString.h"

namespace Ui
{
	class AgriPatrolLegend;
}

class AgriPatrolLegend : public QWidget
{
	Q_OBJECT

public:
	struct ImageData {
		ImageData()
			:data("")
			, category("")
			, title("")
			, imagePath("")
			, description("")
		{};
		QString data;		  // Í¼ÏñÈÕÆÚ
		QString category;	  // Í¼Ïñ·ÖÀà
		QString title;        // Í¼ÏñÃû³Æ
		QString imagePath;    // Í¼ÏñÂ·¾¶
		QString description;  // Í¼ÏñÃèÊö
	};

	explicit AgriPatrolLegend(QWidget *parent = nullptr);
	~AgriPatrolLegend();

public:
	void UpdatTaskInfoLegend();
	void UpdateShowLegend();

protected:
	void changeEvent(QEvent *event) override;
	void showEvent(QShowEvent *event) override;
	void hideEvent(QHideEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

private:
	void Init();
	void InitGrowthDisplayLegend(QTableWidget *table);
	void InitYieldDisplayLegend(QTableWidget *table);

	void UpdateGrowthLegendLabel(QLabel *label);
	void UpdateYieldLegendLabel(QLabel *label);
	void UpdateGrowthSuggestLabel(QLabel *label);
	QWidget* UPdateBottomWidget(QString image, QString title);

	void UpdateGrowthLegendTable(QTableWidget *table, const QVector<QVector<QString>> &growthList);
	void UpdateYieldLegendTable(QTableWidget *table, const QVector<QVector<QString>> &yieldList);

private:
	QPoint m_dragPosition;
	bool m_dragging = false;

private:
	Ui::AgriPatrolLegend		*ui;
	std::unordered_map<std::string, QVector<QVector<QString>>> m_growthListMap = {
		{ "20250416T051208",{
			{ "#379123", tr2("ÓÅ"), "5.87", "76.44" },
			{ "#b9db86", tr2("Á¼"), "1.38", "18.04" },
			{ "#f2ae79", tr2("ÖÐ"), "0.31", "4.03" },
			{ "#ba1414", tr2("²î"), "0.11", "1.5" },
		} }
	};

	QVector<QVector<QString>> m_yieldList = {
		{ "#6b0000", "90-115", "5.87", "76.44" },
		{ "#ad5313", "116-131", "1.38", "18.04" },
		{ "#f2a72e", "132-145", "0.31", "4.03" },
		{ "#fad155", "146-160", "0.11", "1.5" },
		{ "#ffff80", "161-183", "0.0", "0.0" },
	};
};
#endif // AGRIPATROLLEGEND_h__
