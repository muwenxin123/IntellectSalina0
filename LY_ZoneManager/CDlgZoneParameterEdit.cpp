#include "CDlgZoneParameterEdit.h"
#include "ui_CDlgZoneParameterEdit.h"
#include "rgba.h"
#include "QCString.h"
#include "IZoneManagerActivator.h"
#include "QDateTime"
#include "QGeoCoordinate"
#include "LY_ZoneManager.h"

CDlgZoneParameterEdit::CDlgZoneParameterEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgZoneParameterEdit)
{
    ui->setupUi(this);

	m_pIZoneManageService = IZoneManagerActivator::Get().getPIZoneManageService();

	CreateTimeListHeader();
	CreatePointListHeader();

	InitTypeCombobox();

	connect(ui->le_qymc, &QLineEdit::editingFinished, [this]
	{
		strcpy(m_stZoneBase.strID, ui->le_qymc->text().toLocal8Bit().data());
		emit(this->sign_lyqymc_changed());
	});
	connect(ui->lineEdit, &QLineEdit::editingFinished, [this]
	{
		m_stZoneBase.stRoundParameter.nNorthAngel = ui->lineEdit->text().toInt();
		emit(this->sign_lyqymc_changed());
	});
	connect(ui->lineEdit_3, &QLineEdit::editingFinished, [this]
	{
		m_stZoneBase.stRoundParameter.nRaidusChang = ui->lineEdit_3->text().toInt();
		emit(this->sign_lyqymc_changed());
	});
	connect(ui->lineEdit_4, &QLineEdit::editingFinished, [this]
	{
		m_stZoneBase.stRoundParameter.nRaidusDuan = ui->lineEdit_4->text().toInt();
		emit(this->sign_lyqymc_changed());
	});

	connect(ui->tw_dj, &QTreeWidget::customContextMenuRequested, this, &CDlgZoneParameterEdit::on_tw_dj_customContextMenuRequested);

	m_treeMenu = new QMenu;
	m_deleteAction = m_treeMenu->addAction(tr2("删除"));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(on_tw_djMenu_delete));

}

CDlgZoneParameterEdit::~CDlgZoneParameterEdit()
{
    delete ui;
}

void CDlgZoneParameterEdit::SetZonebase(const qnzkna::ZoneManage::CZoneBase &stZoneBase, const bool bEdited)
{
	m_stZoneBase = stZoneBase;

	this->setEnabled(bEdited);

	int index = stZoneBase.nZoneType +1;

	ui->lb_yxjd->setVisible(index == 1 || index == 2 || index == 3 || index == 6 || index == 7 );
	ui->le_yxjd->setVisible(index == 1 || index == 2 || index == 3 || index == 6 || index == 7 );
	ui->lb_yxwd->setVisible(index == 1 || index == 2 || index == 3 || index == 6 || index == 7 );
	ui->le_yxwd->setVisible(index == 1 || index == 2 || index == 3 || index == 6 || index == 7 );

	ui->lb_bj->setVisible(index == 1 || index == 2);
	ui->le_bj->setVisible(index == 1 || index == 2);

	ui->lb_qsj->setVisible(index == 2 || index == 7);
	ui->le_qsj->setVisible(index == 2 || index == 7);

	ui->lb_zzj->setVisible(index == 2 || index == 7);
	ui->le_zzj->setVisible(index == 2 || index == 7);

	ui->lb_cbz->setVisible(index == 3);
	ui->le_cbz->setVisible(index == 3);

	ui->lb_dbz->setVisible(index == 3);
	ui->le_dbz->setVisible(index == 3);

	ui->lb_czyzbfxjj->setVisible(index == 3);
	ui->le_czyzbfxjj->setVisible(index == 3);

	ui->lb_chang->setVisible(index == 5 || index == 8);
	ui->le_chang->setVisible(index == 5 || index == 8);

	ui->lb_kuan->setVisible(index == 5 || index == 8);
	ui->le_kuan->setVisible(index == 5 || index == 8);

	ui->lb_kuan_2->setVisible(index == 5);
	ui->lineEdit->setVisible(index == 5);

	ui->lb_kuan_3->setVisible(false);
	ui->lineEdit_2->setVisible(false);

	ui->lb_kuan_4->setVisible(index == 5);
	ui->lineEdit_3->setVisible(index == 5);

	ui->lb_kuan_5->setVisible(index == 5);
	ui->lineEdit_4->setVisible(index == 5);

	ui->lb_wbj->setVisible(index == 6 || index == 7);
	ui->le_wbj->setVisible(index == 6 || index == 7);

	ui->lb_nbj->setVisible(index == 6 || index == 7);
	ui->le_nbj->setVisible(index == 6 || index == 7);

	ui->lb_dj->setVisible(index == 4 || index == 9);
	ui->tw_dj->setVisible(index == 4 || index == 9);

	ui->lb_hlkd->setVisible(index == 9);
	ui->le_hlkd->setVisible(index == 9);

	UpdateCtrlData();
}

