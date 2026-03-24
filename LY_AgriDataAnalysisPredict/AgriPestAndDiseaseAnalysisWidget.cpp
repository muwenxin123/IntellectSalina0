#include <QApplication>
#include <QHeaderView>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QSettings>
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>

#include "LyMessageBox.h"
#include "AgriPestAndDiseaseAnalysisWidget.h"
#include "ui_AgriPestAndDiseaseAnalysisWidget.h"
#include "LY_AgriDataAnalysisPredict.h"
#include "AgriAPSimMet.h"
#include "AgriPestAndDiseaseDataSetDialog.h"
#include "AgriDataAnalysisPredictUtil.h"
#include "AgriDataAnalysisPredictAlgorithm.h"
#include "LY_AgriDataAnalysisPredict.h"


AgriPestAndDiseaseAnalysisWidget::AgriPestAndDiseaseAnalysisWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriPestAndDiseaseAnalysisWidget),
    m_show(false)
{
    ui->setupUi(this);

	//土地图标
	m_sIconPath = (":/images/mointor/");

	Init();

	// 树叶子节点点击
	//connect(ui->field_tree_widget, &AgriFieldTree::LeafNodeClicked, [this](QTreeWidgetItem *item, int column) {

	//	if (item && item->parent() == nullptr)
	//	{
	//		AgriFieldInfo fieldInfo = item->data(0, Qt::UserRole + 1).value<AgriFieldInfo>();
	//		if (fieldInfo.id.length())
	//		{
	//			m_pFieldInfo = std::make_shared<AgriFieldInfo>(fieldInfo);

	//			// 更新表格展示数据
	//			UpdateTableData();

	//			// 清理数据集脏数据
	//			ui->distribute_widget->cleanUp();
	//		}
	//	}
	//});

	connect(m_pAgrisprayComboBox, &AgrisprayComboBox::sigCurrentTextChange, [&](const QString& text_t) {
		if (m_pAgrisprayComboBox->currentIndex() == 0) {
			ui->tableWidget->clearContents();
			ui->tableWidget->setRowCount(0);
			return;
		}
		AgriFieldInfo fieldInfo = m_pAgrisprayComboBox->currentData().value<AgriFieldInfo>();
		if (fieldInfo.id.length())
		{
			m_pFieldInfo = std::make_shared<AgriFieldInfo>(fieldInfo);

			// 更新表格展示数据
			UpdateTableData();

			// 清理数据集脏数据
			ui->distribute_widget->cleanUp();
		}
	});
}

AgriPestAndDiseaseAnalysisWidget::~AgriPestAndDiseaseAnalysisWidget()
{
    delete ui;
}

AgriPestAndDiseaseDataCollectDialog* AgriPestAndDiseaseAnalysisWidget::getPestAndDiseaseDataCollectDialog()
{
	if (m_pestAndDiseaseDataSetDialog)
	{
		return m_pestAndDiseaseDataSetDialog->getPestAndDiseaseDataCollectDialog();
	}

	return nullptr;
}

AgriPestAndDiseaseDistributeWidget* AgriPestAndDiseaseAnalysisWidget::getPestAndDiseaseDataCollectWidget()
{
	return ui->distribute_widget;
}

AgrisprayComboBox* AgriPestAndDiseaseAnalysisWidget::getAgrisprayComboBox()
{
	return m_pAgrisprayComboBox;
}

qnzkna::ZoneManage::CZoneBase AgriPestAndDiseaseAnalysisWidget::GetActiveFieldZone()
{
	//if (ui->field_tree_widget)
	//{
	//	return ui->field_tree_widget->GetActiveFieldZone();
	//}
	if (m_pAgrisprayComboBox) {
		return m_pAgrisprayComboBox->GetActiveFieldZone();
	}
	return qnzkna::ZoneManage::CZoneBase();
}

void AgriPestAndDiseaseAnalysisWidget::UpdataCurActiveFarm()
{
	//if (ui->field_tree_widget)
	//{
	//	ui->field_tree_widget->SwitchActiveFarm();
	//}
	if (m_pAgrisprayComboBox) {
		m_pAgrisprayComboBox->switchActiveFarm();
	}
}

