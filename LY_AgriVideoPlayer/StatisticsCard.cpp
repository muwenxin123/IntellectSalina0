// StatisticsCard.cpp
#include "StatisticsCard.h"
#include <QPainter>
#include <QFont>
#include <QDateTime>
#include <QDebug>
#include <QCString.h>

// 构造函数
StatisticsCard::StatisticsCard(QWidget *parent)
	: QWidget(parent)
	, m_isHovered(false)
{
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	//setAttribute(Qt::WA_TranslucentBackground);
	//setFixedSize(280, 400);

	setupUI();

	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(500);
	connect(m_updateTimer, &QTimer::timeout, this, &StatisticsCard::refreshDisplay);
	m_updateTimer->start();
}

// 析构函数
StatisticsCard::~StatisticsCard()
{
}

// 辅助函数：格式化类别名
QString StatisticsCard::formatClassName(const QString& className)
{
	QString formatted = className;
	formatted.replace("_", " ");

	if (!formatted.isEmpty()) {
		formatted[0] = formatted[0].toUpper();
	}

	return formatted;
}

// 辅助函数：获取类别图标
QString StatisticsCard::getIconForClass(const QString& className)
{
	static QMap<QString, QString> iconMap;
	if (iconMap.isEmpty()) {
		iconMap["person"] = "??";
		iconMap["people"] = "??";
		iconMap["car"] = "??";
		iconMap["truck"] = "??";
		iconMap["bus"] = "??";
		iconMap["bicycle"] = "??";
		iconMap["motorbike"] = "???";
		iconMap["airplane"] = "??";
		iconMap["drone"] = "??";
		iconMap["boat"] = "?";
		iconMap["traffic light"] = "??";
		iconMap["fire"] = "??";
		iconMap["smoke"] = "??";
		iconMap["helmet"] = "??";
		iconMap["dog"] = "??";
		iconMap["cat"] = "??";
		iconMap["bird"] = "??";
	}

	for (auto it = iconMap.begin(); it != iconMap.end(); ++it) {
		if (className.contains(it.key(), Qt::CaseInsensitive)) {
			return it.value();
		}
	}

	return "??";
}

// 设置UI
void StatisticsCard::setupUI()
{
	m_mainLayout = new QVBoxLayout(this);
	m_mainLayout->setContentsMargins(15, 15, 15, 15);
	m_mainLayout->setSpacing(8);

	// 标题区域
	QHBoxLayout* titleLayout = new QHBoxLayout();

	m_titleLabel = new QLabel(tr2("实时检测统计"));
	m_titleLabel->setStyleSheet(
		"QLabel {"
		"   color: #333;"
		"   font-size: 16px;"
		"   font-weight: bold;"
		"}"
	);

	m_updateTimeLabel = new QLabel();
	m_updateTimeLabel->setStyleSheet(
		"QLabel {"
		"   color: #999;"
		"   font-size: 10px;"
		"}"
	);

	titleLayout->addWidget(m_titleLabel);
	titleLayout->addStretch();
	titleLayout->addWidget(m_updateTimeLabel);

	m_mainLayout->addLayout(titleLayout);

	// 模型信息
	m_modelLabel = new QLabel(tr2("当前模型: 未连接"));
	m_modelLabel->setStyleSheet(
		"QLabel {"
		"   color: #2196F3;"
		"   font-size: 11px;"
		"   background-color: rgba(33, 150, 243, 0.1);"
		"   padding: 4px 8px;"
		"   border-radius: 4px;"
		"}"
	);
	m_mainLayout->addWidget(m_modelLabel);

	QFrame* line = new QFrame();
	line->setFrameShape(QFrame::HLine);
	line->setStyleSheet("QFrame { background-color: rgba(200,200,200,0.3); max-height: 1px; }");
	m_mainLayout->addWidget(line);

	m_scrollArea = new QScrollArea();
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setFrameShape(QFrame::NoFrame);
	m_scrollArea->setStyleSheet(
		"QScrollArea {"
		"   background-color: transparent;"
		"}"
		"QScrollBar:vertical {"
		"   width: 8px;"
		"   background: rgba(0,0,0,0.1);"
		"   border-radius: 4px;"
		"}"
		"QScrollBar::handle:vertical {"
		"   background: rgba(0,0,0,0.3);"
		"   border-radius: 4px;"
		"}"
	);

	m_contentWidget = new QWidget();
	m_contentWidget->setStyleSheet("background-color: transparent;");
	m_contentLayout = new QVBoxLayout(m_contentWidget);
	m_contentLayout->setContentsMargins(0, 5, 0, 5);
	m_contentLayout->setSpacing(8);
	m_contentLayout->addStretch();

	m_scrollArea->setWidget(m_contentWidget);
	m_mainLayout->addWidget(m_scrollArea, 1);

	line = new QFrame();
	line->setFrameShape(QFrame::HLine);
	line->setStyleSheet("QFrame { background-color: rgba(200,200,200,0.3); max-height: 1px; }");
	m_mainLayout->addWidget(line);

	m_totalLabel = new QLabel(tr2("总计: 0 个目标"));
	m_totalLabel->setStyleSheet(
		"QLabel {"
		"   color: #4CAF50;"
		"   font-size: 14px;"
		"   font-weight: bold;"
		"   padding: 5px 0;"
		"}"
	);
	m_mainLayout->addWidget(m_totalLabel);
}