void CDlgZoneParameterEdit::SetEdited(bool bEdited)
{
	this->setEnabled(bEdited);
}

void CDlgZoneParameterEdit::UpdateCtrlData()
{
	ui->le_qymc->setText(QString::fromLocal8Bit(m_stZoneBase.strID));

	ui->comboBox->setCurrentIndex(m_stZoneBase.nProperty);

	ui->cb_xx->setPenStyle((Qt::PenStyle)m_stZoneBase.nLineType);

	unsigned int uiV = m_stZoneBase.nLineColor;
	QColor c = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
	ui->pb_xs->setColor(c);

	ui->cb_tc->setBrushStyle((Qt::BrushStyle)m_stZoneBase.nFillType);

	ui->le_sg->setText(QString::number(m_stZoneBase.nAboveAlt));

	ui->le_kydg->setText(QString::number(m_stZoneBase.nBottomhAlt));

	ui->le_yxjd->setText(QString::number(m_stZoneBase.stRoundParameter.dLon, 'f', 7));
	ui->le_yxwd->setText(QString::number(m_stZoneBase.stRoundParameter.dLat, 'f', 7));
	ui->le_bj->setText(QString::number(m_stZoneBase.stRoundParameter.nRadius));
	ui->le_qsj->setText(QString::number(m_stZoneBase.stRoundParameter.nStartAngel));
	ui->le_zzj->setText(QString::number(m_stZoneBase.stRoundParameter.nEndAngel));
	ui->le_cbz->setText(QString::number(m_stZoneBase.stRoundParameter.nRaidusChang));
	ui->le_dbz->setText(QString::number(m_stZoneBase.stRoundParameter.nRaidusDuan));
	ui->le_czyzbfxjj->setText(QString::number(m_stZoneBase.stRoundParameter.nAngelChang));
	ui->le_chang->setText(QString::number(m_stZoneBase.stRoundParameter.dLon, 'f', 7));
	ui->le_kuan->setText(QString::number(m_stZoneBase.stRoundParameter.dLat, 'f', 7));
	ui->lineEdit->setText(QString::number(m_stZoneBase.stRoundParameter.nNorthAngel));
	ui->lineEdit_2->setText(QString::number(m_stZoneBase.stRoundParameter.nCornerRadius));
	ui->lineEdit_3->setText(QString::number(m_stZoneBase.stRoundParameter.nRaidusChang));
	ui->lineEdit_4->setText(QString::number(m_stZoneBase.stRoundParameter.nRaidusDuan));
	ui->le_wbj->setText(QString::number(m_stZoneBase.stRoundParameter.nRaidusChang));
	ui->le_nbj->setText(QString::number(m_stZoneBase.stRoundParameter.nRaidusDuan));
	ui->le_hlkd->setText(QString::number(m_stZoneBase.nLineWidth));
	UpdatePointListData();

	UpdateTimeListData();
}

