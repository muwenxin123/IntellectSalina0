#ifndef LY_AGRISPRAYCOMMANDWIDGET_H
#define LY_AGRISPRAYCOMMANDWIDGET_H
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"
#include "AgriDataAnalysisPredictAlgorithm.h"
#include <QWidget>
#include <QHideEvent>
#include <QLabel>
#include <QTableWidget>
#include <QLabel>

using namespace AgriSoilTestingFertilizer;

namespace Ui {
class LY_AgriSprayCommandWidget;
}

class LY_AgriSprayCommandWidget : public QWidget
{
	Q_OBJECT

public:
    explicit LY_AgriSprayCommandWidget(QWidget *parent = nullptr);
    ~LY_AgriSprayCommandWidget();

	void setData(std::vector< AirInstructionPoi>airInstructionPoints);
	
	void setIsSpray(bool isSpray);
protected:

	virtual void hideEvent(QHideEvent *event);
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;


private:

	void updateLegendTable(QTableWidget *table, const QVector<QVector<QString>> &growthList);
    void setupPenYaoIMage(QLabel *label,const QString imgUrl);
	void setupUI();

private:

	QLabel *m_titleLabel = nullptr;
	QPoint m_dragPosition;
	bool m_dragging = false;
    Ui::LY_AgriSprayCommandWidget *ui;
};

#endif // LY_AGRISPRAYCOMMANDWIDGET_H