void AgriPestAndDiseaseAnalysisWidget::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (!m_show)
	{
		return;
	}

	// 采集点绘制
	int pointSize = 12;
	QFont font("Arial", pointSize, QFont::Bold);
	painter.setFont(font);

	// 设置颜色（带透明度（0-255，128 代表 50% 透明度））
	QColor textColor(205, 220, 0);
	textColor.setAlpha(200);
	QPen pen;
	pen.setWidth(1);
	pen.setColor(textColor);
	painter.setPen(pen);

	// 获取文本尺寸
	QFontMetrics fm(font);
	int textHeight = fm.ascent();

	m_pestDrawMap.iterate_query([&](const std::string& id, const AgriPestCollectionPtrVec& collectionVec) {

		for (auto & collect : collectionVec)
		{
			//int radius = collect->range;

			QPoint point = pProj->Jwd2Screen(collect->lon, collect->lat);

			// 使用矩形来表示圆形，绘制时保证宽高相等
			// painter.drawEllipse(point.x() - radius, point.y() - radius, radius * 2, radius * 2);

			QPixmap pixmap;
			if (pixmap.load(m_sIconPath + "pest_warning.png"))
			{
				painter.drawPixmap(point.x() - 15, point.y() - 15, 30, 30, pixmap);
			}

			painter.drawText(QPoint(point.x() - textHeight * 1.5, point.y() + textHeight * 2.0), tr2(collect->name.toStdString().c_str()));
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/* 绘制路径

		std::vector<AgriDataAnalysisPredictAlgorithm::Poi> points;
		std::vector<QPointF> qPoints;
		std::vector<double> lons;
		std::vector<double> lats;

		// 添加起点
		//points.emplace_back(AgriDataAnalysisPredictAlgorithm::Poi(116.1946670, 40.2647784));

		for (auto & collect : collectionVec)
		{
			points.emplace_back(AgriDataAnalysisPredictAlgorithm::Poi(collect->lon, collect->lat));
			qPoints.emplace_back(QPointF(collect->lon, collect->lat));
			lons.emplace_back(collect->lon);
			lats.emplace_back(collect->lat);
		}

		std::vector<AgriDataAnalysisPredictAlgorithm::Poi> path;

		// 添加终点
		//points.emplace_back(AgriDataAnalysisPredictAlgorithm::Poi(116.1946670,40.2647784));
		//AgriDataAnalysisPredictAlgorithm::zoneMinPath(points, path);

		auto activeZone = GetActiveFieldZone();
		std::vector<QPointF> boundPoings;
		if (activeZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
			activeZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
			if (activeZone.stZonePointVec.size() > 0) {
				for (auto zone : activeZone.stZonePointVec){
					QPointF pointF;
					pointF.setX(zone.dLon);
					pointF.setY(zone.dLat);
					boundPoings.push_back(pointF);
				}
			}
		}

		double spacing = ui->distribute_widget->getGridSpace();
		std::vector<QPoint> pointPath;
		std::map<QPoint, AgriDataAnalysisPredictAlgorithm::Poi, AgriDataAnalysisPredictAlgorithm::QPointFCompare> pointMap;
		if (AgriDataAnalysisPredictAlgorithm::createGridGeometryMap(boundPoings, lons, lats, spacing, pointPath, pointMap))
		{
			// std::vector<QPoint> keys;
			// keys.reserve(pointMap.size());  // 预分配空间，提高性能
			// std::transform(pointMap.begin(), pointMap.end(), std::back_inserter(keys),
			// [](const auto& pair) { return pair.first; });
			

			// 计算路径
			std::vector<QPoint> qPath;
			AgriDataAnalysisPredictAlgorithm::generatePath(pointPath, qPath);
			for (const auto& qPoint : qPath) {
				//auto it = pointMap.find(qPoint); // 不存在找不到现象，此处不做判断
				//if (it != pointMap.end()) {
					path.emplace_back(pointMap[qPoint]);
				//}
			}

			// 校准路径
			double maxDisSpace = sqrt(pow(1.5*spacing, 2) + pow(0.5*spacing, 2));
			for (int i = 0; i< path.size() - 1; i++){
				auto &poi1 = path[i];
				auto &poi2 = path[i+1];

				bool poi1isFind = false;
				bool poi2isFind = false;

				auto it1 = std::find_if(points.begin(), points.end(), [&](const AgriDataAnalysisPredictAlgorithm::Poi& poi) { return (poi.lon == poi1.lon) && poi.lat == poi1.lat; });
				if (it1 != points.end()) {
					poi1isFind = true;
				}
				auto it2 = std::find_if(points.begin(), points.end(), [&](const AgriDataAnalysisPredictAlgorithm::Poi& poi) { return (poi.lon == poi2.lon) && poi.lat == poi2.lat; });
				if (it2 != points.end()) {
					poi2isFind = true;
				}

				if (std::fabs(poi1.lon - poi2.lon) < maxDisSpace){
					if (poi1isFind && !poi2isFind){
						poi2.lon = poi1.lon;
					}else if (poi2isFind && !poi1isFind){
						poi1.lon = poi2.lon;
					}
				}else if (std::fabs(poi1.lat - poi2.lat) < maxDisSpace){
					if (poi1isFind && !poi2isFind) {
						poi2.lat = poi1.lat;
					}
					else if (poi2isFind && !poi1isFind) {
						poi1.lat = poi2.lat;
					}
				}
			}
		}

		if (path.size() >= 2)
		{
			// 创建路径
			QPainterPath painterPath;
			QPoint beginPoint = pProj->Jwd2Screen(path[0].lon, path[0].lat);
			painterPath.moveTo(beginPoint.x(), beginPoint.y());  // 起点
			for (int i = 1; i < path.size(); i++)
			{
				QPoint point = pProj->Jwd2Screen(path[i].lon, path[i].lat);
				painterPath.lineTo(point.x(), point.y()); // 直线
			}
			// 关闭路径形成封闭图形
			//painterPath.closeSubpath();

			// 绘制路径
			QPen pen(Qt::white);
			pen.setWidth(2);
			painter.setPen(pen);
			painter.drawPath(painterPath);
		}

		*/
		////////////////////////////////////////////////////////////////////////////////////////////////////
	});

	// 更新区域高亮
	//ui->field_tree_widget->ZoneHighLight(painter, pProj);
	m_pAgrisprayComboBox->zoneHighLight(painter, pProj);

	// 更新分布图(2D)
	ui->distribute_widget->draw2DGrid(painter, pProj);
}