bool CDlgZoneParameterEdit::isQYMCEmpty()
{
	if (ui->le_qymc->text() == "")
		return true;
	return false;
}

qnzkna::ZoneManage::CZoneBase CDlgZoneParameterEdit::GetZonneBase()
{
	strcpy(m_stZoneBase.strID, ui->le_qymc->text().toLocal8Bit().data());
	m_stZoneBase.nZoneType = m_stZoneBase.nZoneType;
	m_stZoneBase.nProperty = ui->comboBox->currentIndex();
	m_stZoneBase.nLineType = ui->cb_xx->penStyle();
	m_stZoneBase.nFillType = ui->cb_tc->brushStyle();
	QColor d = ui->pb_xs->color();
	unsigned int uiV = (d.blue() & 0xFF) << 16 | (d.green() & 0xFF) << 8 | (d.red() & 0xFF);
	m_stZoneBase.nLineColor = uiV;
	m_stZoneBase.nAboveAlt = ui->le_sg->text().toInt();
	m_stZoneBase.nBottomhAlt = ui->le_kydg->text().toInt();
	m_stZoneBase.stRoundParameter.dLon = ui->le_yxjd->text().toDouble();
	m_stZoneBase.stRoundParameter.dLat = ui->le_yxwd->text().toDouble();
	m_stZoneBase.stRoundParameter.nRadius = ui->le_bj->text().toInt();
	m_stZoneBase.stRoundParameter.nStartAngel = ui->le_qsj->text().toInt();
	m_stZoneBase.stRoundParameter.nEndAngel = ui->le_zzj->text().toInt();
	m_stZoneBase.nLineWidth = ui->le_hlkd->text().toInt();

	switch (m_stZoneBase.nZoneType) {
	case qnzkna::ZoneManage::CZoneBase::ELLIPSE: {
		m_stZoneBase.stRoundParameter.nRaidusChang = ui->le_cbz->text().toInt();
		m_stZoneBase.stRoundParameter.nRaidusDuan = ui->le_dbz->text().toInt();
	}break;
	case qnzkna::ZoneManage::CZoneBase::CIRCLERING: {
		m_stZoneBase.stRoundParameter.nRaidusChang = ui->le_wbj->text().toInt();
		m_stZoneBase.stRoundParameter.nRaidusDuan = ui->le_nbj->text().toInt();
	}break;
	case qnzkna::ZoneManage::CZoneBase::PIERING: {
		m_stZoneBase.stRoundParameter.nRaidusChang = ui->le_wbj->text().toInt();
		m_stZoneBase.stRoundParameter.nRaidusDuan = ui->le_nbj->text().toInt();
	}break;
	case qnzkna::ZoneManage::CZoneBase::RECTANGLE: {
		m_stZoneBase.stRoundParameter.dLon = ui->le_chang->text().toDouble();
		m_stZoneBase.stRoundParameter.dLat = ui->le_kuan->text().toDouble();
		m_stZoneBase.stRoundParameter.nRaidusChang = ui->lineEdit_3->text().toInt();
		m_stZoneBase.stRoundParameter.nRaidusDuan = ui->lineEdit_4->text().toInt();
		m_stZoneBase.stRoundParameter.nNorthAngel = ui->lineEdit->text().toInt();
	}break;
	default:
		break;
	}

	m_stZoneBase.stRoundParameter.nAngelChang = ui->le_czyzbfxjj->text().toInt();
	m_stZoneBase.stZonePointVec.clear();
	int rowCount = ui->tw_dj->rowCount();
	for (int i = 0; i < rowCount; i++) {
		double lon = ui->tw_dj->item(i, 1)->text().toDouble();
		double lat = ui->tw_dj->item(i, 2)->text().toDouble();
		m_stZoneBase.stZonePointVec.emplace_back(lon, lat);
	}

	return m_stZoneBase;
}

