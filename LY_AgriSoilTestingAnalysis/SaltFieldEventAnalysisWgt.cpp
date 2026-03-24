#include <QApplication>
#include <QHeaderView>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QSettings>
#include <QCheckBox>
#include <QBuffer>

#include "CreateShape.h"
#include "ShapeClip.h"
#include "DrawShape.h"
#include "LyMessageBox.h"
#include "SaltFieldEventAnalysisWgt.h"
#include "ui_SaltFieldEventAnalysisWgt.h"
#include "LY_AgriSoilAnalysisPredict.h"
#include "AgriSoilTestingDataSetDialog.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "AgriSoilAnalysisPredictAlgorithm.h"
#include "LY_AgriSoilAnalysisPredictInterface.h"
#include "LY_AgriSoilAnalysisPredict.h"

#define  EVENT_TYPE_COLUMN  2
#define  TIME_COLUMN  5
#define  LON_COLUMN  6
#define  LAT_COLUMN  7
#define  PICTURE_COLUMN  8
#define  SHOW_FLAG_COLUMN  10

SaltFieldEventAnalysisWgt::SaltFieldEventAnalysisWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SaltFieldEventAnalysisWgt),
    m_show(false)
{
    ui->setupUi(this);

	m_pDetaiWidget = new SaltFieldEventAnalysisDetailWgt();
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_pDetaiWidget);
	ui->distribute_widget->setLayout(layout);
	m_pDetaiWidget->hide();

	//土地图标
	m_sIconPath = (":/images/mointor/");

	Init();

	// 树叶子节点点击
	connect(ui->field_tree_widget, &AgriFieldTree::LeafNodeClicked, [this](QTreeWidgetItem *item, int column) {

		if (item && item->parent() == nullptr)
		{
			AgriFieldInfo fieldInfo = item->data(0, Qt::UserRole + 1).value<AgriFieldInfo>();
			if (fieldInfo.id.length())
			{
				m_pFieldInfo = std::make_shared<AgriFieldInfo>(fieldInfo);

				// 更新表格展示数据
				UpdateTableData();
			}
		}
	});

	connect(ui->tableView, &QTableView::customContextMenuRequested, [=](const QPoint & pos) {
		m_tableMenu->exec(QCursor::pos());
	});

	m_tableMenu = new QMenu(this);
	m_tableAnakysis = m_tableMenu->addAction(tr2("重分析"));
	m_tableView = m_tableMenu->addAction(tr2("查看"));
	m_tableDelete = m_tableMenu->addAction(tr2("删除"));

	connect(m_tableAnakysis, SIGNAL(triggered()), this, SLOT(on_Menu_Anakysis()));
	connect(m_tableView, &QAction::triggered, this, [=]() {
		ViewImage("");
	});
	connect(m_tableDelete, &QAction::triggered, this, [=]() {
		
	});
}

void SaltFieldEventAnalysisWgt::ViewImage(QString fileName)
{
	//获取图片大小，传输参数。
	if (nullptr != LY_AgriSoilAnalysisPredict::Get().GetISaltDetailedInformation()
		&& LY_AgriSoilAnalysisPredict::Get().GetISaltDetailedInformation()->isHidden())
	{
		LY_AgriSoilAnalysisPredict::Get().GetISaltDetailedInformation()->Init(640,640);
		LY_AgriSoilAnalysisPredict::Get().GetISaltDetailedInformation()->show();
	}
}

SaltFieldEventAnalysisWgt::~SaltFieldEventAnalysisWgt()
{
    delete ui;
}


AgriFieldTree* SaltFieldEventAnalysisWgt::getAgriFieldTree()
{
	return ui->field_tree_widget;
}

qnzkna::ZoneManage::CZoneBase SaltFieldEventAnalysisWgt::GetActiveFieldZone()
{
	if (ui->field_tree_widget)
	{
		return ui->field_tree_widget->GetActiveFieldZone();
	}
	return qnzkna::ZoneManage::CZoneBase();
}

void SaltFieldEventAnalysisWgt::UpdataCurActiveFarm()
{
	if (ui->field_tree_widget)
	{
		ui->field_tree_widget->SwitchActiveFarm();
	}
}