IUserMesh2 *CreateRectMesh(IGraphics *pGrp, int x, int y, int z)
{
	int i, j, k, n;
	n = (x + y)*(z + 1) + ((x - 1)*(y - 1));
	n <<= 1;
	std::vector<VERTEX> v;
	v.reserve(n);

	for (k = 0; k <= z; k++)
	{
		for (i = 0; i <= x; i++)
		{
			v.push_back({ (float)i, (float)0, (float)k });
			v.push_back({ (float)i, (float)y, (float)k });
		}
		for (j = 1; j < y; j++)
		{
			v.push_back({ (float)0, (float)j, (float)k });
			v.push_back({ (float)x, (float)j, (float)k });
		}
	}
	for (i = 1; i < x; i++)
	{
		for (j = 1; j < y; j++)
		{
			v.push_back({ (float)i, (float)j, (float)z });
			v.push_back({ (float)i, (float)j, (float)0 });
		}
	}

	IUserMesh2 *pUM = pGrp->CreateUserMesh2();
	pUM->SetPoints(v.size(), v.data());

	j = (x + y) << 1;
	k = x << 1;
	n = j*z;
	std::vector<unsigned short> vs;
	vs.reserve(v.size() + (z + 1) * 4 + j * 2);
	for (i = 0; i < v.size(); i++)
	{
		vs.push_back(i);
	}
	for (i = 0; i <= z; i++)
	{
		vs.push_back(i*j + 0);
		vs.push_back(i*j + k);
		vs.push_back(i*j + 1);
		vs.push_back(i*j + k + 1);
	}
	for (i = 0; i < j; i++)
	{
		vs.push_back(i);
		vs.push_back(i + n);
	}
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_LINES, false);

	vs.clear();
	vs.reserve(8);
	vs.push_back(0);
	vs.push_back(1);
	vs.push_back(k + 1);
	vs.push_back(k);
	vs.push_back(n + 0);
	vs.push_back(n + k);
	vs.push_back(n + k + 1);
	vs.push_back(n + 1);
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_QUADS, false);

	vs.clear();
	vs.reserve(10);
	vs.push_back(n);
	vs.push_back(0);
	vs.push_back(k + n);
	vs.push_back(k);
	vs.push_back(k + n + 1);
	vs.push_back(k + 1);
	vs.push_back(n + 1);
	vs.push_back(1);
	vs.push_back(n);
	vs.push_back(0);
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_QUAD_STRIP, true);

	//pUM->SetState(IUserMesh::LIGHT, 1);
	return pUM;
}