void CDlgZoneParameterEdit::CreateTimeListHeader()
{
	ui->tableWidget->clear();
	QStringList headerlabels;
	headerlabels << tr2("序号") << tr2("开始时间") << tr2("结束时间");
	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableWidget->setRowCount(0);

	for (int i = 0; i < headerlabels.size(); i++)
	{
		if (i == 0)
		{
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
		}
		else
		{
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
		}
	}
}

void CDlgZoneParameterEdit::UpdateTimeListData()
{
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);

	if (m_pIZoneManageService == nullptr)
	{
		return;
	}

	qnzkna::ZoneManage::TimeVec  StartTimeVec, EndTimeVec;

	bool bsuccess = m_pIZoneManageService->GetZoneBaseValidTime(QString::fromLocal8Bit(m_stZoneBase.strID), StartTimeVec,EndTimeVec);
	if (bsuccess == false)
	{
		return;
	}
	if (StartTimeVec.size() != EndTimeVec.size())
	{
		return;
	}

	ui->tableWidget->setRowCount(StartTimeVec.size());
	QString strNo, strStartTime, strEndTime;
	int nNo = 0;
	for (int i=0;i<StartTimeVec.size();i++)
	{
		strNo = QString::number(nNo).sprintf("%02d", nNo + 1);
		strStartTime = QDateTime::fromSecsSinceEpoch(StartTimeVec[i]).toString("yyyy-MM-dd hh:mm:ss");;
		strEndTime = QDateTime::fromSecsSinceEpoch(EndTimeVec[i]).toString("yyyy-MM-dd hh:mm:ss");

		QTableWidgetItem* item0 = new QTableWidgetItem(strNo);
		QTableWidgetItem* item1 = new QTableWidgetItem(strStartTime);
		QTableWidgetItem* item2 = new QTableWidgetItem(strEndTime);

		ui->tableWidget->setItem(nNo, 0, item0);
		ui->tableWidget->setItem(nNo, 1, item1);
		ui->tableWidget->setItem(nNo, 2, item2);

		ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		nNo++;
	}
}

void CDlgZoneParameterEdit::CreatePointListHeader()
{
	ui->tw_dj->clear();
	QStringList headerlabels;
	headerlabels << tr2("序号") << tr2("经度") << tr2("纬度");
	ui->tw_dj->setColumnCount(headerlabels.size());
	ui->tw_dj->setHorizontalHeaderLabels(headerlabels);
	ui->tw_dj->verticalHeader()->setHidden(true);
	ui->tw_dj->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tw_dj->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tw_dj->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui->tw_dj->setAlternatingRowColors(true);
	ui->tw_dj->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tw_dj->setRowCount(0);

	for (int i = 0; i < headerlabels.size(); i++)
	{
		if (i == 0)
		{
			ui->tw_dj->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
		}
		else
		{
			ui->tw_dj->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
		}
	}

}

void CDlgZoneParameterEdit::UpdatePointListData()
{
	ui->tw_dj->clearContents();

	ui->tw_dj->setRowCount(m_stZoneBase.stZonePointVec.size());

	QString strNo, strLon, strLat;
	int nNo = 0;
	for (auto itr : m_stZoneBase.stZonePointVec)
	{
		strNo = QString::number(nNo).sprintf("%02d", nNo + 1);
		strLon = QString::number(itr.dLon, 'f', 7);
		strLat = QString::number(itr.dLat, 'f', 7);

		QTableWidgetItem* item0 = new QTableWidgetItem(strNo);
		QTableWidgetItem* item1 = new QTableWidgetItem(strLon);
		QTableWidgetItem* item2 = new QTableWidgetItem(strLat);

		ui->tw_dj->setItem(nNo, 0, item0);
		ui->tw_dj->setItem(nNo, 1, item1);
		ui->tw_dj->setItem(nNo, 2, item2);

		ui->tw_dj->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tw_dj->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tw_dj->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		nNo++;
	}
}

