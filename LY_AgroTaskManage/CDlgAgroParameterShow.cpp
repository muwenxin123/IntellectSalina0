#include "CDlgAgroParameterShow.h"
#include "ui_CDlgAgroParameterShow.h"
#include "QCString.h"
#include "IAgroTaskManagerActivator.h"
#include "LY_AgroTaskManage.h"
#include "LY_AgroTaskManageInterface.h"

CDlgAgroParameterShow::CDlgAgroParameterShow(QWidget *parent) :
	LyDialog(tr2("规则内容"), parent),
	ui(new Ui::CDlgAgroParameterShow)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(960, 900);

	setMaxable(true);

	InitCtrl();
	InitUI();

	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
	connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
	connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));
	connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_4_clicked()));
	connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(on_pushButton_5_clicked()));
	connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(on_pushButton_6_clicked()));
}

CDlgAgroParameterShow::~CDlgAgroParameterShow()
{
    delete ui;
}

void CDlgAgroParameterShow::SetStringName(const QString& strTmp)
{
	ui->label_2->setText(strTmp);
}

void CDlgAgroParameterShow::SetStringNYCS(const QString& strTmp)
{
	ui->textEdit->setText(strTmp);
}

void CDlgAgroParameterShow::InitUI()
{
	QStringList headerlabels;
	headerlabels << tr2("序号") << tr2("经度") << tr2("纬度") << tr2("喷药高度") << tr2("喷药幅宽") << tr2("喷药设备") << tr2("喷嘴类型") << tr2("喷药方式") << tr2("喷药开关") << tr2("转速") << tr2("流速") << tr2("雾滴大小") << tr2("喷嘴大小");
	ui->tableWidget_2->setColumnCount(headerlabels.size());
	ui->tableWidget_2->setHorizontalHeaderLabels(headerlabels);
	for (int i = 0; i < headerlabels.size(); i++)
	{
		if (i == 0 || i == 1 || i == 2)
		{
			ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
		}
		else
		{
			ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
		}
	}
	ui->tableWidget_2->verticalHeader()->setHidden(true);
	ui->tableWidget_2->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget_2->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui->tableWidget_2->setAlternatingRowColors(true);
	ui->tableWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableWidget_2->setRowCount(0);

	headerlabels.clear();
	headerlabels << tr2("") << tr2("杀虫剂") << tr2("杀菌剂") << tr2("调节剂") << tr2("生长肥") << tr2("助剂") << tr2("总药剂量") << tr2("亩用水量") ;
	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableWidget->setRowCount(0);

	ui->route_diration_box->clear();
	ui->route_diration_box->addItem(tr2("东西"));

	localWeaponNameBindToId();
	InitWeaponCombobox();
}

void CDlgAgroParameterShow::InitCtrl()
{
	ui->label_2->setText("");
	ui->textEdit->setText("");
}

void CDlgAgroParameterShow::SetAgriFieldInfo(const AgriFieldInfo& stInfo)
{
	m_FieldInfo = stInfo;

	m_boundPoings.clear();
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = IAgroTaskManagerActivator::Get().getIZoneManageService()->GetAllZoneBase();
	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == m_FieldInfo.area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			double curLon = itrZone.stRoundParameter.dLon;
			double curLat = itrZone.stRoundParameter.dLat;

			if (itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
				itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
				if (itrZone.stZonePointVec.size() > 0) {
					curLon = itrZone.stZonePointVec[0].dLon;
					curLat = itrZone.stZonePointVec[0].dLat;

					for (auto zone : itrZone.stZonePointVec)
					{
						QPointF pointF;
						pointF.setX(zone.dLon);
						pointF.setY(zone.dLat);
						m_boundPoings.push_back(pointF);
					}
				}
			}
		}
	}

	ui->spray_name_comBox_2->disconnect();
	InitSprayPrescriptionCombobox();
	ui->spray_name_comBox_2->setCurrentIndex(m_pescriptionVec.size() - 1);
	connect(ui->spray_name_comBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(on_spray_name_comBox_2_currentIndexChanged(int)));

	//生成处方图
	GenerateSprayPrescriptionDiagram();
}