IUserMesh2 *SetCubeColor(IGraphics *pGrp, int x, int y, int z, unsigned int color)
{
	std::vector<VERTEX> v;
	v.reserve(8);

	v.push_back({ (float)x, (float)y, (float)z });
	v.push_back({ (float)(x + 1), (float)y, (float)z });
	v.push_back({ (float)(x + 1), (float)(y + 1), (float)z });
	v.push_back({ (float)x, (float)(y + 1), (float)z });
	v.push_back({ (float)x, (float)y, (float)(z + 1) });
	v.push_back({ (float)(x + 1), (float)y, (float)(z + 1) });
	v.push_back({ (float)(x + 1), (float)(y + 1), (float)(z + 1) });
	v.push_back({ (float)x, (float)(y + 1), (float)(z + 1) });


	IUserMesh2 *pUM = pGrp->CreateUserMesh2();
	pUM->SetPoints(v.size(), v.data());

	std::vector<unsigned short> vs;
	vs.clear();
	vs.reserve(8);
	vs.push_back(3);
	vs.push_back(2);
	vs.push_back(1);
	vs.push_back(0);
	vs.push_back(4);
	vs.push_back(5);
	vs.push_back(6);
	vs.push_back(7);
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_QUADS, false);

	vs.clear();
	vs.reserve(10);
	vs.push_back(4);
	vs.push_back(0);
	vs.push_back(5);
	vs.push_back(1);
	vs.push_back(6);
	vs.push_back(2);
	vs.push_back(7);
	vs.push_back(3);
	vs.push_back(4);
	vs.push_back(0);
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_QUAD_STRIP, true);
	pUM->SetColor(color);

	return pUM;
}

void AgriPestAndDiseaseAnalysisWidget::On2d3dChanged(int wParam)
{
	/*
	typedef std::vector<IUserMesh*> VUM;
	typedef std::map<std::string, VUM> MapSVUM;
	static MapSVUM smSVUM;
	CCreateShape cs;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth->IsVisible())
	{
		IGraphics *pGrp = pEarth->GetGraphics();
		{
			IUserMesh2 *pUM = CreateRectMesh(pGrp, 100, 100, 1);
			pUM->SetPosition(116.195, 40.2647, 100);
			pUM->SetScale(1);
			for (int j = 0; j < 100; j++)
			{
				//pUM = SetCubeColor(pGrp, rand() % 100, rand() % 100, rand() % 1, RGBA(rand() & 0xFF, rand() & 0xFF, rand() & 0xFF, 128));
				//pUM->SetPosition(116.195, 40.2647, 100);
				//pUM->SetScale(1);
			}
		}
	}
	*/
}

void AgriPestAndDiseaseAnalysisWidget::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void AgriPestAndDiseaseAnalysisWidget::showEvent(QShowEvent *event)
{
    m_show = true;
}

void AgriPestAndDiseaseAnalysisWidget::hideEvent(QHideEvent *event)
{
    m_show = false;

	// 清除绘制
	m_pestDrawMap.clear();

	//ui->field_tree_widget->ClearZoneHighLight();
	m_pAgrisprayComboBox->clearZoneHighLight();
}

void AgriPestAndDiseaseAnalysisWidget::Init()
{
	ui->dateTime_begin->setDateTime(QDateTime::currentDateTime());
	ui->dateTime_end->setDateTime(QDateTime::currentDateTime());

	InitDisplayTable();

	InitDisplayCombox();

	ui->field_tree_widget->hide();
	m_pAgrisprayComboBox = new AgrisprayComboBox(this);
	auto vLayout = (QVBoxLayout*)(ui->main_widget->layout());
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(new QLabel(tr2("当前盐田：")), 0);
	hlayout->addWidget(m_pAgrisprayComboBox, 1);
	vLayout->insertLayout(0, hlayout);
}