void CDlgZoneParameterEdit::InitTypeCombobox()
{
	QStringList strType;

	strType << tr2("其他") << tr2("禁飞区") << tr2("限飞区") << tr2("危险区") << tr2("起降区")
		<< tr2("作业区") << tr2("航道") << tr2("高盐度盐田(>3.5%)") << tr2("中盐度盐田(2.5%-3.5%)") << tr2("低盐度盐田(<2.5%)");

	ui->comboBox->clear();
	ui->comboBox->addItems(strType);
	ui->le_sg->setText(QString::number(200));
	ui->le_kydg->setText(QString::number(0));
}

void CDlgZoneParameterEdit::setGroupHiden()
{
	ui->groupBox->hide();
}

void CDlgZoneParameterEdit::EditMouseMoveJWD(double ll, double bb)
{
	if (m_stZoneBase.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY || m_stZoneBase.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
		m_stZoneBase.stZonePointVec.emplace_back(ll, bb);
		UpdatePointListData();
	}
	if (m_stZoneBase.nZoneType == qnzkna::ZoneManage::CZoneBase::RECTANGLE) {
		ui->le_chang->setText(QString::number(ll, 'f', 7));
		ui->le_kuan->setText(QString::number(bb, 'f', 7));
	}
	else {
		ui->le_yxjd->setText(QString::number(ll, 'f', 7));
		ui->le_yxwd->setText(QString::number(bb, 'f', 7));
	}
	m_stZoneBase.stRoundParameter.dLon = ll;
	m_stZoneBase.stRoundParameter.dLat = bb;
}

void CDlgZoneParameterEdit::EditMouseMoveRadius(int raduis)
{
	ui->le_bj->setText(QString::number(raduis));
	m_stZoneBase.stRoundParameter.nRadius = raduis;
}

void CDlgZoneParameterEdit::EditMouseMoveSEAngle(int qsj, int zzj)
{
	if (qsj != 0) {
		ui->le_qsj->setText(QString::number(qsj));
		m_stZoneBase.stRoundParameter.nStartAngel = qsj;
	}
	if (zzj != 0) {
		ui->le_zzj->setText(QString::number(zzj));
		m_stZoneBase.stRoundParameter.nEndAngel = zzj;
	}
}

void CDlgZoneParameterEdit::EditMouseMoveCDRadius(int cbz, int dbz)
{
	if (cbz > 0) {
		ui->le_cbz->setText(QString::number(cbz));
		m_stZoneBase.stRoundParameter.nRaidusChang = cbz;
	}
	if (dbz > 0) {
		ui->le_dbz->setText(QString::number(dbz));
		m_stZoneBase.stRoundParameter.nRaidusDuan = dbz;
	}
}

void CDlgZoneParameterEdit::EditMouseMoveWNRadius(int wbj, int nbj)
{
	if (wbj > 0) {
		ui->le_wbj->setText(QString::number(wbj));
		m_stZoneBase.stRoundParameter.nRaidusChang = wbj;
	}
	if (nbj > 0) {
		ui->le_nbj->setText(QString::number(nbj));
		m_stZoneBase.stRoundParameter.nRaidusDuan = nbj;
	}
}

void CDlgZoneParameterEdit::EditMouseMoveChangKuan(int chang, int kuan)
{
	if (chang > 0) {
		ui->lineEdit_3->setText(QString::number(chang));
		m_stZoneBase.stRoundParameter.nRaidusChang = chang;
	}
	if (kuan > 0) {
		ui->lineEdit_4->setText(QString::number(kuan));
		m_stZoneBase.stRoundParameter.nRaidusDuan = kuan;
	}
}

void CDlgZoneParameterEdit::on_tw_djMenu_delete()
{
}

void CDlgZoneParameterEdit::on_tw_dj_customContextMenuRequested(const QPoint &pos)
{
}