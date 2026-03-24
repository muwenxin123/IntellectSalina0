#include "LY_AgriSprayCommandWidget.h"
#include "ui_LY_AgriSprayCommandWidget.h"
#include "LY_AgriSprayPrescriptionActivator.h"
#include <QFont>
#include <QTableWidget>
#include <QDebug>
#include <QStringList>
#include  "QCString.h"

LY_AgriSprayCommandWidget::LY_AgriSprayCommandWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_AgriSprayCommandWidget)
{
    ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	//setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);

	this->setStyleSheet("background-color: #162130};");
	ui->widget->setObjectName("parentWidget");
	ui->widget->setStyleSheet("QWidget#parentWidget {background-image: url(:/images/AgriFarm/biaopaiBgk.png)};");

	setupUI();
}
void LY_AgriSprayCommandWidget::setupUI()
{
	QStringList headerlabels;
	headerlabels << tr2("经度") << tr2("纬度")  << tr2("喷头开关") << tr2("喷嘴大小");
	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	//ui->tableWidget->horizontalHeader()->setFixedHeight(40);
	ui->tableWidget->horizontalHeader()->setStyleSheet(
		"QHeaderView::section {"
		"    height: 40px;"           // 表头高度
		"    font-size: 18px;"        // 文字大小
		"}"
	);
	ui->tableWidget->setRowCount(0);

	m_titleLabel = new QLabel(this);
	m_titleLabel->setStyleSheet("font-size: 30px;");
	m_titleLabel->setText(tr2(""));
	m_titleLabel->move(96, 25);
	m_titleLabel->setFixedSize(135,32);

	ui->label->setAlignment(Qt::AlignTop);
	ui->label_4->setAlignment(Qt::AlignTop);
	ui->label_3->setWordWrap(true);     
	ui->label_3->setFixedWidth(390);
	ui->label_4->setWordWrap(true);
	//ui->label_3->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	ui->label->setStyleSheet("font-size: 20px;");
	ui->label_2->setStyleSheet("font-size: 20px;");
	ui->label_3->setStyleSheet("font-size: 20px;");
	ui->label_4->setStyleSheet("font-size: 20px;");
	ui->label_5->setStyleSheet("font-size: 20px;");
	ui->label_6->setStyleSheet("font-size: 20px;");
	ui->label_8->setStyleSheet("font-size: 20px;");
	ui->label_7->setStyleSheet("font-size: 20px;");

	QFont font = ui->tableWidget->font();
	font.setPointSize(40);
	ui->tableWidget->setFont(font);

	setupPenYaoIMage(ui->label_11,":/images/Legend/1234_01.png");
	setupPenYaoIMage(ui->label_12, ":/images/Legend/1234_02.png");
	setupPenYaoIMage(ui->label_10, ":/images/Legend/1234_03.png");
	setupPenYaoIMage(ui->label_9, ":/images/Legend/1234_04.png");

	 QVector<QVector<QString>> m_growthListMap = {
		{
			{ "#8F1D1D",  "1.7", "10.8" },
			{ "#E53935",  "1.2", "7.6" },
			{ "#FF9800",  "1.5", "9.5" },
			{ "#CDDC39",  "1.3", "8.3" },
			{ "#4CAF50",  "2.0", "12.7" },
		} 
	};

	 QStringList headerlabels1;
	 headerlabels1 << tr2("颜色") << tr2("面积(亩)") << tr2("施肥量（L）");
	 ui->tableWidget_2->setColumnCount(headerlabels1.size());
	 ui->tableWidget_2->setHorizontalHeaderLabels(headerlabels1);
	 ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	 ui->tableWidget_2->verticalHeader()->setHidden(true);
	 ui->tableWidget_2->setSelectionBehavior(QTableWidget::SelectRows);
	 ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
	 ui->tableWidget_2->setEditTriggers(QAbstractItemView::DoubleClicked);
	 ui->tableWidget_2->setAlternatingRowColors(true);
	 ui->tableWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);
	 //ui->tableWidget->horizontalHeader()->setFixedHeight(40);
	 ui->tableWidget_2->horizontalHeader()->setStyleSheet(
		 "QHeaderView::section {"
		 "    height: 40px;"           // 表头高度
		 "    font-size: 18px;"        // 文字大小
		 "}"
	 );
	 ui->tableWidget_2->setRowCount(0);
	 updateLegendTable(ui->tableWidget_2, m_growthListMap);
}
void LY_AgriSprayCommandWidget::setIsSpray(bool isSpray)
{
	if (isSpray)
	{
		m_titleLabel->setText(tr2("喷药任务"));
		ui->label_3->setText(tr2("检测到较高玉米螟虫害风险（0.68），建议使用以下药物配比进行无人机喷药，尽快执行防控。"));

		ui->label_4->setText(tr2(" 30%苯甲·丙环唑：192.5毫升 \n") +
			tr2("20 % 氯虫苯甲酰胺：77毫升 \n") + 
			tr2("5 % 甲维盐：231毫升 \n") +
			tr2(" 0.01%芸苔素内酯：77毫升 \n") +
			tr2(" 98 % 磷酸二氢钾：770克（先溶解） \n") +
			tr2(" 流体硼：231毫升 \n") +
			tr2(" 有机硅助剂：77毫升 \n") +
			tr2(" 清水：补足至17.32升"));
		ui->label_6->setText(tr2("48.6升"));
	}
	else
	{
		m_titleLabel->setText(tr2("施肥任务"));
		ui->label_3->setText(tr2("玉米进入大喇叭口，检测显示NDVI偏低，生物量不足，建议补施肥料以促进粒重形成。"));
		ui->label_4->setText(tr2(" 高氮高钾型水溶肥：40毫升 \n") +
			tr2("0.3%~0.5% 磷酸二氢钾：40毫升 \n") +
			tr2("5 % 甲维盐：231毫升 \n") +
			tr2(" 0.2%硫酸锌（20毫升/亩）+0.1%硼砂（10毫升/亩） \n") +
			tr2(" 氨基酸叶面肥：40毫升 \n") +
			tr2(" 有机硅助剂：10毫升 \n") +
			tr2(" 清水：补足至17.32升"));
		ui->label_6->setText(tr2("49.2升"));
	}
}
void LY_AgriSprayCommandWidget::hideEvent(QHideEvent *event)
{
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);
}
void LY_AgriSprayCommandWidget::setData(std::vector< AirInstructionPoi> airInstructionPoints)
{
//	ui->tableWidget->setStyleSheet("QTableWidget { height: 70px; }");

	//QFont font1;
	//font1.setPointSize(14);
	//ui->label->setFont(font1);
	//ui->label_2->setFont(font1);
	//ui->label_3->setFont(font1);
	//ui->label_4->setFont(font1);
	//ui->label_5->setFont(font1);
	//ui->label_6->setFont(font1);

	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(1);
	int index = 1;
	ui->tableWidget->setRowCount(airInstructionPoints.size());
	//ui->tableWidget->clearContents();
	
	
	QFont font;
	font.setPointSize(8);


	for (size_t i = 0; i < airInstructionPoints.size(); i++)
	{
		const auto&poi = airInstructionPoints.at(i);
		ui->tableWidget->insertRow(i);
		

		QTableWidgetItem *item1;

		AgriTaskInstruction  Instruction;
		switch (poi.point_status)
		{
		case POINT_STATUS_START:
		{
			item1 = new QTableWidgetItem(QString::number(poi.startLon, 'f', 7));
			Instruction.lon = QString::number(poi.startLon, 'f', 7).toDouble();
		}
		break;
		case POINT_STATUS_MIDDLE:
		{
			item1 = new QTableWidgetItem(QString::number(poi.middleLon, 'f', 7));
			Instruction.lon = QString::number(poi.middleLon, 'f', 7).toDouble();
		}
		break;
		case POINT_STATUS_END:
		{
			item1 = new QTableWidgetItem(QString::number(poi.endLon, 'f', 7));
			Instruction.lon = QString::number(poi.endLon, 'f', 7).toDouble();
		}
		break;
		default:
			break;
		}
		//QFont font = item1->font();
		//font.setPointSize(20);  // 设置字体大小为14
		item1->setFont(font);

		QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(poi.centerLat, 'f', 7));
		item2->setFont(font);
		QTableWidgetItem *item3 = new QTableWidgetItem(poi.isOpen ? tr2("打开") : tr2("关闭"));
		item3->setFont(font);
		QString value = QString::number(poi.openValue, 'f', 2);
		QTableWidgetItem *item4 = new QTableWidgetItem(value);
		item4->setFont(font);
		ui->tableWidget->setItem(i, 0, item1);
		ui->tableWidget->setItem(i, 1, item2);
		ui->tableWidget->setItem(i, 2, item3);
		ui->tableWidget->setItem(i, 3, item4);


		ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		ui->tableWidget->setRowHeight(i, 40);
	}
}
void LY_AgriSprayCommandWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		QApplication::setOverrideCursor(Qt::OpenHandCursor);
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		m_dragging = true;
	}
}