void AgriPestAndDiseaseAnalysisWidget::InitDisplayTable()
{
	ui->tableWidget->setColumnCount(10);
	ui->tableWidget->setHorizontalHeaderLabels({
		tr2("序号"),
		tr2("显示"),
		tr2("盐田"),
		tr2("数据集名称"),
		tr2("作物类型"),
		tr2("采集方式"),
		tr2("开始时间"),
		tr2("结束时间"),
		tr2("病虫害"),
		tr2("害虫类型")
	});
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setVisible(false);

	// 禁用表格的编辑功能
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置 QTableWidget 按行选择
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	// 连接单击信号，监听用户的单击操作
	connect(ui->tableWidget, &QTableWidget::cellClicked, this, &AgriPestAndDiseaseAnalysisWidget::onCellClicked);

	// 连接双击信号，监听用户的双击操作
	connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &AgriPestAndDiseaseAnalysisWidget::onCellDoubleClicked);

}

void AgriPestAndDiseaseAnalysisWidget::InitDisplayCombox()
{
	for (int i = 0; i <= 4; ++i)
	{
		CropType crop_type = static_cast<CropType>(i);
		ui->comboBox_crop_type->addItem(AgriDataAnalysisPredictUtil::CropTypeToString(crop_type), QVariant::fromValue(crop_type));
	}

	for (int i = 0; i <= 5; ++i)
	{
		DataCollectType acq_type = static_cast<DataCollectType>(i);
		ui->comboBox_acq_mode->addItem(AgriDataAnalysisPredictUtil::DataCollectTypeToString(acq_type), QVariant::fromValue(acq_type));
	}

	for (int i = 0; i <= 4; ++i)
	{
		PestDisease pestDisease = static_cast<PestDisease>(i);
		ui->comboBox_pest->addItem(AgriDataAnalysisPredictUtil::PestDiseaseToString(pestDisease), QVariant::fromValue(pestDisease));
	}

	for (int i = 0; i <= 4; ++i)
	{
		PestCategory category = static_cast<PestCategory>(i);
		ui->comboBox_pest_type->addItem(AgriDataAnalysisPredictUtil::PestCategoryToString(category), QVariant::fromValue(category));
	}
}