// 创建类别项
void StatisticsCard::createClassItem(const QString& className, int count, float avgConf)
{
	if (m_classItems.contains(className)) {
		QWidget* item = m_classItems[className];

		QLabel* countLabel = item->findChild<QLabel*>("countLabel");
		if (countLabel) {
			countLabel->setText(QString::number(count));
		}

		QLabel* confLabel = item->findChild<QLabel*>("confLabel");
		if (confLabel) {
			confLabel->setText(QString("%1%").arg(int(avgConf * 100)));
		}

		return;
	}

	QWidget* item = new QWidget();
	item->setFixedHeight(32);
	item->setStyleSheet(
		"QWidget:hover {"
		"   background-color: rgba(255,255,255,0.1);"
		"   border-radius: 4px;"
		"}"
	);

	QHBoxLayout* layout = new QHBoxLayout(item);
	layout->setContentsMargins(8, 0, 8, 0);
	layout->setSpacing(8);

	QLabel* iconLabel = new QLabel(getIconForClass(className));
	iconLabel->setStyleSheet("QLabel { font-size: 16px; }");

	QLabel* nameLabel = new QLabel(formatClassName(className));
	nameLabel->setStyleSheet(
		"QLabel {"
		"   color: #666;"
		"   font-size: 13px;"
		"}"
	);
	nameLabel->setWordWrap(true);

	QLabel* confLabel = new QLabel(QString("%1%").arg(int(avgConf * 100)));
	confLabel->setObjectName("confLabel");
	confLabel->setStyleSheet(
		"QLabel {"
		"   color: #FF9800;"
		"   font-size: 11px;"
		"   background-color: rgba(255,152,0,0.1);"
		"   padding: 2px 6px;"
		"   border-radius: 10px;"
		"}"
	);

	QLabel* countLabel = new QLabel(QString::number(count));
	countLabel->setObjectName("countLabel");
	countLabel->setStyleSheet(
		"QLabel {"
		"   color: #4CAF50;"
		"   font-size: 15px;"
		"   font-weight: bold;"
		"   min-width: 30px;"
		"   alignment: right;"
		"}"
	);

	layout->addWidget(iconLabel);
	layout->addWidget(nameLabel, 1);
	layout->addWidget(confLabel);
	layout->addWidget(countLabel);

	m_contentLayout->insertWidget(m_contentLayout->count() - 1, item);
	m_classItems[className] = item;
}

// 移除类别项
void StatisticsCard::removeClassItem(const QString& className)
{
	if (m_classItems.contains(className)) {
		QWidget* item = m_classItems[className];
		m_contentLayout->removeWidget(item);
		item->deleteLater();
		m_classItems.remove(className);
	}
}

// 刷新显示
void StatisticsCard::refreshDisplay()
{
	if (m_lastUpdateTime.isValid()) {
		qint64 msSince = m_lastUpdateTime.msecsTo(QDateTime::currentDateTime());
		if (msSince < 1000) {
			m_updateTimeLabel->setText("刚刚更新");
		}
		else {
			m_updateTimeLabel->setText(QString(tr2("%1秒前")).arg(msSince / 1000));
		}
	}

	m_totalLabel->setText(QString(tr2("总计: %1 个目标")).arg(m_currentData.totalCount));

	QStringList currentClasses = m_currentData.classCounts.keys();

	for (const QString& className : currentClasses) {
		int count = m_currentData.classCounts[className];
		float avgConf = m_currentData.avgConfidence.value(className, 0);
		createClassItem(className, count, avgConf);
	}

	QStringList existingClasses = m_classItems.keys();
	for (const QString& className : existingClasses) {
		if (!currentClasses.contains(className)) {
			removeClassItem(className);
		}
	}
}

// 更新统计数据
void StatisticsCard::updateStatistics(const StatisticsData& data)
{
	qDebug() << "========== StatisticsCard::updateStatistics ==========";
	qDebug() << "totalCount:" << data.totalCount;
	qDebug() << "classCounts size:" << data.classCounts.size();

	m_currentData = data;
	m_lastUpdateTime = QDateTime::currentDateTime();

	if (!data.modelNames.isEmpty()) {
		m_modelLabel->setText(QString(tr2("当前模型: %1")).arg(data.modelNames.join(", ")));
		m_modelLabel->show();
	}
	else {
		m_modelLabel->hide();
	}

	refreshDisplay();
}

// 清空统计
void StatisticsCard::clearStatistics()
{
	m_currentData.clear();
	m_lastUpdateTime = QDateTime();
	m_updateTimeLabel->setText("");
	m_modelLabel->setText(tr2("当前模型: 未连接"));

	QStringList classes = m_classItems.keys();
	for (const QString& className : classes) {
		removeClassItem(className);
	}

	m_totalLabel->setText(tr2("总计: 0 个目标"));
}

// 事件处理函数
void StatisticsCard::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QColor bgColor = m_isHovered ? QColor(30, 30, 40, 240) : QColor(25, 25, 35, 220);
	painter.setBrush(bgColor);
	painter.setPen(Qt::NoPen);

	QPainterPath path;
	path.addRoundedRect(rect(), 12, 12);
	painter.drawPath(path);

	painter.setPen(QPen(QColor(100, 100, 120, 100), 1));
	painter.drawPath(path);

	QWidget::paintEvent(event);  // 调用父类事件
}

void StatisticsCard::enterEvent(QEvent *event)
{
	m_isHovered = true;
	update();
	QWidget::enterEvent(event);
}

void StatisticsCard::leaveEvent(QEvent *event)
{
	m_isHovered = false;
	update();
	QWidget::leaveEvent(event);
}

void StatisticsCard::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
}

void StatisticsCard::setVisible(bool visible)
{
	if (visible) {
		QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
		anim->setDuration(200);
		anim->setStartValue(0);
		anim->setEndValue(1);
		anim->start(QAbstractAnimation::DeleteWhenStopped);
	}
	QWidget::setVisible(visible);
}