void LY_AgriSprayCommandWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (m_dragging && (event->buttons() & Qt::LeftButton)) {
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		move(event->globalPos() - m_dragPosition);
	}
}

void LY_AgriSprayCommandWidget::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	QApplication::setOverrideCursor(Qt::ArrowCursor);
	m_dragging = false;
}

LY_AgriSprayCommandWidget::~LY_AgriSprayCommandWidget()
{
    delete ui;
}
void LY_AgriSprayCommandWidget::setupPenYaoIMage(QLabel *label, const QString imgUrl)
{
	QImage Image;
	//Image.load(":/images/Legend/ferili.png");
	Image.load(imgUrl);
	QPixmap pixmap = QPixmap::fromImage(Image);
	label->setFixedSize(QSize(100, 100));
	int width = label->width();
	int height = label->height();
	QPixmap fitpixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	label->setPixmap(fitpixmap);

}
void LY_AgriSprayCommandWidget::updateLegendTable(QTableWidget *table, const QVector<QVector<QString>> &growthList)
{
	table->setRowCount((int)growthList.size());

	for (int row = 0; row < growthList.size(); ++row)
	{
		auto& growth = growthList[row];

		QWidget *tmpWidget = new QWidget();
		QFrame *colorBox = new QFrame(tmpWidget);
		colorBox->setFixedSize(30, 30);
		colorBox->setFrameShape(QFrame::StyledPanel);
		colorBox->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(growth[0]));

		QVBoxLayout* vLayout = new QVBoxLayout(colorBox);
		colorBox->setLayout(vLayout);
		vLayout->setContentsMargins(0, 0, 0, 0);

		// 创建一个容器 widget 用于居中布局
		QWidget* container = new QWidget();
		QHBoxLayout* hLayout = new QHBoxLayout(container);
		hLayout->addStretch();
		hLayout->addWidget(colorBox);
		hLayout->addStretch();
		hLayout->setContentsMargins(0, 0, 0, 0);
		container->setLayout(hLayout);

		table->setCellWidget(row, 0, container);

		QTableWidgetItem* growthItem = new QTableWidgetItem(growth[1]);
		growthItem->setTextAlignment(Qt::AlignCenter);
		table->setItem(row, 1, growthItem);

		QTableWidgetItem* percentItem = new QTableWidgetItem(growth[2]);
		percentItem->setTextAlignment(Qt::AlignCenter);
		table->setItem(row, 2, percentItem);

	}

	//ui->tableWidget->resizeColumnsToContents();
}