void AgriPestAndDiseaseAnalysisWidget::CreateDataSetDialog(DataSetHandleMode handleMode)
{
	if (!m_pFieldInfo)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("未选中到地块信息!"));
		return;
	}

	if (m_pestAndDiseaseDataSetDialog)
	{
		delete m_pestAndDiseaseDataSetDialog;
	}

	AgriPestDataset dataset;
	int row = ui->tableWidget->currentRow();
	if (row >= 0) 
	{
		if (QTableWidgetItem *item = ui->tableWidget->item(row, 0))
		{
			dataset = item->data(Qt::UserRole).value<AgriPestDataset>();
		}
	}

	switch (handleMode)
	{
	case DataSetHandleMode::DATA_SET_HANDLE_VIEW:
	{
		m_pestAndDiseaseDataSetDialog = new AgriPestAndDiseaseDataSetDialog(handleMode, m_pFieldInfo, dataset, this);
	}
	break;
	case DataSetHandleMode::DATA_SET_HANDLE_ADD:
	{
		m_pestAndDiseaseDataSetDialog = new AgriPestAndDiseaseDataSetDialog(handleMode, m_pFieldInfo, dataset, this);
		connect(m_pestAndDiseaseDataSetDialog, &AgriPestAndDiseaseDataSetDialog::addFinish, this, &AgriPestAndDiseaseAnalysisWidget::HandleDatasetAddFinish);
	}
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_UPDATE:
	{
		m_pestAndDiseaseDataSetDialog = new AgriPestAndDiseaseDataSetDialog(handleMode, m_pFieldInfo, dataset, this);
		connect(m_pestAndDiseaseDataSetDialog, &AgriPestAndDiseaseDataSetDialog::updateFinish, this, &AgriPestAndDiseaseAnalysisWidget::HandleDatasetUpdateFinish);
	}
		break;
	case DataSetHandleMode::DATA_SET_HANDLE_DELETE:
		break;
	default:
		break;
	}

	if (m_pestAndDiseaseDataSetDialog)m_pestAndDiseaseDataSetDialog->show();

	// 采集点更新
	connect(m_pestAndDiseaseDataSetDialog, &AgriPestAndDiseaseDataSetDialog::updateCollection, [this](DataCollectHandle handle, AgriPestCollectionPtr pestCollection) {

		switch (handle)
		{
		case DataCollectHandle::DATA_COLLECT_ADD:
		{
			m_pestDrawMap.iterate_update([&](const std::string& id, AgriPestCollectionPtrVec& collectionVec) {

				if (id == pestCollection->dataset_id.toStdString())
				{
					// 添加
					collectionVec.emplace_back(pestCollection);
				}
			});
		}
			break;
		case DataCollectHandle::DATA_COLLECT_UPDATE:
		{
			m_pestDrawMap.iterate_update([&](const std::string& id, AgriPestCollectionPtrVec& collectionVec) {

				if (id == pestCollection->dataset_id.toStdString())
				{
					// 更新(先删除再添加)
					collectionVec.erase(std::remove_if(collectionVec.begin(), collectionVec.end(),
						[pestCollection](const auto& collect) {
						return collect->guid == pestCollection->guid;
					}),
						collectionVec.end());
					collectionVec.emplace_back(pestCollection);
				}
			});
		}
			break;
		case DataCollectHandle::DATA_COLLECT_DELETE:
		{
			m_pestDrawMap.iterate_update([&](const std::string& id, AgriPestCollectionPtrVec& collectionVec) {

				if (id == pestCollection->dataset_id.toStdString())
				{
					// 使用 remove_if 删除
					collectionVec.erase(std::remove_if(collectionVec.begin(), collectionVec.end(),
						[pestCollection](const auto& collect) {
						return collect->guid == pestCollection->guid;
					}),
						collectionVec.end());
				}
			});
		}
			break;
		default:
			break;
		}
	});
}

void AgriPestAndDiseaseAnalysisWidget::HandleDatasetAddFinish(AgriPestDatasetPtr pestDataset)
{
	m_pestAndDiseaseDataSetDialog->close();

	if (pestDataset)
	{
		AddOneRowTotable(pestDataset);
	}
}

void AgriPestAndDiseaseAnalysisWidget::HandleDatasetUpdateFinish(AgriPestDatasetPtr pestDataset)
{
	m_pestAndDiseaseDataSetDialog->close();

	if (pestDataset)
	{
		UpdateOneRowTotable(pestDataset);
	}
}

void AgriPestAndDiseaseAnalysisWidget::UpdateTableData()
{
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);

	if (m_pFieldInfo)
	{
		// 查询数据集列表
		auto pestDatasetList = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->GetPestDatasetList(QStringList({ m_pFieldInfo->id }));

		//ui->tableWidget->setRowCount(pestDatasetList.size());

		for (int row = 0; row < pestDatasetList.size(); ++row)
		{
			AddOneRowTotable(pestDatasetList[row]);
		}
	}
}


void AgriPestAndDiseaseAnalysisWidget::AddOneRowTotable(AgriPestDatasetPtr pestDataset)
{
	int rowCount = ui->tableWidget->rowCount();
	ui->tableWidget->insertRow(rowCount);

	QCheckBox* checkBox = new QCheckBox();
	QWidget* radioWidget = new QWidget();
	QHBoxLayout* checkBoxLayout = new QHBoxLayout(radioWidget);
	checkBoxLayout->addWidget(checkBox);
	checkBoxLayout->setAlignment(Qt::AlignCenter);
	checkBoxLayout->setContentsMargins(0, 0, 0, 0);

	// 连接复选框的选中信号
	connect(checkBox, &QCheckBox::toggled, this, [this, rowCount, checkBox](bool checked) {
		onCheckBoxToggled(rowCount, checked);
	});

	auto userDefItem = new QTableWidgetItem(QString::number(rowCount + 1));
	userDefItem->setTextAlignment(Qt::AlignCenter);
	userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(pestDataset.get())));
	ui->tableWidget->setItem(rowCount, 0, userDefItem);

	ui->tableWidget->setCellWidget(rowCount, 1, radioWidget);
	ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(m_pFieldInfo->name));
	ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(pestDataset->name));
	ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(AgriDataAnalysisPredictUtil::CropTypeToString(static_cast<CropType>(pestDataset->crop_type))));
	ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(AgriDataAnalysisPredictUtil::DataCollectTypeToString(static_cast<DataCollectType>(pestDataset->collect_type))));
	ui->tableWidget->setItem(rowCount, 6, new QTableWidgetItem(pestDataset->begin_time));
	ui->tableWidget->setItem(rowCount, 7, new QTableWidgetItem(pestDataset->end_time));
	ui->tableWidget->setItem(rowCount, 8, new QTableWidgetItem(AgriDataAnalysisPredictUtil::PestDiseaseToString(static_cast<PestDisease>(pestDataset->pest_disease))));
	ui->tableWidget->setItem(rowCount, 9, new QTableWidgetItem(AgriDataAnalysisPredictUtil::PestCategoryToString(static_cast<PestCategory>(pestDataset->pest_category))));
}


