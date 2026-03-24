#include "ly_pestmointorwidget.h"
#include "ui_ly_pestmointorwidget.h"
#include "LY_AgricultureHomepage.h"
#include "LyMessageBox.h"
LY_PestMointorWidget::LY_PestMointorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_PestMointorWidget)
{
    ui->setupUi(this);

	qRegisterMetaType<PestMointer>("PestMointer");
	setupUI();
	addSlots();


}

void LY_PestMointorWidget::setupUI()
{
	ui->dryingTem->setText("--.--");
	ui->insecticideTem->setText("--.--");
	ui->illum->setText("--.--");
	ui->voltage->setText("--.--");

	ui->label_41->setPixmap(QPixmap(":/images/mointor/wormMax.png"));

	ui->attractWorm->setCheckable(true);
	ui->rainFlap->setCheckable(true);
	ui->wormFlap->setCheckable(true);
	ui->drying->setCheckable(true);
	ui->moveWorm->setCheckable(true);
	ui->shake->setCheckable(true);
	ui->fillLight->setCheckable(true);
	ui->insecticide->setCheckable(true);

	QStringList recode_header;
	recode_header << tr2("序号") << tr2("地址码") << tr2("操作内容") << tr2("用户名称") << tr2("操作时间");
	ui->recode_tab->setColumnCount(recode_header.size());
	ui->recode_tab->setHorizontalHeaderLabels(recode_header);
	ui->recode_tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->recode_tab->verticalHeader()->setHidden(true);
	ui->recode_tab->setSelectionBehavior(QTableWidget::SelectRows);
	ui->recode_tab->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->recode_tab->setAlternatingRowColors(true);
	ui->recode_tab->setRowCount(0);

	QStringList history_header;
	history_header << tr2("序号") << tr2("设备名称") << tr2("详情") << tr2("记录时间");
	ui->history_tab->setColumnCount(history_header.size());
	ui->history_tab->setHorizontalHeaderLabels(history_header);
	ui->history_tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->history_tab->verticalHeader()->setHidden(true);
	ui->history_tab->setSelectionBehavior(QTableWidget::SelectRows);
	ui->history_tab->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->history_tab->setAlternatingRowColors(true);
	
	ui->history_tab->setRowCount(0);
	ui->tabWidget->setCurrentIndex(0);
	//ui->stackedWidget->setCurrentIndex(0);
	
}
void LY_PestMointorWidget::errorCurrent()
{
	LyMessageBox::information(NULL, tr2("提示"), tr2("设备正在连接"));
}

void LY_PestMointorWidget::getPestData(PestMointer p)
{
	ui->dryingTem->setText(QString::fromStdString(p.dryingTem) + tr2(" ℃"));
	ui->insecticideTem->setText(QString::fromStdString(p.dryingTem) + tr2(" ℃"));
	ui->illum->setText(QString::fromStdString(p.illum) + " Lux");
	ui->voltage->setText(QString::fromStdString(p.voltage) + " V");

}
void LY_PestMointorWidget::getWormOperationLog(QVector <WormOperationLog> vec) {

	ui->recode_tab->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->recode_tab->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui->recode_tab->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
	ui->recode_tab->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
	ui->recode_tab->setRowCount(vec.size());
	if (vec.size() <= 0 )
	{
		return;
	}
	for (size_t i = 0; i < vec.size(); i++)
	{
		WormOperationLog worm = vec.at(i);
		QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i+ 1));
		QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromStdString(worm.deviceAddr));
		QTableWidgetItem *item3 = new QTableWidgetItem(QString::fromStdString(worm.value));
		QTableWidgetItem *item4 = new QTableWidgetItem(QString::fromStdString(worm.userName));
		QTableWidgetItem *item5 = new QTableWidgetItem(QString::fromStdString(worm.createTime));

		ui->recode_tab->setItem(i, 0, item1);
		ui->recode_tab->setItem(i, 1, item2);
		ui->recode_tab->setItem(i, 2, item3);
		ui->recode_tab->setItem(i, 3, item4);
		ui->recode_tab->setItem(i, 4, item5);

		ui->recode_tab->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->recode_tab->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->recode_tab->item(i, 2)->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
		ui->recode_tab->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->recode_tab->item(i, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}
}

void LY_PestMointorWidget::getHistoryData(WormHistToryData historyData)
{
	ui->history_tab->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->history_tab->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	//ui->history_tab->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	ui->history_tab->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
	ui->history_tab->setRowCount(historyData.rows.size());

	if (historyData.rows.size() <= 0 )
	{
		return;
	}
	for (size_t i = 0; i < historyData.rows.size(); i++)
	{
		DeviceGistoryInfo worm = historyData.rows.at(i);
		ui->history_tab->setRowHeight(i, 50);
		QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i + 1));
		QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromStdString(worm.deviceAddr));
		QTableWidgetItem *item3 = new QTableWidgetItem(QString::fromStdString(worm.value));
		QTableWidgetItem *item4 = new QTableWidgetItem(QString::fromStdString(worm.createTime));

		ui->history_tab->setItem(i, 0, item1);
		ui->history_tab->setItem(i, 1, item2);
		ui->history_tab->setItem(i, 2, item3);
		ui->history_tab->setItem(i, 3, item4);

		ui->history_tab->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->history_tab->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->history_tab->item(i, 2)->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
		ui->history_tab->setWordWrap(true);
		ui->history_tab->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	}
	//ui->history_tab->resizeRowsToContents();
}