void CDlgAgroParameterShow::SetDrugRatioVec(const CDrugRatioVec& RatioVec)
{
	m_DrugRatioVec = RatioVec;

	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(2);

	int index = 1;
	ui->tableWidget->setRowCount(2);
	QTableWidgetItem *item11 = new QTableWidgetItem(tr2("药剂名称"));
	QTableWidgetItem *item12 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item13 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item14 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item15 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item16 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item17 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item18 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item21 = new QTableWidgetItem(tr2("用量(ml/g)"));
	QTableWidgetItem *item22 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item23 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item24 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item25 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item26 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item27 = new QTableWidgetItem(tr2("-"));
	QTableWidgetItem *item28 = new QTableWidgetItem(tr2("-"));

	ui->tableWidget->setItem(0, 0, item11);
	ui->tableWidget->setItem(0, 1, item12);
	ui->tableWidget->setItem(0, 2, item13);
	ui->tableWidget->setItem(0, 3, item14);
	ui->tableWidget->setItem(0, 4, item15);
	ui->tableWidget->setItem(0, 5, item16);
	ui->tableWidget->setItem(0, 6, item17);
	ui->tableWidget->setItem(0, 7, item18);
	ui->tableWidget->setItem(1, 0, item21);
	ui->tableWidget->setItem(1, 1, item22);
	ui->tableWidget->setItem(1, 2, item23);
	ui->tableWidget->setItem(1, 3, item24);
	ui->tableWidget->setItem(1, 4, item25);
	ui->tableWidget->setItem(1, 5, item26);
	ui->tableWidget->setItem(1, 6, item27);
	ui->tableWidget->setItem(1, 7, item28);

	int nRatioCount = 0;
	for (auto itr :m_DrugRatioVec)
	{
		if (itr.nType == 0)
		{
			item12->setText(itr.stDrugName);
			item22->setText(QString::number(itr.nRaito));
		}
		else if (itr.nType == 1)
		{
			item13->setText(itr.stDrugName);
			item23->setText(QString::number(itr.nRaito));
		}
		else if (itr.nType == 2)
		{
			item14->setText(itr.stDrugName);
			item24->setText(QString::number(itr.nRaito));
		}
		else if (itr.nType == 3)
		{
			item15->setText(itr.stDrugName);
			item25->setText(QString::number(itr.nRaito));
		}
		else if (itr.nType == 4)
		{
			item16->setText(itr.stDrugName);
			item26->setText(QString::number(itr.nRaito));
		}
		nRatioCount += itr.nRaito;
	}
	item27->setText(QString::number(nRatioCount));
	item28->setText(QString::number(1000 - nRatioCount));

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0;j < 8;j++)
		{
			ui->tableWidget->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
	}
}

void CDlgAgroParameterShow::draw2DPathLine(QPainter &painter, IProj *pProj)
{
	if (m_pescription.data_source_id.length() > 0)
	{

		//绘制二维处方图
		if (m_spayMapshow == true)
		{
			IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->create2DSprayDiagram(painter, pProj);
		}

		//绘制二维航线
		if (m_isGeneratingCourse == true)
		{
			QVector<QPoint> startPoints;
			QVector<QPoint> middlePoints;
			QVector<QPoint> endPoints;
			QPen pen(Qt::blue);
			pen.setWidth(3);
			painter.setPen(pen);

			m_gridPoints.clear();
			for (const auto &it : m_flyPoints)
			{
				QPoint point1 = pProj->Jwd2Screen(it.lon, it.lat);
				m_gridPoints.push_back(point1);
			}

			for (int i = 0; i < m_gridPoints.size() - 1; ++i) 
			{
				painter.drawLine(m_gridPoints[i], m_gridPoints[i + 1]);
			}

			// 绘制航线点标记
			painter.setPen(Qt::white);
			painter.setBrush(Qt::white);
			for (const QPointF &point : m_gridPoints) {
				painter.drawEllipse(point, 2, 2);
			}
			if (m_isInstructionStatus)
			{
				//开始的点
				painter.setPen(Qt::yellow);
				painter.setBrush(Qt::yellow);

				for (int i = 0; i < m_airInstructionPoints.size(); i++)
				{
					AirInstructionPoi it = m_airInstructionPoints[i];
					QPoint point1 = pProj->Jwd2Screen(it.startLon,
						it.centerLat);
					startPoints.push_back(point1);
				}
				for (const QPointF &point : startPoints) {
					//painter.drawEllipse(point, 2, 2);
					painter.drawText(point, tr2("开"));
					painter.drawEllipse(point, 2, 2);
				}

				//中间的点
				painter.setPen(Qt::white);
				painter.setBrush(Qt::white);

				for (int i = 0; i < m_airInstructionPoints.size(); i++)
				{
					AirInstructionPoi it = m_airInstructionPoints[i];
					QPoint point1 = pProj->Jwd2Screen(it.middleLon,
						it.centerLat);
					middlePoints.push_back(point1);
				}
				for (const QPointF &point : middlePoints) 
				{
					//painter.drawEllipse(point, 2, 2);
					painter.drawEllipse(point, 2, 2);
					painter.drawText(point, tr2("调整"));
				}
				//结束的点
				painter.setPen(Qt::red);
				painter.setBrush(Qt::red);

				for (int i = 0; i < m_airInstructionPoints.size(); i++)
				{
					AirInstructionPoi it = m_airInstructionPoints[i];
					QPoint point1 = pProj->Jwd2Screen(it.endLon,
						it.centerLat);
					endPoints.push_back(point1);
				}
				for (const QPointF &point : endPoints) 
				{
					//painter.drawEllipse(point, 2, 2);
					painter.drawText(point, tr2("关"));
					painter.drawEllipse(point, 2, 2);
				}
			}
		}
	}
}