void AgriPestAndDiseaseAnalysisWidget::UpdateOneRowTotable(AgriPestDatasetPtr pestDataset)
{
	int rowCount = ui->tableWidget->rowCount();
	for (int row = 0; row < rowCount; ++row) {
		if (QTableWidgetItem *item = ui->tableWidget->item(row, 0)) {
			AgriPestDataset dataset = item->data(Qt::UserRole).value<AgriPestDataset>();
			if (dataset.guid == pestDataset->guid) {

				auto userDefItem = new QTableWidgetItem(QString::number(row + 1));
				userDefItem->setTextAlignment(Qt::AlignCenter);
				userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(pestDataset.get())));
				ui->tableWidget->setItem(row, 0, userDefItem);
				ui->tableWidget->setItem(row, 2, new QTableWidgetItem(m_pFieldInfo->name));
				ui->tableWidget->setItem(row, 3, new QTableWidgetItem(pestDataset->name));
				ui->tableWidget->setItem(row, 4, new QTableWidgetItem(AgriDataAnalysisPredictUtil::CropTypeToString(static_cast<CropType>(pestDataset->crop_type))));
				ui->tableWidget->setItem(row, 5, new QTableWidgetItem(AgriDataAnalysisPredictUtil::DataCollectTypeToString(static_cast<DataCollectType>(pestDataset->collect_type))));
				ui->tableWidget->setItem(row, 6, new QTableWidgetItem(pestDataset->begin_time));
				ui->tableWidget->setItem(row, 7, new QTableWidgetItem(pestDataset->end_time));
				ui->tableWidget->setItem(row, 8, new QTableWidgetItem(AgriDataAnalysisPredictUtil::PestDiseaseToString(static_cast<PestDisease>(pestDataset->pest_disease))));
				ui->tableWidget->setItem(row, 9, new QTableWidgetItem(AgriDataAnalysisPredictUtil::PestCategoryToString(static_cast<PestCategory>(pestDataset->pest_category))));
			}
		}
	}
}