void LY_PestMointorWidget::reciveDeviceStatus(PEST_DEVICE_STATUS type, bool isOpen)
{
	switch (type)
	{
	case PEST_DEVICE_STATUS_None:
	{

	}
	break;
	case PEST_DEVICE_STATUS_WormFlap:
	{
		m_isAttractWorm = isOpen;
		ui->wormFlap->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	case PEST_DEVICE_STATUS_Sharke:
	{
		m_isShake = isOpen;
		ui->shake->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	case PEST_DEVICE_STATUS_DryingFlap:
	{
		ui->dryingFlap->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	case PEST_DEVICE_STATUS_Insecticide:
	{
		m_isInsecticide = isOpen;
		ui->insecticide->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	case PEST_DEVICE_STATUS_MoveWorm:
	{
		m_isMoveWorm = isOpen;
		ui->moveWorm->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	case PEST_DEVICE_STATUS_Drying:
	{
		m_isDrying = isOpen;
		ui->drying->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	case PEST_DEVICE_STATUS_RainFlap:
	{
		m_isRainFlap = isOpen;
		ui->rainFlap->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	case PEST_DEVICE_STATUS_AttractWorm:
	{
		m_isAttractWorm = isOpen;
		ui->attractWorm->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	case PEST_DEVICE_STATUS_FillLight:
	{
		m_isFillLight = isOpen;
		ui->fillLight->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	case PEST_DEVICE_STATUS_Camera:
	{
		m_isFillLight = isOpen;
		ui->fillLight->setText(isOpen ? tr2("打开") : tr2("关闭"));
	}
	break;
	default:
		break;
	}
}
void LY_PestMointorWidget::addSlots()
{

	//connect(ui->recode_btn, &QPushButton::clicked, this, &LY_PestMointorWidget::getWormOperationLog);
	LY_MointorViewModel *viewModel = LY_AgricultureHomepage::Get().GetViewModel();
	if (viewModel == nullptr)
	{
		return;
	}
	connect(ui->tabWidget, &QTabWidget::currentChanged, [=](int index) {

		qDebug() << "====**" << index;
		if (index == 0)
		{
			ui->stackedWidget->setCurrentIndex(0);
		}
		else if(index == 1)
		{
			viewModel->GetWormDataList();
		}
		
	});
	connect(ui->recode_btn, &QPushButton::clicked, [=]() {
		viewModel->GetWormOperationLog();
		ui->stackedWidget->setCurrentIndex(0);
	});
	connect(ui->history_btn, &QPushButton::clicked, [=]() {
		viewModel->GetWormHistoryData();
		ui->stackedWidget->setCurrentIndex(1);
	});
	connect(viewModel, &LY_MointorViewModel::getHistoryData, this, &LY_PestMointorWidget::getHistoryData, Qt::QueuedConnection);

	connect(viewModel, &LY_MointorViewModel::getReocdeData, this, &LY_PestMointorWidget::getWormOperationLog, Qt::QueuedConnection);

	connect(viewModel, &LY_MointorViewModel::receivePestMointerata, this, &LY_PestMointorWidget::getPestData, Qt::QueuedConnection);

	connect(viewModel, &LY_MointorViewModel::contrlDeviceStatus, this, &LY_PestMointorWidget::reciveDeviceStatus, Qt::QueuedConnection);

	connect(viewModel, &LY_MointorViewModel::errorCurrent, this, &LY_PestMointorWidget::errorCurrent, Qt::QueuedConnection);

	connect(ui->attractWorm, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_AttractWorm, !m_isAttractWorm);
	});
	connect(ui->rainFlap, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_RainFlap, !m_isRainFlap);
	});
	connect(ui->wormFlap, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_WormFlap, !m_isWormFlap);
	});
	connect(ui->drying, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_Drying, !m_isDrying);
	});
	connect(ui->moveWorm, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_MoveWorm, !m_isMoveWorm);
	});
	connect(ui->shake, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_Sharke, !m_isShake);
	});
	connect(ui->dryingFlap, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_DryingFlap, !m_isDryingFlap);
	});
	connect(ui->fillLight, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_FillLight, !m_isFillLight);
	});
	connect(ui->insecticide, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_Insecticide, !m_isInsecticide);
	});
	connect(ui->insecticide, &QPushButton::clicked, [=]() {

		viewModel->controlPestDevice(PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_Insecticide, !m_isInsecticide);
	});


}
LY_PestMointorWidget::~LY_PestMointorWidget()
{
    delete ui;
}