void CDlgAgroParameterShow::localWeaponNameBindToId()
{
	if (IAgroTaskManagerActivator::Get().getPWeaponManageService() == nullptr)
	{
		return;
	}

	m_SystemSysNameAndID.clear();
	qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
	IAgroTaskManagerActivator::Get().getPWeaponManageService()->GetWeaponSystemMap(weaponSystemMap);
	for (auto sysItr : weaponSystemMap)
	{
		if (sysItr.second.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_UAV)
		{
			auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
			for (auto weapon : *weaponComPonentMap)
			{
				m_SystemSysNameAndID.insert(std::make_pair(weapon.second.getComponentID(), weapon.second.getComponentName()));
			}
		}
	}
}

void CDlgAgroParameterShow::InitWeaponCombobox()
{
	ui->drone_box->clear();

	if (m_SystemSysNameAndID.size() == 0)
	{
		return;
	}

	int nNo = 0;
	for (auto itr : m_SystemSysNameAndID)
	{
		ui->drone_box->insertItem(nNo, QString::fromStdString(itr.second));
		ui->drone_box->setItemData(nNo, QString::fromStdString(itr.first));
		nNo++;
	}
}

void CDlgAgroParameterShow::AddLineToServer(const std::vector<Poi>& LinePoints)
{
	if (IAgroTaskManagerActivator::Get().getIZoneManageService() == nullptr)
	{
		return;
	}

	CAirTrafficLine stLine;
	stLine.nID = IAgroTaskManagerActivator::Get().getIZoneManageService()->GetAirTrafficLineMaxID();
	QString strTmp = tr2("HX") + QString::number(stLine.nID);
	strncpy(stLine.strName, strTmp.toLocal8Bit(), MAX_AIR_LEN_1);
	if (ui->drone_box->currentIndex() == -1)
	{
		return;
	}
	QVariant  qv = ui->drone_box->itemData(ui->drone_box->currentIndex());
	QString strUAVID = qv.toString();
	strncpy(stLine.strUAVID, strUAVID.toLocal8Bit(), MAX_AIR_LEN_1);

	int nHeight = ui->spinBox_3->value();
	int nSpeed = ui->spinBox_4->value();
	int nPontID = IAgroTaskManagerActivator::Get().getIZoneManageService()->GetAirTrafficLinePointMaxID();

	int nCount = 0;
	for (auto itr : LinePoints)
	{
		CAirTrafficLinePoint stPoint;
		stPoint.nNo = nPontID + nCount;
		stPoint.nLineID = stLine.nID;
		stPoint.dLon = itr.lon;
		stPoint.dLat = itr.lat;
		stPoint.dAlt = nHeight;
		stPoint.dSpeed = nSpeed;
		stPoint.dHoverTime = 0;

		stLine.LinePointVec.push_back(stPoint);
		nCount++;
	}

	IAgroTaskManagerActivator::Get().getIZoneManageService()->AddAirTrafficLine(stLine);
}