void AgriPestAndDiseaseAnalysisWidget::Update3DIconDraw(const AgriPestCollectionPtrVec& pestCollectVec, bool isDraw)
{
	if (isDraw)
	{
		for (auto & collect : pestCollectVec)
		{
			IEarth *pEarth = IEarth::GetEarth();
			if (pEarth == nullptr) return;
			QCString m_strDataDir = (m_sIconPath + "pest_warning.png").toStdString().c_str();

			/* 预防图标添加崩溃使用。暂未发现崩溃问题(2025.03.20)
				const QByteArray &ba = QUuid::createUuid().toRfc4122();
				QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
				QString strPathOld = QObject::tr(m_strDataDir) + "pest_warning.png";
				QFile currenFile(strPathOld);
				QString strPath = QObject::tr(m_strDataDir);
				strPath += QLatin1String("temp/");
				strPath += uid.toString(QUuid::WithoutBraces);
				strPath += QLatin1String(".png");
				currenFile.copy(strPath);
				std::string strImg = tr3(strPath).data();
			*/

			QString strLable = "";
			double terrainHeight = 0.0;

			osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
			if (mapNode)
			{
				if (const auto& terrain = mapNode->getTerrain())
				{
					terrain->getHeight(mapNode->getMapSRS(), collect->lon, collect->lat, &terrainHeight);
				}
			}

			// 通过terrainHeight设置自动贴地
			if (const auto& graphics = pEarth->GetGraphics())
			{
				int pixel = 30;
				double mapDis = 0.0;
				if (auto proj = LY_AgriDataAnalysisPredict::Get().GetProj())
				{
					mapDis = proj->Screen2MapDistance(1/*(pixel/2)*/); // TODO：确定实际pixel转距离值
				}
				;
				ILabel *pLabel = graphics->CreateLabel(strLable.toLocal8Bit(), m_strDataDir, collect->lon, collect->lat, terrainHeight + mapDis, pixel, pixel, 0);
				if (pLabel != nullptr) {
					m_3DPestCollectIconVec.emplace_back(pLabel);
				}
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

void AgriPestAndDiseaseAnalysisWidget::on_ptn_view_dataset_clicked()
{
	if (-1 == m_nSelectedIndex)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行"));
		return;
	}
	
	CreateDataSetDialog(DataSetHandleMode::DATA_SET_HANDLE_VIEW);
}

void AgriPestAndDiseaseAnalysisWidget::on_ptn_add_dataset_clicked()
{
	CreateDataSetDialog(DataSetHandleMode::DATA_SET_HANDLE_ADD);
}

void AgriPestAndDiseaseAnalysisWidget::on_ptn_delete_dataset_clicked()
{
	int row = ui->tableWidget->currentRow();
	if (row >= 0) {

		QTableWidgetItem *item = ui->tableWidget->item(row, 0);
		if (item)
		{
			AgriPestDataset dataset = item->data(Qt::UserRole).value<AgriPestDataset>();

			if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除数据集[%1]？").arg(dataset.name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			{
				bool isSuccess = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->DeletePestDataset(dataset.guid);
				if (!isSuccess)
				{
					LyMessageBox::warning(this, "错误", "数据库删除失败！");
				}
				else
				{
					ui->tableWidget->removeRow(row); // 删除该行

					// 移除所有采集点展示
					m_pestDrawMap.remove(dataset.guid.toStdString());
				}
			}
		}
	}
	else {
		LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行进行删除"));
	}
}

void AgriPestAndDiseaseAnalysisWidget::on_ptn_update_dataset_clicked()
{
	if (-1 == m_nSelectedIndex)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行"));
		return;
	}

	CreateDataSetDialog(DataSetHandleMode::DATA_SET_HANDLE_UPDATE);
}

void AgriPestAndDiseaseAnalysisWidget::on_ptn_export_dataset_clicked()
{
	// 导出
}

void AgriPestAndDiseaseAnalysisWidget::onCellClicked(int row, int column)
{
	QTableWidgetItem *item = ui->tableWidget->item(row, 0);
	if (item) {
		m_nSelectedIndex = row;

		AgriPestDataset dataset = item->data(Qt::UserRole).value<AgriPestDataset>();

		// 更新病虫害分布图界面
		ui->distribute_widget->setCurrentDataset(std::make_shared<AgriPestDataset>(dataset));
	}
}

void AgriPestAndDiseaseAnalysisWidget::onCellDoubleClicked(int row, int column)
{
	// 获取双击单元格的数据
	QTableWidgetItem *item = ui->tableWidget->item(row, column);
	if (item) {

		m_nSelectedIndex = row;

		CreateDataSetDialog(DataSetHandleMode::DATA_SET_HANDLE_VIEW);
	}
}

void AgriPestAndDiseaseAnalysisWidget::onCheckBoxToggled(int row, bool checked)
{
	QTableWidgetItem *item = ui->tableWidget->item(row, 0);
	if (item) {
		AgriPestDataset dataset = item->data(Qt::UserRole).value<AgriPestDataset>();
		// 查询采集点列表
		auto pestCollectionList = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->GetPestCollectionList(dataset.guid);

		if (checked)
		{
			m_pestDrawMap.insert(dataset.guid.toStdString(), pestCollectionList);
		}
		else
		{
			m_pestDrawMap.remove(dataset.guid.toStdString());
		}

		// 更新3D绘制
		Update3DIconDraw(pestCollectionList, checked);
	}
}