void SaltFieldEventAnalysisWgt::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (!m_show)
	{
		return;
	}

	m_soiltestingDrawMap.iterate_query([&](const std::string& id, const SaltFieldEventInfoPtr& eventInfo) {

			QPoint point = pProj->Jwd2Screen(eventInfo->lon, eventInfo->lat);

			QPixmap pixmap;
			if (pixmap.load(m_sIconPath + "pest_warning.png"))
			{
				painter.drawPixmap(point.x() - 15, point.y() - 15, 30, 30, pixmap);
			}
	});

	// 更新区域高亮
	ui->field_tree_widget->ZoneHighLight(painter, pProj);
}

void SaltFieldEventAnalysisWgt::On2d3dChanged(int wParam)
{
}

void SaltFieldEventAnalysisWgt::AddEvent(SaltFieldEventInfo& soilTestingDataset)
{
	auto ptr = std::make_shared<SaltFieldEventInfo>(soilTestingDataset);
	if (LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->AddSaltFieldEventDataset(ptr))
	{
		AppendOneRowTotable(ptr);
	}
}

QString SaltFieldEventAnalysisWgt::GetAreaID(double lon, double lat)
{
	QString id = 0;
	QSet<QString> areaIDs;
	AgriFieldInfoVec vecFieldList;
	if (nullptr != AgriFarmManagerService())
	{
		AgriFarmManagerService()->GetFieldInfoList(vecFieldList);
	}
	for (auto& fieldInfo : vecFieldList)
	{
		if (ui->field_tree_widget->GetCurrentFarm() == fieldInfo.farm_id)
		{
			areaIDs.insert(fieldInfo.area_id);
		}
	}

	QGeoCoordinate geo(lat, lon);
	for (auto& areaID : areaIDs)
	{
		qnzkna::ZoneManage::CZoneBase baseData;
		if (ZoneManageService()->GetZoneBase(areaID, baseData))
		{
			switch (baseData.nZoneType)
			{
			case qnzkna::ZoneManage::CZoneBase::ZoneBaseType::RECTANGLE:
			case qnzkna::ZoneManage::CZoneBase::ZoneBaseType::POLY:
			{
				QVector<QGeoCoordinate> list;
				for (auto& pt : baseData.stZonePointVec)
				{
					list.push_back({ pt.dLat, pt.dLon });
				}
				if (pointInPolygon(geo, list))
				{
					id = QString::fromLocal8Bit(baseData.strID);
				}
			}
			break;
			case qnzkna::ZoneManage::CZoneBase::ZoneBaseType::CIRCLE:
			{
				auto& pt = baseData.stRoundParameter;
				QGeoCoordinate target(pt.dLat, pt.dLon);
				double distance = target.distanceTo(geo);
				if (distance < pt.nRadius)
				{
					id = QString::fromLocal8Bit(baseData.strID);
				}
			}
			break;
			default:
				break;
			}

			if (!id.isEmpty())
				break;
		}
	}

	return id;
}

QString SaltFieldEventAnalysisWgt::GenerateName(SaltFieldEventInfo& info)
{
	QString nameRet;
	auto iter = m_eventID2DataMap.find(info.eventType);
	if (iter != m_eventID2DataMap.end())
	{
		QString tempStr = iter->eventTemplate;
		nameRet = QString(tempStr).arg(info.sourceFieldID);
	}
	return nameRet;
}

void SaltFieldEventAnalysisWgt::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void SaltFieldEventAnalysisWgt::showEvent(QShowEvent *event)
{
    m_show = true;
}

void SaltFieldEventAnalysisWgt::hideEvent(QHideEvent *event)
{
    m_show = false;

	// 清除绘制
	m_soiltestingDrawMap.clear();

	ui->field_tree_widget->ClearZoneHighLight();
}

void SaltFieldEventAnalysisWgt::Init()
{
	ui->dateTime_begin->setDateTime(QDateTime::currentDateTime());
	ui->dateTime_end->setDateTime(QDateTime::currentDateTime());

	InitDisplayTable();
}