void CDlgAgroParameterShow::addTaskSummary()
{
	ui->edit_width_5->setText(QString::number(50));

	double allValue = 0;
	for (int i = 0; i < m_pescription.leve_value.size(); i++)
	{
		allValue += m_pescription.leve_value[i];
	}
	ui->edit_width_6->setText(QString::number(allValue, 'f', 3));
}

void CDlgAgroParameterShow::SetWorkTime(const long long & llTime1, const long long & llTime2)
{
	m_StartTime = llTime1;
	m_EndTIme = llTime2;

	QDateTime qTime = QDateTime::fromSecsSinceEpoch(llTime1);
	ui->start_time_2->setDateTime(qTime);
	qTime = QDateTime::fromSecsSinceEpoch(llTime2);
	ui->end_time_2->setDateTime(qTime);
}

void CDlgAgroParameterShow::InitSprayPrescriptionCombobox()
{
	m_pescriptionVec.clear();
	m_pescriptionVec = IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->GetSprayPrescription(m_FieldInfo.id);

	int noNumber(0);
	ui->spray_name_comBox_2->clear();
	for (const auto &item : m_pescriptionVec)
	{
		ui->spray_name_comBox_2->insertItem(noNumber, item.map_name);
		ui->spray_name_comBox_2->setItemData(noNumber, item.map_name);
		noNumber++;
	}
}

void CDlgAgroParameterShow::hideEvent(QHideEvent *event)
{
	InitCtrl();
	m_spayMapshow = false;
	m_isGeneratingCourse = false;
	m_isInstructionStatus = false;
	IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->showSprayDiagramData(m_spayMapshow);
}

