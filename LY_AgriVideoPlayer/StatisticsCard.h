#ifndef STATISTICSCARD_H
#define STATISTICSCARD_H

#include <QWidget>
#include <QMap>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QDateTime>

struct StatisticsData {
	int totalCount = 0;
	QMap<QString, int> classCounts;
	QMap<QString, float> avgConfidence;
	QMap<QString, QVector<int>> trackIds;
	QStringList modelNames;

	void clear() {
		totalCount = 0;
		classCounts.clear();
		avgConfidence.clear();
		trackIds.clear();
		modelNames.clear();
	}
};

class StatisticsCard : public QWidget
{
	Q_OBJECT
public:
	explicit StatisticsCard(QWidget *parent = nullptr);
	~StatisticsCard();

	public slots:
	void updateStatistics(const StatisticsData& data);
	void setVisible(bool visible) override;
	void clearStatistics();

protected:
	void paintEvent(QPaintEvent *event) override;
	void enterEvent(QEvent *event) override;
	void leaveEvent(QEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;

private:
	void setupUI();
	void refreshDisplay();
	void createClassItem(const QString& className, int count, float avgConf);
	void removeClassItem(const QString& className);


	QString getIconForClass(const QString& className);
	QString formatClassName(const QString& className);

private:
	QVBoxLayout* m_mainLayout;
	QLabel* m_titleLabel;
	QLabel* m_modelLabel;
	QLabel* m_totalLabel;
	QLabel* m_updateTimeLabel;
	QScrollArea* m_scrollArea;
	QWidget* m_contentWidget;
	QVBoxLayout* m_contentLayout;

	QMap<QString, QWidget*> m_classItems;
	StatisticsData m_currentData;
	bool m_isHovered;
	QTimer* m_updateTimer;
	QDateTime m_lastUpdateTime;
};

#endif // STATISTICSCARD_H