void SaltFieldEventAnalysisWgt::InitDisplayTable()
{
	m_pModel = new QStandardItemModel(this);
	m_pSortModel = new SaltFieldSortFilterModel(this);
	m_pSortModel->setSourceModel(m_pModel);
	ui->tableView->verticalHeader()->setHidden(true);;
	ui->tableView->setSelectionBehavior(QTableView::SelectRows);
	ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableView->setAlternatingRowColors(true);
	ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableView->setModel(m_pSortModel);
	ui->tableView->horizontalHeader()->setStretchLastSection(true);
	ui->tableView->verticalHeader()->setVisible(false);
	ui->tableView->verticalHeader()->setDefaultSectionSize(100);
	ui->tableView->horizontalHeader()->setDefaultSectionSize(100);
	ui->dateTime_begin->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
	ui->dateTime_end->setDisplayFormat("yyyy-MM-dd hh:mm:ss");

	QStringList  headerTips = {
		tr2("序号"),
		tr2("事件名"),
		tr2("事件类型"),
		tr2("盐田"),
		tr2("发现者"),
		tr2("发现时间"),
		tr2("经度"),
		tr2("纬度"),
		tr2("图片信息"),
		tr2("描述"),
		tr2("显示"),
	};

	m_pModel->setColumnCount(headerTips.size());
	m_pModel->setHorizontalHeaderLabels(headerTips);
	m_pSortModel->setTimeFilterColumn(TIME_COLUMN);
	ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

	m_pPixMapDelegate = new CommPixMapDelegate(this);
	ui->tableView->setItemDelegateForColumn(PICTURE_COLUMN, m_pPixMapDelegate);

	m_pEventTypeDelegate = new ComboboxDelegate(this);
	ui->tableView->setItemDelegateForColumn(EVENT_TYPE_COLUMN, m_pEventTypeDelegate);
	auto eventTypeMap = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSaltFieldEventTypeIDMap();
	m_eventID2DataMap.clear();
	m_eventCode2IDMap.clear();
	QList<QPair<QString, QVariant>> optionList;
	for (auto& data : eventTypeMap)
	{
		m_eventID2DataMap.insert(data.id, data);
		m_eventCode2IDMap.insert(data.code, data.id);
		optionList.push_back({data.name, data.id});
	}
	m_pEventTypeDelegate->setCbbInfo(optionList);

	// 连接单击信号，监听用户的单击操作
	connect(ui->tableView, &QTableView::clicked, this, &SaltFieldEventAnalysisWgt::onCellClicked);

	// 连接双击信号，监听用户的双击操作
	connect(ui->tableView, &QTableView::doubleClicked, this, &SaltFieldEventAnalysisWgt::onCellDoubleClicked);

	connect(m_pModel, &QStandardItemModel::itemChanged, this, &SaltFieldEventAnalysisWgt::onItemChanged);

	connect(ui->searchBtn, &QPushButton::clicked, this, &SaltFieldEventAnalysisWgt::onSearchClicked);
	connect(ui->ptn_delete, &QPushButton::clicked, this, &SaltFieldEventAnalysisWgt::onDeleteClicked);
}

bool SaltFieldEventAnalysisWgt::pointInPolygon(const QGeoCoordinate& point, const QVector<QGeoCoordinate>& polygon)
{
	int crossings = 0;
	for (int i = 0; i < polygon.size(); ++i) {
		int j = (i + 1) % polygon.size();
		if (((polygon[i].latitude() <= point.latitude() &&
			point.latitude() < polygon[j].latitude()) ||
			(polygon[j].latitude() <= point.latitude() &&
				point.latitude() < polygon[i].latitude())) &&
				(point.longitude() < (polygon[j].longitude() - polygon[i].longitude()) *
			(point.latitude() - polygon[i].latitude()) /
					(polygon[j].latitude() - polygon[i].latitude()) + polygon[i].longitude())) {
			crossings++;
		}
	}
	return (crossings % 2) == 1;
}