void CDlgAgroParameterShow::GenerateSprayPrescriptionDiagram()
{
	auto itr = m_pescriptionVec.begin();
	if (itr + ui->spray_name_comBox_2->currentIndex() != m_pescriptionVec.end())
	{
		itr += ui->spray_name_comBox_2->currentIndex();
		m_pescription = *itr;
	}

	IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->generateSprayPrescriptionDiagram(m_boundPoings, m_pescription.data_source_id, double(m_pescription.pyfk), m_pescription.map_level, false);
	IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->showSprayDiagramData(true);
	m_spayMapshow = true;

	//生成航线
	double flyDIsance = 0.0;
	m_flyPoints.clear();
	IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->generateSprayDiagramFlyLine(
		m_boundPoings, m_pescription.data_source_id, m_pescription.pyfk, ui->spinBox_3->value(), false, m_flyPoints, flyDIsance);
	ui->flight_point->setText(QString::number(m_flyPoints.size()));
	ui->flight_distance->setText(QString::number(flyDIsance, 'f', 2) + "m");
	//上传航线
	AddLineToServer(m_flyPoints);

	//生成指令集
	addTaskSummary();
	int nPYGD = ui->spinBox_2->value();
	double nPYFK = ui->spinBox->value();
	QString strUAVName = ui->drone_box->currentText();
	QString strPYLX = tr2("离心喷嘴");
	QString strPYFS = tr2("喷药方式");
	int nPYZS = 10000;
	int nPYLS = 60;
	int nWDDX = 60;
	m_airInstructionPoints = IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->generateInstructionPoint(nPYFK);
	//isInstructionStatus = true;

	ui->tableWidget_2->clearContents();
	ui->tableWidget_2->setRowCount(0);
	int index = 1;
	ui->tableWidget_2->setRowCount(m_airInstructionPoints.size());
	int row = 0;
	IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->DeleteTaskInstruction();
	for (const auto &poi : m_airInstructionPoints)
	{
		QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(index));
		QTableWidgetItem *item2;
		AgriTaskInstruction  Instruction;
		switch (poi.point_status)
		{
		case POINT_STATUS_START:
		{
			item2 = new QTableWidgetItem(QString::number(poi.startLon, 'f', 7));
			Instruction.lon = QString::number(poi.startLon, 'f', 7).toDouble();
		}
		break;
		case POINT_STATUS_MIDDLE:
		{
			item2 = new QTableWidgetItem(QString::number(poi.middleLon, 'f', 7));
			Instruction.lon = QString::number(poi.middleLon, 'f', 7).toDouble();
		}
		break;
		case POINT_STATUS_END:
		{
			item2 = new QTableWidgetItem(QString::number(poi.endLon, 'f', 7));
			Instruction.lon = QString::number(poi.endLon, 'f', 7).toDouble();
		}
		break;
		default:
			break;
		}

		QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(poi.centerLat, 'f', 7));
		QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(nPYGD));
		QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(nPYFK));
		QTableWidgetItem *item6 = new QTableWidgetItem(strUAVName);
		QTableWidgetItem *item7 = new QTableWidgetItem(strPYLX);
		QTableWidgetItem *item8 = new QTableWidgetItem(strPYLX);
		QTableWidgetItem *item9 = new QTableWidgetItem(poi.isOpen ? tr2("打开") : tr2("关闭"));
		double value = QString::number(poi.openValue, 'f', 2).toDouble();
		double speed = nPYZS * 1.0  * value;
		QTableWidgetItem *item10 = new QTableWidgetItem(QString::number(value * speed, 'f', 2));
		double ls = nPYLS * 1.0  *  value;
		QTableWidgetItem *item11 = new QTableWidgetItem(QString::number(value * ls, 'f', 2));
		double wd = nWDDX * 1.0 * value;
		QTableWidgetItem *item12 = new QTableWidgetItem(QString::number(value * wd, 'f', 2));
		QTableWidgetItem *item13 = new QTableWidgetItem(QString::number(value, 'f', 2));

		ui->tableWidget_2->setItem(row, 0, item1);
		ui->tableWidget_2->setItem(row, 1, item2);
		ui->tableWidget_2->setItem(row, 2, item3);
		ui->tableWidget_2->setItem(row, 3, item4);
		ui->tableWidget_2->setItem(row, 4, item5);
		ui->tableWidget_2->setItem(row, 5, item6);
		ui->tableWidget_2->setItem(row, 6, item7);
		ui->tableWidget_2->setItem(row, 7, item8);
		ui->tableWidget_2->setItem(row, 8, item9);
		ui->tableWidget_2->setItem(row, 9, item10);
		ui->tableWidget_2->setItem(row, 10, item11);
		ui->tableWidget_2->setItem(row, 11, item12);
		ui->tableWidget_2->setItem(row, 12, item13);

		ui->tableWidget_2->item(row, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		if (item2 != nullptr)
		{
			ui->tableWidget_2->item(row, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		ui->tableWidget_2->item(row, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 8)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 10)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 11)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget_2->item(row, 12)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		Instruction.spray_guid = m_pescription.guid;
		Instruction.point_status = poi.point_status;
		Instruction.lat = QString::number(poi.centerLat, 'f', 7).toDouble();
		Instruction.pygd = nPYGD;
		Instruction.pyfk = nPYFK;
		Instruction.pysb = strUAVName;
		Instruction.pzlx = strPYLX;
		Instruction.pyfs = strPYFS;
		Instruction.pykg = poi.isOpen ? tr2("打开") : tr2("关闭");
		Instruction.rote_speed = value * speed;
		Instruction.flow_speed = value * ls;
		Instruction.droplet_size = value * wd;
		Instruction.penzui_size = QString::number(poi.openValue, 'f', 1).toDouble() * 100;
		IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->AddTaskInstruction(Instruction);

		index++;
		row++;
	}
}

void CDlgAgroParameterShow::on_pushButton_clicked()
{
	InitCtrl();
	this->hide();
}

void CDlgAgroParameterShow::on_pushButton_4_clicked()
{
	m_spayMapshow = !m_spayMapshow;
	IAgroTaskManagerActivator::Get().GetPIAgriSprayPrescriptionService()->showSprayDiagramData(m_spayMapshow);
}

void CDlgAgroParameterShow::on_pushButton_3_clicked()
{
	m_isGeneratingCourse = !m_isGeneratingCourse;
}

void CDlgAgroParameterShow::on_pushButton_6_clicked()
{
	m_isInstructionStatus = !m_isInstructionStatus;
}

void CDlgAgroParameterShow::on_pushButton_5_clicked()
{
	GenerateSprayPrescriptionDiagram();
}

void CDlgAgroParameterShow::on_pushButton_2_clicked()
{
	LY_AgroTaskManageInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50009, nullptr);
}

void CDlgAgroParameterShow::on_spray_name_comBox_2_currentIndexChanged(const int &arg1)
{
	GenerateSprayPrescriptionDiagram();
}
