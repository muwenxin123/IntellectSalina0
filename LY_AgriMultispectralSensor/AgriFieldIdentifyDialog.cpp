#include <QPainter>

#include "AgriFieldIdentifyDialog.h"
#include "ui_AgriFieldIdentifyDialog.h"
#include "LY_AgriMultispectralSensor.h"


struct CropStat {
	int count = 0;
	double area_mu_sum = 0;
	double area_ha_sum = 0;
};

AgriFieldIdentifyDialog::AgriFieldIdentifyDialog(QWidget *parent) :
	LyDialog(tr2("田块识别"), parent),
    ui(new Ui::AgriFieldIdentifyDialog),
    m_show(true)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	InitData();

	connect(ui->btn_select_area, &QPushButton::clicked, this, &AgriFieldIdentifyDialog::on_btn_select_area_clicked);

	// 仿真运行结束
	connect(ui->widget_2, &AgriFieldIdentifyCalculate::AreaIdentifyFinishied, this, &AgriFieldIdentifyDialog::handleAreaIdentifyFinish);

	connect(ui->checkBox_mapShow, &QCheckBox::stateChanged, this, &AgriFieldIdentifyDialog::handleMapShowOrHide);
	
}

AgriFieldIdentifyDialog::~AgriFieldIdentifyDialog()
{
	m_areaPoints.clear();

    delete ui;
}

void AgriFieldIdentifyDialog::updateTargetPoint(const QPointF & point)
{
	m_areaPoints.emplace_back(point);

	AddOneRowToTable(point);
}

void AgriFieldIdentifyDialog::getSelectArea(QVector<QPointF> & areaPoints)
{
	m_areaPoints.forEach([&](const QPointF& poi) {
		areaPoints.push_back(poi);
	});
}

void AgriFieldIdentifyDialog::drawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_areaPoints.size())
	{
		QBrush brush;
		brush.setColor(Qt::transparent);
		brush.setStyle(Qt::BrushStyle::SolidPattern);
		painter.setBrush(brush);

		QPen pen(Qt::white);
		pen.setWidth(5);
		painter.setPen(pen);

		QVector<QPoint> points;
		points.clear();

		m_areaPoints.forEach([&](const QPointF& poi) {
			QPoint point = pProj->Jwd2Screen(poi.x(),
				poi.y());
			points.push_back(point);
		});

		int count = points.size();

		if (count == 1) {
			// 绘制点
			painter.setPen(Qt::red);
			painter.setBrush(Qt::red);
			painter.drawEllipse(points[0], 3, 3);  // 半径为3的小圆点
		}
		else if (count == 2) {
			// 绘制线段
			painter.setPen(QPen(Qt::blue, 2));
			painter.drawLine(points[0], points[1]);
		}
		else if (count >= 3) {
			// 绘制多边形
			painter.setPen(QPen(Qt::green, 2));
			painter.setBrush(QColor(0, 255, 0, 100)); // 半透明绿色
			painter.drawPolygon(points.constData(), points.size());
		}
	}

	if (m_bMapShow)
	{
		QFont originFont = painter.font();

		QPen pen(Qt::darkRed);
		pen.setWidth(2);

		QBrush brush;
		brush.setStyle(Qt::BrushStyle::SolidPattern);
		brush.setColor(QColor(67, 228, 255, 45));
		painter.setBrush(brush);

		// 设置颜色（带透明度（0-255，128 代表 50% 透明度））
		QColor textColor(67, 228, 255);
		textColor.setAlpha(200);
		pen.setColor(textColor);
		painter.setPen(pen);

		// 采集点绘制
		int pointSize = 9;
		QFont font("Arial", pointSize);
		painter.setFont(font);
		painter.setRenderHint(QPainter::TextAntialiasing);

		for (auto& shpInfo : m_areaList)
		{
			QVector<QPoint> points;

			QVector<QPointF>& tmpVec = shpInfo.point_list;
			for (unsigned j = 0; j < tmpVec.size(); ++j)
			{
				const QPointF& pt = tmpVec[j];
				QPoint point = pProj->Jwd2Screen(pt.x(), pt.y());
				points.push_back(point);
			}

			QPolygon poly(points);
			painter.drawPolygon(poly);
			QString text = "#" + QString::fromStdString(shpInfo.id) + "\n"
				+ tr2(shpInfo.crop_type.c_str()) + "\n"
				+ QString::number(shpInfo.area_mu) + tr2("亩");

			QPoint screenPoint = pProj->Jwd2Screen(shpInfo.center_point.x(), shpInfo.center_point.y());
			int x = screenPoint.x() - painter.fontMetrics().width(text) / 4;
			int y = screenPoint.y();

			QStringList lines = text.split('\n');
			for (const QString& line : lines) {
				painter.drawText(QPoint(x, y), line);
				y += painter.fontMetrics().height();
			}
		}

		painter.setFont(originFont);
	}
}

void AgriFieldIdentifyDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void AgriFieldIdentifyDialog::showEvent(QShowEvent *event)
{
    m_show = true;
}

void AgriFieldIdentifyDialog::hideEvent(QHideEvent *event)
{
    m_show = false;
}


void AgriFieldIdentifyDialog::InitData()
{
	InitAreaPointsTable();
}
 
void AgriFieldIdentifyDialog::InitAreaPointsTable()
{
	ui->table_area_points->clear();
	ui->table_area_points->verticalHeader()->setHidden(true);
	ui->table_area_points->setSelectionBehavior(QTableWidget::SelectRows);
	ui->table_area_points->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->table_area_points->setAlternatingRowColors(true);
	ui->table_area_points->setRowCount(0);
	// 禁用表格的编辑功能
	ui->table_area_points->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->table_area_points->setColumnCount(3);
	ui->table_area_points->setHorizontalHeaderLabels(QStringList() << tr2("序号") << tr2("经度") << tr2("纬度"));
	ui->tableWidget->resizeColumnsToContents(); // 自动调整列宽
}

void AgriFieldIdentifyDialog::AddOneRowToTable(QPointF point)
{
	int rowCount = ui->table_area_points->rowCount();
	ui->table_area_points->insertRow(rowCount);

	ui->table_area_points->setItem(rowCount, 0, new QTableWidgetItem(QString::number(rowCount + 1)));
	ui->table_area_points->setItem(rowCount, 1, new QTableWidgetItem(QString::number(point.x())));
	ui->table_area_points->setItem(rowCount, 2, new QTableWidgetItem(QString::number(point.y())));
}

void AgriFieldIdentifyDialog::on_btn_select_area_clicked()
{
	m_bPick = !m_bPick;

	if (m_bPick){
		m_areaPoints.clear();
		ui->table_area_points->clear();
		ui->table_area_points->setRowCount(0);
		ui->table_area_points->setColumnCount(3);
		ui->table_area_points->setHorizontalHeaderLabels(QStringList() << tr2("序号") << tr2("经度") << tr2("纬度"));
	}

	ui->btn_select_area->setText(m_bPick ? tr2("结束选择") : tr2("选择区域"));
}

void AgriFieldIdentifyDialog::handleAreaIdentifyFinish()
{
	if (ui->widget_2){
		m_areaList.clear();
		ui->widget_2->GetIndentifyAreaList(m_areaList);

		// 1. 分组统计
		std::unordered_map<std::string, CropStat> statMap;
		for (const auto& info : m_areaList) {
			auto& stat = statMap[info.crop_type];
			stat.count++;
			stat.area_mu_sum += info.area_mu;
			stat.area_ha_sum += info.area_ha;
		}

		// 2. 设置表格结构（列标题）
		ui->tableWidget->clear();
		ui->tableWidget->setRowCount(static_cast<int>(statMap.size()));
		ui->tableWidget->setColumnCount(5);
		ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr2("序号") << tr2("作物类型") << tr2("数量") << tr2("总面积（公顷）") << tr2("总亩数"));

		// 3. 填充表格内容
		int row = 0;
		for (const auto& pair : statMap) {
			const std::string& cropType = pair.first;
			const CropStat& stat = pair.second;

			ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));  // 序号
			ui->tableWidget->setItem(row, 1, new QTableWidgetItem(tr2(cropType.c_str()))); // 作物类型
			ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(stat.count))); // 数量
			ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(stat.area_ha_sum, 'f', 4))); // 总面积（公顷）
			ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(stat.area_mu_sum, 'f', 2)));  // 总亩数
			row++;
		}

		ui->tableWidget->resizeColumnsToContents(); // 自动调整列宽
	}
}

void AgriFieldIdentifyDialog::handleMapShowOrHide(int state)
{
	if (Qt::Unchecked == state) { 
		m_bMapShow = false;
	}
	else if (Qt::Checked == state) {
		m_bMapShow = true;
	}
}