void SaltFieldEventAnalysisWgt::UpdateTableData()
{
	m_id2DataMap.clear();
	m_pModel->setRowCount(0);
	m_pDetaiWidget->hide();
	if (m_pFieldInfo)
	{ 
		disconnect(m_pModel, &QStandardItemModel::itemChanged, this, &SaltFieldEventAnalysisWgt::onItemChanged);
		auto dataList = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSaltFieldEventDatasetList(QStringList({ m_pFieldInfo->area_id }));
		for (auto& data : dataList)
		{
			AppendOneRowTotable(data);
		}
		connect(m_pModel, &QStandardItemModel::itemChanged, this, &SaltFieldEventAnalysisWgt::onItemChanged);
	}
}

void SaltFieldEventAnalysisWgt::AppendOneRowTotable(SaltFieldEventInfoPtr soilTestingDataset)
{
	QStandardItem* pItem = nullptr;
	QList<QStandardItem*> itemList;
	
	int rowCount = m_pModel->rowCount() + 1;
	// 序号
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(QString::number(rowCount));
	pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	// 事件名
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(soilTestingDataset->name);
	pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	// 事件类型
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(m_eventID2DataMap[soilTestingDataset->eventType].name);
	pItem->setData(soilTestingDataset->eventType, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	// 盐田
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(soilTestingDataset->sourceFieldID);
	pItem->setData(soilTestingDataset->sourceFieldID, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	// 发现者
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(GetWeaponName(soilTestingDataset->reporterID));
	pItem->setData(soilTestingDataset->reporterID, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	// 发现时间
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(soilTestingDataset->findTime.toString("yyyy-MM-dd hh:mm:ss.zzz"));
	pItem->setData(soilTestingDataset->findTime, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	// 经度
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(QString::number(soilTestingDataset->lon, 'f', 6));
	pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	// 纬度
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(QString::number(soilTestingDataset->lat, 'f', 6));
	pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	// 图片信息
	pItem = new QStandardItem();
	QPixmap image;
	image.loadFromData(soilTestingDataset->picData, "JPEG");
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setData(image, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	//	tr2("描述"),
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setText(soilTestingDataset->desc);
	itemList.append(pItem);

	// tr2("显示"),
	pItem = new QStandardItem();
	pItem->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	pItem->setTextAlignment(Qt::AlignCenter);
	pItem->setCheckable(true);
	SaltFieldEventInfoPtr ptr;
	if (m_soiltestingDrawMap.query(QString::number(soilTestingDataset->id).toStdString() + "_SaltFieldEventAnalysisWgt", ptr))
		pItem->setCheckState(Qt::Checked);
	else
		pItem->setCheckState(Qt::Unchecked);

	pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);
	itemList.append(pItem);

	m_id2DataMap.insert(soilTestingDataset->id, itemList.first());
	m_pModel->appendRow(itemList);
}

void SaltFieldEventAnalysisWgt::UpdateOneRowTotable(SaltFieldEventInfoPtr soilTestingDataset)
{
	auto iter = m_id2DataMap.find(soilTestingDataset->id);
	if (iter != m_id2DataMap.end())
	{
		auto Index = m_pModel->indexFromItem(iter.value());
		int rowIndex = Index.row();
		int column = 0;

		auto pItem = m_pModel->item(rowIndex, column++);
		pItem->setText(QString::number(rowIndex + 1));
		pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);

		// 事件名
		pItem = m_pModel->item(rowIndex, column++);
		pItem->setTextAlignment(Qt::AlignCenter);
		pItem->setText(soilTestingDataset->name);
		pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);

		// 事件类型
		pItem = m_pModel->item(rowIndex, column++);
		pItem->setText(m_eventID2DataMap[soilTestingDataset->eventType].name);
		pItem->setData(soilTestingDataset->eventType, DELEGATE_DATA_ROLE);

		// 盐田
		pItem = m_pModel->item(rowIndex, column++);
		pItem->setText(soilTestingDataset->sourceFieldID);
		pItem->setData(soilTestingDataset->sourceFieldID, DELEGATE_DATA_ROLE);

		// 发现者
		pItem = m_pModel->item(rowIndex, column++);
		pItem->setData(soilTestingDataset->reporterID, DELEGATE_DATA_ROLE);

		// 发现时间
		pItem = m_pModel->item(rowIndex, column++);
		pItem->setText(soilTestingDataset->findTime.toString("yyyy-MM-dd hh:mm:ss.zzz"));
		pItem->setData(soilTestingDataset->findTime, DELEGATE_DATA_ROLE);

		// 经度
		pItem = m_pModel->item(rowIndex, column++);
		pItem->setText(QString::number(soilTestingDataset->lon, 'f', 6));
		pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);

		// 纬度
		pItem = m_pModel->item(rowIndex, column++);
		pItem->setText(QString::number(soilTestingDataset->lat, 'f', 6));
		pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);

		// 图片信息
		QPixmap image;
		image.loadFromData(soilTestingDataset->picData, "JPEG");
		pItem = m_pModel->item(rowIndex, column++);
		pItem->setTextAlignment(Qt::AlignCenter);
		pItem->setData(image, DELEGATE_DATA_ROLE);

		//	tr2("描述"),
		pItem = m_pModel->item(rowIndex, column++);
		pItem->setText(soilTestingDataset->desc);

		// tr2("显示"),
		pItem = new QStandardItem();
		pItem->setData(soilTestingDataset->id, DELEGATE_DATA_ROLE);
	}
}

void SaltFieldEventAnalysisWgt::Update3DIconDraw(const SaltFieldEventInfoPtr& eventInfo, bool isDraw)
{
	if (isDraw)
	{
			IEarth *pEarth = IEarth::GetEarth();
			if (pEarth == nullptr) return;

			QCString m_strDataDir = (m_sIconPath + "pest_warning.png").toStdString().c_str();
			QString strLable = "";
			double terrainHeight = 0.0;

			osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
			if (mapNode)
			{
				if (const auto& terrain = mapNode->getTerrain())
				{
					terrain->getHeight(mapNode->getMapSRS(), eventInfo->lon, eventInfo->lat, &terrainHeight);
				}
			}

			// 通过terrainHeight设置自动贴地
			if (const auto& graphics = pEarth->GetGraphics())
			{
				int pixel = 30;
				double mapDis = 0.0;
				if (auto proj = LY_AgriSoilAnalysisPredict::Get().GetProj())
				{
					mapDis = proj->Screen2MapDistance(1/*(pixel/2)*/); // TODO：确定实际pixel转距离值
				}
				;
				ILabel *pLabel = graphics->CreateLabel(strLable.toLocal8Bit(), m_strDataDir, eventInfo->lon, eventInfo->lat, terrainHeight + mapDis, pixel, pixel, 0);
				if (pLabel != nullptr) {
					m_3DPestCollectIconVec.emplace_back(pLabel);
				}
			}
	}
	else
	{
		IEarth *pEarth = IEarth::GetEarth();
		if (pEarth == nullptr)
			return;
		auto itr = m_3DPestCollectIconVec.begin();
		for (; itr != m_3DPestCollectIconVec.end(); itr++)
		{
			IEarth *pEarth = IEarth::GetEarth();
			pEarth->GetGraphics()->RemoveObject(*itr);
			*itr = nullptr;
			delete *itr;
		}
		m_3DPestCollectIconVec.clear();
	}
}

void SaltFieldEventAnalysisWgt::UpdateTableIndex()
{
	int rowCount = m_pModel->rowCount();
	for (int i = 0; i < rowCount; i++)
	{
		auto pItem = m_pModel->item(i, 0);
		pItem->setText(QString::number(i+1));
	}
}

QString SaltFieldEventAnalysisWgt::GetWeaponName(const QString& reporterID)
{
	QString name;
	std::string WeaponComponentName;
	bool isGetWeaponComponentName = IAgriSoilAnalysisPredictActivator::Get().getPIWeaponManageService()->
		GetWeaponComponentName(WeaponComponentName, reporterID.toStdString());
	if (isGetWeaponComponentName)
	{
		name = QString::fromStdString(WeaponComponentName);
	}
	else
	{
		name = reporterID;
	}
	return name;
}

void SaltFieldEventAnalysisWgt::onCellClicked(const QModelIndex &index)
{
	auto sourceIndex = m_pSortModel->mapToSource(index);
	auto pItem = m_pModel->item(sourceIndex.row(), 0);
	if (nullptr != pItem)
	{
		int id = pItem->data(DELEGATE_DATA_ROLE).toInt();
		auto data = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSaltFieldEventDataset(id);
		if (data->id > 0)
		{
			m_pDetaiWidget->setData(m_pFieldInfo->name, data);
			m_pDetaiWidget->show();

			auto pShowColumnItem= m_pModel->item(sourceIndex.row(), SHOW_FLAG_COLUMN);
			if (nullptr != pShowColumnItem)
			{
				pShowColumnItem->setCheckState(pShowColumnItem->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
			}
		}
	}
}

void SaltFieldEventAnalysisWgt::onCellDoubleClicked(const QModelIndex &index)
{
	auto sourceIndex = m_pSortModel->mapToSource(index);
	auto pLonItem = m_pModel->item(sourceIndex.row(), LON_COLUMN);
	auto pLatItem = m_pModel->item(sourceIndex.row(), LAT_COLUMN);
	if (nullptr !=pLonItem && nullptr != pLatItem)
	{
		double dLon = pLonItem->text().toDouble();
		double dLat = pLatItem->text().toDouble();
		if (dLon >= -180.0 && dLon <= 180.0 && dLat >= -90.0 && dLat <= 90.0)
		{
			LY_AgriSoilAnalysisPredictInterface::Get().Script("SetMapScale(4000)");
			QString str = "CenterAt(" + QString::number(dLon, 'f', 8)
				+ "," + QString::number(dLat, 'f', 8) + ")";
			LY_AgriSoilAnalysisPredictInterface::Get().Script(str.toLocal8Bit());
		}
	}
}

void SaltFieldEventAnalysisWgt::onItemChanged(QStandardItem *item)
{
	auto Index = m_pModel->indexFromItem(item);
	int rowIndex = Index.row();
	int column = Index.column();

	if (column == m_pModel->columnCount() - 1)
	{
		bool isChecked = item->checkState() == Qt::Checked;
	
		int  id = item->data(DELEGATE_DATA_ROLE).toInt();
		auto saltEventInfo = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSaltFieldEventDataset(id);

		if (isChecked)
		{
			m_soiltestingDrawMap.insert(QString::number(id).toStdString() + "_SaltFieldEventAnalysisWgt", saltEventInfo);
		}
		else
		{
			m_soiltestingDrawMap.remove(QString::number(id).toStdString() + "_SaltFieldEventAnalysisWgt");

		}

		// 更新3D绘制
		Update3DIconDraw(saltEventInfo, isChecked);
	}
}

void SaltFieldEventAnalysisWgt::onSearchClicked()
{
	QDateTime startTime = ui->dateTime_begin->dateTime();
	QDateTime endTime = ui->dateTime_end->dateTime();
	if (startTime > endTime)
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("开始事件大于结束事件！"));
		return;
	}

	QString filterString = ui->lineEdit->text();
	m_pSortModel->setFilterString(filterString, startTime, endTime);
}

void SaltFieldEventAnalysisWgt::onDeleteClicked()
{
	auto index = ui->tableView->currentIndex();
	if (!index.isValid())
		return;

	auto sourceIndex = m_pSortModel->mapToSource(index);
	auto pItem = m_pModel->item(sourceIndex.row(), 0);
	if (nullptr != pItem)
	{
		int id = pItem->data(DELEGATE_DATA_ROLE).toInt();
		if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除事件？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			bool isSuccess = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->DeleteSaltFieldEventDataset(id);
			if (!isSuccess)
			{
				LyMessageBox::warning(this, "错误", "数据库删除失败！");
			}
			else
			{
				m_pDetaiWidget->hide();
				m_pModel->removeRow(sourceIndex.row()); // 删除该行
				m_soiltestingDrawMap.remove(QString::number(id).toStdString() + "_SaltFieldEventAnalysisWgt");
				UpdateTableIndex();
			}
		}
	}
}


void SaltFieldEventAnalysisWgt::on_Menu_Anakysis()
{

}
