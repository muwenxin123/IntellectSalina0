#include <QDebug>
#include <QPointer>

#include "LyMessageBox.h"
#include "AgriSoilTestingDistributeWidget.h"
#include "ui_AgriSoilTestingDistributeWidget.h"
#include "AgriSoilTestingDataSetDialog.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "LY_AgriSoilAnalysisPredict.h"
#include "IAgriSoilAnalysisPredictActivator.h"
 

QStringList AgriSoilTestingDistributeWidget::N_List = { "< 150.00", "150.00 - 200.00", "200.00 - 250.00","250.00 - 300.00", "> 300.00" };
QStringList AgriSoilTestingDistributeWidget::P_List = { "< 15.00", "16.00 - 25.00", "25.00 - 40.00","40.00 - 45.00", "> 45.00" };
QStringList AgriSoilTestingDistributeWidget::K_List = { "< 60.00", "60.00 - 130.00", "130.00 - 200.00","200.00 - 230.00", "> 230.00" };

QStringList AgriSoilTestingDistributeWidget::N_Headers = { tr2("颜色"), tr2("N 值范围 (mg/kg)"), tr2("面积百分比 (%)") };
QStringList AgriSoilTestingDistributeWidget::P_Headers = { tr2("颜色"), tr2("P 值范围 (mg/kg)"), tr2("面积百分比 (%)") };
QStringList AgriSoilTestingDistributeWidget::K_Headers = { tr2("颜色"), tr2("K 值范围 (mg/kg)"), tr2("面积百分比 (%)") };

AgriSoilTestingDistributeWidget::AgriSoilTestingDistributeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriSoilTestingDistributeWidget),
    m_show(false)
{
    ui->setupUi(this); 

	Init();
}

AgriSoilTestingDistributeWidget::~AgriSoilTestingDistributeWidget()
{
    delete ui;
}

QPair<bool, QString> AgriSoilTestingDistributeWidget::getSoilFertilityDistribute(const AgriSoilFertilityDistributeMapPtr inDistributeMap, AgriSoilTestingFertilityPointPtrVec & outSoilFertilityPointVec, std::vector<QPointF> boundPoings /*= std::vector<QPointF>()*/, QString propName /*= ""*/)
{
	QString errorInfo = "error: ";
	if (!inDistributeMap) {
		errorInfo += tr2("分布图参数无效!");
		return QPair<bool, QString>(false, errorInfo);
	}

	// 获取区域编辑
	if (boundPoings.empty()){
		GetZoneBoundPoints(inDistributeMap->dataset_id, boundPoings);
	}
	if (boundPoings.size() < 3){
		errorInfo += tr2("区域边界无效!");
		return QPair<bool, QString>(false, errorInfo);
	}

	std::vector<KrigingInterpolator::Point> _samples;
	std::vector<double> _lons, _lats, _originValues, _fertilityValues;

	// 获取数据集内采样点列表
	auto soilTestingCollectionList = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionList(inDistributeMap->dataset_id);
	if (soilTestingCollectionList.empty()) {
		errorInfo += tr2("数据集不存在样本点，生成失败！");
		return QPair<bool, QString>(false, errorInfo);
	}

	// 根据数据有效标识进行样本生成
	if (propName.isEmpty()){
		propName = GetMatchPropName(inDistributeMap->valid_identify);
	}

	for (auto& point : soilTestingCollectionList) {
		double matchValue = GetMatchValue(point, propName);
		_lons.emplace_back(point->lon);
		_lats.emplace_back(point->lat);
		_originValues.emplace_back(matchValue);
		_samples.emplace_back(KrigingInterpolator::Point(point->lon, point->lat, matchValue));
	}

	switch (static_cast<SampleType>(inDistributeMap->sample_type))
	{
	case SampleType::CELL_SAMPLING:
	{
		// 计算土壤肥力
		_fertilityValues.clear();
		for (auto& value : _originValues) {
			_fertilityValues.emplace_back(CalcMatchLayer(propName, value));
		}
	}
	break;
	case SampleType::POINT_SAMPLING:
	{
		int neighbors = 0;
		int pointCount = _originValues.size();
		if (pointCount < 100) {
			neighbors = pointCount;
		}
		else if ((pointCount >= 100 && pointCount < 500)) {
			neighbors = 64;
		}
		else if ((pointCount >= 500 && pointCount < 1000)) {
			neighbors = 32;
		}
		else if ((pointCount >= 1000 && pointCount < 2000)) {
			neighbors = 16;
		}
		else if ((pointCount >= 2000)) {
			neighbors = 8;
		}

		// 设置默认插值栅格大小为2
		inDistributeMap->grid_unit = KRIGING_INTERPOLATOR_GRID_SPACE;

		KrigingInterpolator interpolator(inDistributeMap->grid_unit, neighbors);
		interpolator.setSamples(_samples);

		double gridAngle = osg::DegreesToRadians(10.0);
		interpolator.generateGrid(boundPoings, _lons, _lats, _originValues, gridAngle);

		// 根据插值计算土壤肥力
		_fertilityValues.clear();
		for (auto& value : _originValues) {
			_fertilityValues.emplace_back(CalcMatchLayer(propName, value));
		}
	}
	break;
	case SampleType::AREA_SAMPLING:
	default:
		break;
	}

	for (size_t i = 0; i < _fertilityValues.size(); i++)
	{
		AgriSoilTestingFertilityPointPtr soliFer = std::make_shared<AgriSoilTestingFertilityPoint>();

		if (i < _lons.size() && i < _lats.size() && i < _originValues.size())
		{
			soliFer->lon = _lons[i];
			soliFer->lat = _lats[i];
			soliFer->origin = _originValues[i];
			soliFer->fertility = _fertilityValues[i];

			outSoilFertilityPointVec.emplace_back(soliFer);
		}
	}

	return QPair<bool, QString>(true, "success");
}


void AgriSoilTestingDistributeWidget::draw2DGrid(QPainter &painter, IProj *pProj)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (pProj && m_heatMap3D)
	{
		if (m_bMap3DShow)
		{
			m_heatMap3D->create2DGridDrawing(painter, pProj);
		}
	}
}

void AgriSoilTestingDistributeWidget::setCurrentDataset(const AgriSoilTestingDatasetPtr& soilTestingDataset)
{
	if (!soilTestingDataset) return;

	// 判断是否切换
	if (m_soilTestingDataset && (soilTestingDataset->guid == m_soilTestingDataset->guid)) return;

	cleanUpMap3D();

	// 网格及坐标显示关闭
	ui->checkBox_coordinate->setChecked(false);
	ui->checkBox_grid->setChecked(false);

	m_soilTestingDataset = soilTestingDataset;

	UpdateDistributeBasicInfo();

	UpdateDistributeMapInfo();

	UpdateDisplayCombox();
}

void AgriSoilTestingDistributeWidget::cleanUp()
{
	// 先做置空处理
	m_soilTestingDataset = nullptr;

	cleanUpMap3D();

	UpdateDistributeBasicInfo();

	UpdateDistributeMapInfo();
}

void AgriSoilTestingDistributeWidget::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void AgriSoilTestingDistributeWidget::showEvent(QShowEvent *event)
{
    m_show = true;
}

void AgriSoilTestingDistributeWidget::hideEvent(QHideEvent *event)
{
    m_show = false;

	// 清理Map3D
	cleanUpMap3D();
}

void AgriSoilTestingDistributeWidget::Init()
{
	// 禁用编辑
	ui->dataset_name->setReadOnly(true);
	ui->field_area->setReadOnly(true);
	ui->collect_count->setReadOnly(true);

	// 更新下拉选择菜单
	InitDisplayCombox();

	// 初始化栅格显示图例
	InitGridDisplayLegend();

	// connect(ui->btn_generate, &QPushButton::clicked, this, &AgriSoilTestingDistributeWidget::on_btn_generate_clicked);
	// connect(ui->btn_show_control, &QPushButton::clicked, this, &AgriSoilTestingDistributeWidget::on_btn_show_control_clicked);
}

void AgriSoilTestingDistributeWidget::InitDisplayCombox()
{
	for (int i = 1; i <= 5; ++i)
	{
		DistributeMapType disMap = static_cast<DistributeMapType>(i);
		ui->combo_dismap_type->addItem(AgriSoilAnalysisPredictUtil::DistributeMapTypeToString(disMap), QVariant::fromValue(disMap));
	}

	// 下拉菜单点击
	connect(ui->combo_dismap_type, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_dismap_type_currentTextChanged(int)));
}

void AgriSoilTestingDistributeWidget::InitGridDisplayLegend()
{
	// 一级
	QList<QPair<QString, std::string>> legendData1 = {
		{ "#FF9800", "一级" }
	};

	// 二级
	QList<QPair<QString, std::string>> legendData2 = {
		{ "#4CAF50", "一级" },{ "#FF9800", "二级" }
	};

	// 五级
	QList<QPair<QString, std::string>> legendData3 = {
		{ "#4CAF50", "一级" },{ "#CDDC39", "二级" },{ "#FF9800", "三级" },{ "#E53935", "四级" },{ "#8F1D1D", "五级" }
	};

	// 十级
	QList<QPair<QString, std::string>> legendData4 = {
		{ "#3EA044", "一级" },{ "#4CAF50", "二级" },{ "#8BC34A", "三级" },{ "#CDDC39", "四级" },{ "#FFC107", "五级" },
		{ "#FF9800", "六级" },{ "#FF5722", "七级" },{ "#E53935", "八级" },{ "#C62828", "九级" },{ "#8F1D1D", "十级" }
	};

	m_mDiffusionLayerHexColor.insert(std::make_pair(DiffusionLayer::DIFFUSION_LAYER_1, legendData1));
	m_mDiffusionLayerHexColor.insert(std::make_pair(DiffusionLayer::DIFFUSION_LAYER_2, legendData2));
	m_mDiffusionLayerHexColor.insert(std::make_pair(DiffusionLayer::DIFFUSION_LAYER_3, legendData3));
	m_mDiffusionLayerHexColor.insert(std::make_pair(DiffusionLayer::DIFFUSION_LAYER_4, legendData4));

	// 十六进制的 RGB 颜色值osgEarth::Symbology::Color 对象进行存储
	for (const auto& pair : m_mDiffusionLayerHexColor)
	{
		std::vector<osgEarth::Symbology::Color> osgColor;
		for (const auto& legend : pair.second)
		{
			osgEarth::Symbology::Color c = HeatMap3DUtil::hexToColor(legend.first.toStdString());
			osgColor.emplace_back(c);
		}
		m_mDiffusionLayerOsgColor.insert(std::make_pair(pair.first, osgColor));
	}

	// 创建图例表格
	ui->tableWidget->setColumnCount(3);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setVisible(false);

	// 禁用表格的编辑功能
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置 QTableWidget 按行选择
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	QStringList headers = { tr2("颜色"), tr2("N 值范围 (mg/kg)"), tr2("面积百分比 (%)") };
	ui->tableWidget->setHorizontalHeaderLabels(headers);

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 第0列固定
	ui->tableWidget->setColumnWidth(0, 60);
	//ui->tableWidget->setColumnWidth(1, 120);
	//ui->tableWidget->setColumnWidth(2, 115);
}

void AgriSoilTestingDistributeWidget::UpdateDisplayCombox()
{
	if (!m_distributeMap) return;

	int vaildIdent = m_distributeMap->valid_identify;
	int ele_index = -1, combo_dismap_type_index = 0, combo_dismap_sidType_index = 0;

	for (int i = 0; i < ui->combo_dismap_type->count(); ++i) {
		DistributeMapType mapType = ui->combo_dismap_type->itemData(i).value<DistributeMapType>();
		if (m_distributeMap->map_type == static_cast<int>(mapType)) {
			combo_dismap_type_index = i;
		}
	}

	for (int j = 0; j < ui->combo_dismap_sidType->count(); ++j) {
		DisMapPropDisplayVaild disData = ui->combo_dismap_sidType->itemData(j).value<DisMapPropDisplayVaild>();
		if (disData.vaild_index == vaildIdent) {
			combo_dismap_sidType_index = j;
		}
	}

	ui->combo_dismap_type->setCurrentIndex(combo_dismap_type_index);
	on_comboBox_dismap_type_currentTextChanged(combo_dismap_type_index);
	ui->combo_dismap_sidType->setCurrentIndex(combo_dismap_sidType_index);
}

void AgriSoilTestingDistributeWidget::UpdateDistributeBasicInfo()
{
	// 清理界面
	ui->dataset_name->clear();
	ui->field_area->clear();
	ui->collect_count->clear();
	ui->ele_nutrient->clear();
	ui->ele_minor->clear();
	ui->ele_heavy_metal->clear();
	ui->ele_other->clear();

	ui->combo_dismap_sidType->clear();
	m_mDistributeMapBitIndex.clear();

	if (!m_soilTestingDataset) return;

	ui->dataset_name->setText(m_soilTestingDataset->name);

	// 计算区域面积
	double area = 0.0;
	std::vector<QPointF> zonePoints;
	qnzkna::ZoneManage::CZoneBase zoneBase = LY_AgriSoilAnalysisPredict::Get().GetActiveFieldZone();
	for (const auto& stZonePoint : zoneBase.stZonePointVec)
	{
		QPointF tmpPointF(stZonePoint.dLon, stZonePoint.dLat);
		zonePoints.emplace_back(tmpPointF);
	}
	AgriSoilAnalysisPredictUtil::CalcPolygonArea(zonePoints,area);
	ui->field_area->setText(QString::number(AgriSoilAnalysisPredictUtil::sqmToMu(area)));

	// 查询数据集采集点数量
	int tmpCount = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionCount(m_soilTestingDataset->guid);
	ui->collect_count->setText(QString::number(tmpCount));

	QString ele_nutrient = "", ele_minor = "", ele_heavy_metal = "", ele_other = "";

	DisMapPropDisplayVaildPtrVec ele_nutrient_list, ele_minor_list, ele_heavy_metal_list, ele_other_list, soil_fertility_list;

	int ele_index = -1;
	rttr::type objType = rttr::type::get<AgriSoilTestingCollection>();
	for (auto& prop : objType.get_properties())
	{
		// 元数注释获取
		rttr::variant type = prop.get_metadata("type");
		if (type.is_valid())
		{
			++ele_index; // !!!有效索引

			if (!((m_soilTestingDataset->valid_identify >> ele_index) & 1)) {
				continue; // 未选中项
			}

			auto inserData = [&](DisMapPropDisplayVaildPtrVec & list) {
				rttr::variant display = prop.get_metadata("display");
				if (display.is_valid())
				{
					QString displayName = QString::fromLocal8Bit(display.to_string().c_str());
					ele_nutrient.append(displayName + ",");
					QString propName = QString::fromStdString(prop.get_name().to_string());
					int vaild_index = 1 << ele_index;

					DisMapPropDisplayVaildPtr data = std::make_shared<DisMapPropDisplayVaild>();
					data->prop_name = propName;
					data->display_name = displayName;
					data->vaild_index = vaild_index;

					list.emplace_back(data);
				}
			};

			if (QString::fromLocal8Bit(type.to_string().c_str()) == "ele_nutrient") {
				inserData(ele_nutrient_list);
			}
			else if (QString::fromLocal8Bit(type.to_string().c_str()) == "ele_minor") {
				inserData(ele_minor_list);
			}
			else if (QString::fromLocal8Bit(type.to_string().c_str()) == "ele_heavy_metal") {
				inserData(ele_heavy_metal_list);
			}
			else if (QString::fromLocal8Bit(type.to_string().c_str()) == "mois") {
				inserData(ele_other_list);
			}
		}
	}

	/* 暂不使用该方式 << 使用rttr类型注册方式获取匹配值 >>

	QStringList eleVec;
	eleVec << tr2("氮") << tr2("磷") << tr2("钾");
	eleVec << tr2("钙") << tr2("镁") << tr2("铜") << tr2("锌") << tr2("铁") << tr2("硫") << tr2("锰") << tr2("钼") << tr2("硼") << tr2("氯") << tr2("硅");
	eleVec << tr2("铅") << tr2("铬") << tr2("镉") << tr2("汞");
	eleVec << tr2("有机质");
	eleVec << tr2("温度") << tr2("湿度") << tr2("PH") << tr2("EC") << tr2("水势");

	for (int i = 0; i < eleVec.size(); ++i) {
		if ((m_soilTestingDataset->valid_identify >> i) & 1) {
			if (i < 3) {
				ele_nutrient.append(eleVec[i] + ",");
				ele_nutrient_list.append({ eleVec[i] , i });
			}
			else if (i >= 3 && i < 14) {
				ele_minor.append(eleVec[i] + ",");
				ele_minor_list.append({ eleVec[i] , i });
			}
			else if (i >= 14 && i < 18) {
				ele_heavy_metal.append(eleVec[i] + ",");
				ele_heavy_metal_list.append({ eleVec[i] , i });
			}
			else if (i >= 18 && i < 24) {
				ele_other.append(eleVec[i] + ",");
				ele_other_list.append({ eleVec[i] , i });
			}
		}
	}
	*/

	soil_fertility_list.emplace_back(std::make_shared<DisMapPropDisplayVaild>(DisMapPropDisplayVaild(tr2("soil_fertility"), tr2("肥力分布图"), 0))); // 单独设置

	m_mDistributeMapBitIndex.insert(std::make_pair(DistributeMapType::ELE_NUTRIENT, ele_nutrient_list));
	m_mDistributeMapBitIndex.insert(std::make_pair(DistributeMapType::ELE_MINOR, ele_minor_list));
	m_mDistributeMapBitIndex.insert(std::make_pair(DistributeMapType::ELE_HEAVY_METAL, ele_heavy_metal_list));
	m_mDistributeMapBitIndex.insert(std::make_pair(DistributeMapType::MOIS, ele_other_list));
	m_mDistributeMapBitIndex.insert(std::make_pair(DistributeMapType::SOIL_FERTILITY, soil_fertility_list));

	auto processString = [](QString str)->QString { 
		if (!str.isEmpty()) {
			str.chop(1);
		}else {
			str = tr2("无");
		}
		return str;
	};

	// 生成土壤元素数据
	ui->ele_nutrient->setText(processString(ele_nutrient));
	ui->ele_minor->setText(processString(ele_minor));
	ui->ele_heavy_metal->setText(processString(ele_heavy_metal));
	ui->ele_other->setText(processString(ele_other));

	// 级联操作
	ui->combo_dismap_type->setCurrentIndex(0);
	on_comboBox_dismap_type_currentTextChanged(0);
}

void AgriSoilTestingDistributeWidget::UpdateDistributeMapInfo()
{
	ui->analysis_name->clear();

	// 清理栅格展示信息
	cleanUpGridDisplay();

	// 清理栅格显示图例
	cleanUpGridDisplayLegend();

	if (m_soilTestingDataset)
	{
		// 设置默认名称
		ui->analysis_name->setText(tr2("分布图_") + m_soilTestingDataset->name);

		// 查询分布图信息
		auto disMapList = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingDistributeMap(m_soilTestingDataset->guid);
		if (disMapList.size())
		{
			m_distributeMap = disMapList[disMapList.size() - 1];

			// 基本信息设置
			ui->analysis_name->setText(m_distributeMap->name);

			// 栅格信息统计
			std::vector<int> statisticnNmbers;
			for (const QString &numStr : m_distributeMap->distribute_statistic.split(",")) {
				statisticnNmbers.push_back(numStr.toInt());
			}

			// 更新栅格图例
			UpdateGridDisplayLegend(m_distributeMap);
		}
		else
		{
			m_distributeMap = nullptr;
		}
	}
	else
	{
		m_distributeMap = nullptr;
	}
}

void AgriSoilTestingDistributeWidget::UpdateGridDisplayStatistic(DiffusionLayer difLayer, const std::vector<int>& difCount)
{
	// 清理栅格统计展示
	cleanUpGridDisplay();

	QList<QPair<QString, std::string>> gridData = m_mDiffusionLayerHexColor.at(difLayer);

	if (gridData.size() != difCount.size()) return;

	for (int i = 0; i < gridData.size(); ++i)
	{
		gridData[i].second = QString::number(difCount[i]).toStdString();
	}

	// 动态生成统计图
	QWidget *legend = createGridWidget(gridData);

	QVBoxLayout *mainLayout = new QVBoxLayout(ui->groupBox_2);
	mainLayout->addWidget(legend);
	mainLayout->addStretch();
}

void AgriSoilTestingDistributeWidget::UpdateGridDisplayLegend(const AgriSoilFertilityDistributeMapPtr pDistributeMap)
{
	// 清理栅格显示图例
	cleanUpGridDisplayLegend();

	// 栅格信息统计
	std::vector<int> statisticNmbers;
	for (const QString &numStr : pDistributeMap->distribute_statistic.split(",")) {
		statisticNmbers.push_back(numStr.toInt());
	}
	DiffusionLayer diffuseLayer = AgriSoilAnalysisPredictUtil::GetDiffusionLayer(pDistributeMap->diffuse_layer);

	auto &gridData = m_mDiffusionLayerHexColor.at(diffuseLayer);
	if (gridData.size() != statisticNmbers.size()) return;

	QString propName = GetMatchPropName(pDistributeMap->valid_identify);
	if (propName.isEmpty()) return;

	// 匹配图例信息
	QStringList headers, legendList;
	GetMatchLegend(propName, headers, legendList);
	if (gridData.size() != legendList.size()) return;
	ui->tableWidget->setHorizontalHeaderLabels(headers);
	
	// 更新图例表格
	UpdateGridDisplayLegendTable(gridData, legendList, statisticNmbers);

	// 更新栅格统计展示
	UpdateGridDisplayStatistic(diffuseLayer, statisticNmbers);
}

void AgriSoilTestingDistributeWidget::UpdateGridDisplayLegendTable(const QList<QPair<QString, std::string>>& gridData, const QStringList& legendList, const std::vector<int>& statisticNmbers)
{
	int totalDifount = 0;
	for (int i = 0; i < statisticNmbers.size(); ++i)
	{
		totalDifount += statisticNmbers[i];
	}

	ui->tableWidget->setRowCount((int)legendList.size());

	for (int row = 0; row < gridData.size(); ++row)
	{
		QWidget *tmpWidget = new QWidget();
		QFrame *colorBox = new QFrame(tmpWidget);
		colorBox->setFixedSize(30, 30);
		colorBox->setFrameShape(QFrame::StyledPanel);
		colorBox->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(gridData[row].first));

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

		ui->tableWidget->setCellWidget(row, 0, container);

		QTableWidgetItem* rangeItem = new QTableWidgetItem(legendList[row]);
		rangeItem->setTextAlignment(Qt::AlignCenter);
		ui->tableWidget->setItem(row, 1, rangeItem);

		QTableWidgetItem* percentItem = new QTableWidgetItem(QString::number((totalDifount ? ((float)statisticNmbers[row] / totalDifount) : 0) * 100, 'f', 2));
		percentItem->setTextAlignment(Qt::AlignCenter);
		ui->tableWidget->setItem(row, 2, percentItem);
	}

	//ui->tableWidget->resizeColumnsToContents();
}

QWidget* AgriSoilTestingDistributeWidget::createLegendGroupBox(const QList<QPair<QString, std::string>>& legendData, QWidget *parent /*= nullptr*/)
{
	QWidget *groupBox = new QWidget(parent);
	QGridLayout *gridLayout = new QGridLayout();

	gridLayout->setSpacing(10);
	gridLayout->setContentsMargins(10, 10, 10, 10);

	for (int i = 0; i < legendData.size(); ++i) {
		// 颜色框
		QFrame *colorBox = new QFrame(groupBox);
		colorBox->setFixedSize(30, 30);
		colorBox->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(legendData[i].first));

		// 文字标签
		QLabel *label = new QLabel(tr2(legendData[i].second.c_str()), groupBox);

		// 两行五列布局
		gridLayout->addWidget(colorBox, i / 5, (i % 5) * 2);
		gridLayout->addWidget(label, i / 5, (i % 5) * 2 + 1);
	}

	groupBox->setLayout(gridLayout);
	return groupBox;
}

QWidget* AgriSoilTestingDistributeWidget::createGridWidget(const QList<QPair<QString, std::string>>& gridData, QWidget *parent /*= nullptr*/)
{
	QWidget *groupBox = new QWidget(parent);
	QGridLayout *gridLayout = new QGridLayout();

	gridLayout->setSpacing(50);
	gridLayout->setContentsMargins(35, 35, 35, 35);

	for (int i = 0; i < gridData.size(); ++i) {
		// 颜色框
		QFrame *colorBox = new QFrame(groupBox);
		colorBox->setFixedSize(30, 30);
		colorBox->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(gridData[i].first));

		// 文字标签
		QLabel *label = new QLabel(tr2(gridData[i].second.c_str()), groupBox);

		// 两列五行布局
		gridLayout->addWidget(colorBox, i / 2, (i % 2) * 5);
		gridLayout->addWidget(label, i / 2, (i % 2) * 5 + 1);
	}

	groupBox->setLayout(gridLayout);
	return groupBox;
}

void AgriSoilTestingDistributeWidget::cleanUpGridDisplay()
{
	auto clearLayout = [&](QGroupBox* groupBox) {
		QLayout* layout = groupBox->layout();
		if (!layout) return;

		// 移除并删除所有子控件
		while (QLayoutItem* item = layout->takeAt(0)) {
			if (QWidget* widget = item->widget()) {
				widget->deleteLater();
			}
			delete item;
		}

		groupBox->setLayout(nullptr);
		delete layout;
	};

	// 清理布局
	clearLayout(ui->groupBox_2);
}

void AgriSoilTestingDistributeWidget::cleanUpMap3D()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	// 清理地图数据
	if (m_heatMap3D)
	{
		m_heatMap3D->cleanUp();
		delete m_heatMap3D;
		m_heatMap3D = nullptr;
	}
}

void AgriSoilTestingDistributeWidget::cleanUpGridDisplayLegend()
{
	// 清理表格数据
	while (ui->tableWidget->rowCount() > 0) {
		ui->tableWidget->removeRow(0);
	}

	ui->ele_max->clear();
	ui->ele_min->clear();
	ui->ele_avg->clear();
}

void AgriSoilTestingDistributeWidget::GetActiveFieldBoundPoings(std::vector<QPointF>& boundPoings)
{
	// 获取当前激活区域
	auto activeZone = LY_AgriSoilAnalysisPredict::Get().GetActiveFieldZone();

	if (activeZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
		activeZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
		if (activeZone.stZonePointVec.size() > 0) {
			for (auto zone : activeZone.stZonePointVec)
			{
				QPointF pointF;
				pointF.setX(zone.dLon);
				pointF.setY(zone.dLat);
				boundPoings.push_back(pointF);
			}
		}
	}
}

void AgriSoilTestingDistributeWidget::GetZoneBoundPoints(QString datasetId, std::vector<QPointF>& boundPoings)
{
	// 查询数据集，获取区域id
	QString area_id = "";
	auto soilTestingDataset = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingDataset(datasetId);
	if (soilTestingDataset)
	{
		AgriFieldInfoVec vecFieldList;
		AgriFarmManagerService()->GetFieldInfoList(vecFieldList);
		for (auto& fieldInfo : vecFieldList)
		{
			if (soilTestingDataset->field_id == fieldInfo.id)
			{
				area_id = fieldInfo.area_id;
				break;
			}
		}
	}

	// 匹配区域边界
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = ZoneManageService()->GetAllZoneBase();
	qnzkna::ZoneManage::CZoneBase tmpCurZone;
	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			tmpCurZone = itrZone;
			break;
		}
	}

	if (tmpCurZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
		tmpCurZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
		if (tmpCurZone.stZonePointVec.size() > 0) {
			for (auto& point : tmpCurZone.stZonePointVec)
			{
				boundPoings.emplace_back(QPointF(point.dLon, point.dLat));
			}
		}
	}
}

QString AgriSoilTestingDistributeWidget::GetMatchPropName(int validIdentify)
{
	QString propName = "";

	int ele_index = -1;
	rttr::type objType = rttr::type::get<AgriSoilTestingCollection>();
	for (auto& prop : objType.get_properties())
	{
		// 元数注释获取
		rttr::variant type = prop.get_metadata("type");
		if (type.is_valid())
		{
			++ele_index; // !!!有效索引

			if (!((validIdentify >> ele_index) & 1)) {
				continue; // 未选中项
			}

			if (QString::fromLocal8Bit(type.to_string().c_str()) == "ele_nutrient"
				|| QString::fromLocal8Bit(type.to_string().c_str()) == "ele_minor"
				|| QString::fromLocal8Bit(type.to_string().c_str()) == "ele_heavy_metal"
				|| QString::fromLocal8Bit(type.to_string().c_str()) == "mois") 
			{
				propName = QString::fromStdString(prop.get_name().to_string());
				break;
			}
		}
	}

	return propName;
}

double AgriSoilTestingDistributeWidget::GetMatchValue(AgriSoilTestingCollectionPtr collect, QString elePropName)
{
	double matchVal = 0.0;
	if (!collect || elePropName.isEmpty()) return matchVal;

	AgriSoilTestingCollection originCollect = *(collect.get());
	rttr::type t = rttr::type::get(originCollect); // 获取类型
	rttr::property prop = t.get_property(elePropName.toStdString()); // 通过属性名称获取属性
	if (prop.is_valid())
	{
		rttr::variant value = prop.get_value(originCollect); // 获取属性值
		if (value.is_type<float>()) {
			matchVal = value.get_value<float>();
		}else if (value.is_type<double>()) {
			matchVal = value.get_value<double>();
		}
	}
	return matchVal;
}

double AgriSoilTestingDistributeWidget::CalcMatchLayer(QString elePropName, double elePropValue)
{
	SoilFertility fertility = SoilFertility::Undefined;
	if (elePropName == "ele_nutrient_N")
	{
		fertility = AgriSoilAnalysisPredictUtil::evaluateFertility_SingleN(static_cast<int>(elePropValue));
	}
	else if (elePropName == "ele_nutrient_P")
	{
		fertility = AgriSoilAnalysisPredictUtil::evaluateFertility_SingleP(static_cast<int>(elePropValue));
	}
	else if (elePropName == "ele_nutrient_K")
	{
		fertility = AgriSoilAnalysisPredictUtil::evaluateFertility_SingleK(static_cast<int>(elePropValue));
	}
	return static_cast<double>(fertility);
}

void AgriSoilTestingDistributeWidget::GetMatchLegend(QString elePropName, QStringList& headerList, QStringList& legendList)
{
	if (elePropName == "ele_nutrient_N")
	{
		headerList = N_Headers;
		legendList = N_List;
	}
	else if (elePropName == "ele_nutrient_P")
	{
		headerList = P_Headers;
		legendList = P_List;
	}
	else if (elePropName == "ele_nutrient_K")
	{
		headerList = K_Headers;
		legendList = K_List;
	}
}

std::vector<AgriSoilTestingDistributeWidget::LegendEntry> AgriSoilTestingDistributeWidget::GenerateLegendFromData(const std::vector<double>& vecData, int groupCount)
{
	std::vector<LegendEntry> legends;

	if (vecData.empty() || groupCount <= 0) return legends;

	double minVal = *std::min_element(vecData.begin(), vecData.end());
	double maxVal = *std::max_element(vecData.begin(), vecData.end());
	double range = maxVal - minVal;
	double interval = range / groupCount;

	for (int i = 0; i < groupCount; ++i) {
		double low = minVal + i * interval;
		double high = (i == groupCount - 1) ? maxVal : (low + interval);

		int count = std::count_if(vecData.begin(), vecData.end(), [=](double v) {
			return (i == groupCount - 1) ? (v >= low && v <= high) : (v >= low && v < high);
		});

		LegendEntry entry;
		//entry.rangeText = QString::number(low, 'f', 2) + " - " + QString::number(high, 'f', 2);
		legends.emplace_back(entry);
	}

	return legends;
}

QString AgriSoilTestingDistributeWidget::JoinStatisticByHeatMap3D(HeatMap3D *heatMap3D, const QString &sep /*= ","*/)
{
	if (!heatMap3D){
		return "";
	}

	std::vector<int> statistics;
	heatMap3D->getPixelStatistic(statistics);

	// 移除第一项数据(底图)
	if (!statistics.empty()) {
		statistics.erase(statistics.begin());
	}

	QStringList strList;
	for (int num : statistics) {
		strList.append(QString::number(num));
	}

	return strList.join(sep);
}

void AgriSoilTestingDistributeWidget::on_btn_generate_clicked()
{
	if (!m_soilTestingDataset){
		LyMessageBox::warning(this, tr2("警告"), tr2("未选中数据集！"));
		return;
	}

	cleanUpMap3D();

	// 边界信息
	std::vector<QPointF> boundPoings;
	GetActiveFieldBoundPoings(boundPoings);
	if (boundPoings.size() < 3){
		LyMessageBox::warning(this, tr2("警告"), tr2("未选中区域田块！"));
		return;
	}

	// 获取数据集内采样点列表
	auto soilTestingCollectionList = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionList(m_soilTestingDataset->guid);
	if (soilTestingCollectionList.empty()){
		LyMessageBox::warning(this, tr2("警告"), tr2("数据集不存在样本点，生成失败！"));
		return;
	}

	// 根据选择分布图类别进行样本生成
	int dismap_sidIndex = ui->combo_dismap_sidType->currentIndex();
	DisMapPropDisplayVaild disData = ui->combo_dismap_sidType->itemData(dismap_sidIndex).value<DisMapPropDisplayVaild>();
	if (disData.prop_name.isEmpty()){
		LyMessageBox::warning(this, tr2("警告"), tr2("无效的分布图类别！"));
		return;
	}

	// 初始化栅格大小
	m_dGridSpace = m_soilTestingDataset->grid_size;

	AgriSoilFertilityDistributeMapPtr inDistributeMap = std::make_shared<AgriSoilFertilityDistributeMap>();
	inDistributeMap->grid_unit = m_dGridSpace;
	inDistributeMap->dataset_id = m_soilTestingDataset->guid;
	inDistributeMap->sample_type = m_soilTestingDataset->sample_type;
	inDistributeMap->valid_identify = disData.vaild_index;
	inDistributeMap->map_type = ui->combo_dismap_type->itemData(ui->combo_dismap_type->currentIndex()).value<int>();

	AgriSoilTestingFertilityPointPtrVec outSoilFertilityPointVec;
	getSoilFertilityDistribute(inDistributeMap, outSoilFertilityPointVec, boundPoings, disData.prop_name);

	// 更新栅格大小
	m_dGridSpace = inDistributeMap->grid_unit;
	
	std::vector<double> _lons, _lats, _originValues, _fertilityValues;
	for (const auto& soilPoint : outSoilFertilityPointVec)
	{
		_lons.emplace_back(soilPoint->lon);
		_lats.emplace_back(soilPoint->lat);
		_originValues.emplace_back(soilPoint->origin);
		_fertilityValues.emplace_back(soilPoint->fertility);
	}

	m_heatMap3D = new HeatMap3D;
	// TODO：栅格大小临时调整1m 20250720
	m_heatMap3D->setPixelSpacing(2.0/*m_dGridSpace*/);
	m_heatMap3D->setEqualCount(m_nEqualCount);
	m_heatMap3D->setDisCount(m_nDisCount); // 不扩散
	m_heatMap3D->setBoundPoints(boundPoings);
	m_heatMap3D->setLonLatValue(_lons, _lats, _fertilityValues);

	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	osgViewer::Viewer* viewer = (osgViewer::Viewer*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_OsgViewer));
	m_heatMap3D->setMapNode(mapNode);
	m_heatMap3D->setViewer(viewer);

	// 更新颜色列表
	DiffusionLayer difLayer = DiffusionLayer::DIFFUSION_LAYER_3;
	auto colorLayer = m_mDiffusionLayerOsgColor.at(difLayer);
	colorLayer.insert(colorLayer.begin(), HeatMap3DUtil::hexToColor("#30913C", 0.95f)); // 插入底色
	m_heatMap3D->updateColors(colorLayer);

	// 生成栅格图
	bool cordIsChecked = ui->checkBox_coordinate->isChecked();
	bool gridIsChecked = ui->checkBox_grid->isChecked();
	double gridAngle = osg::DegreesToRadians(10.0);
	m_heatMap3D->startCreateHeatMap(cordIsChecked, gridIsChecked, gridAngle);
	m_heatMap3D->setGridLineShowOrHide(true);

	// 栅格显示统计
	if (m_heatMap3D){

		inDistributeMap->distribute_statistic = JoinStatisticByHeatMap3D(m_heatMap3D);
		inDistributeMap->diffuse_layer = 5;

		UpdateGridDisplayLegend(inDistributeMap);
	}
}

void AgriSoilTestingDistributeWidget::on_btn_save_clicked()
{
	if (!m_soilTestingDataset)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("未选中数据集！"));
		return;
	} 

	// 根据选择分布图类别进行样本生成
	int dismap_sidIndex = ui->combo_dismap_sidType->currentIndex();
	DisMapPropDisplayVaild disData = ui->combo_dismap_sidType->itemData(dismap_sidIndex).value<DisMapPropDisplayVaild>();
	if (disData.prop_name.isEmpty()) {
		LyMessageBox::warning(this, tr2("警告"), tr2("无效的分布图类别！"));
		return;
	}

	AgriSoilFertilityDistributeMapPtr pPestDistributeMap = std::make_shared<AgriSoilFertilityDistributeMap>();
	pPestDistributeMap->name = ui->analysis_name->text();
	pPestDistributeMap->dataset_id = m_soilTestingDataset->guid;
	pPestDistributeMap->field_area = ui->field_area->text().toDouble();
	pPestDistributeMap->collect_count = ui->collect_count->text().toInt();
	pPestDistributeMap->grid_unit = m_dGridSpace;
	pPestDistributeMap->diffuse_layer = 5;
	pPestDistributeMap->sample_type = m_soilTestingDataset->sample_type;
	pPestDistributeMap->sample_mode = m_soilTestingDataset->sample_mode;
	pPestDistributeMap->map_type = ui->combo_dismap_type->itemData(ui->combo_dismap_type->currentIndex()).value<int>();
	pPestDistributeMap->valid_identify = disData.vaild_index;

	if (m_distributeMap)
	{
		// 修改分布图
		if (m_heatMap3D){
			pPestDistributeMap->distribute_statistic = JoinStatisticByHeatMap3D(m_heatMap3D);
		}
		else{
			pPestDistributeMap->distribute_statistic = m_distributeMap->distribute_statistic;
			pPestDistributeMap->diffuse_layer = m_distributeMap->diffuse_layer; // 未重新生成，分布层数不做改变
		}

		pPestDistributeMap->guid = m_distributeMap->guid;

		bool isSuccess = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->UpdateSoilTestingDistributeMap(pPestDistributeMap);
		if (!isSuccess)
		{
			LyMessageBox::warning(this, tr2("错误"), tr2("存储数据库失败！"));
		}
		else
		{
			LyMessageBox::information(this, tr2("成功"), tr2("分布图更新成功！"));
		}
	}
	else
	{
		// 新增分布图
		if (!m_heatMap3D)
		{
			LyMessageBox::warning(this, tr2("警告"), tr2("未生成分布图！"));
			return;
		}

		pPestDistributeMap->guid = AgriSoilAnalysisPredictUtil::GenerateUuid();

		pPestDistributeMap->distribute_statistic = JoinStatisticByHeatMap3D(m_heatMap3D);

		bool isSuccess = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->AddSoilTestingDistributeMap(pPestDistributeMap);
		if (!isSuccess)
		{
			LyMessageBox::warning(this, tr2("错误"), tr2("存储数据库失败！"));
		}
		else
		{
			LyMessageBox::information(this, tr2("成功"), tr2("分布图保存成功！"));
		}
	}
}

void AgriSoilTestingDistributeWidget::on_btn_show_control_clicked()
{
	if (m_heatMap3D)
	{
		m_heatMap3D->setImageShowOrHide(!m_bMap3DShow);
		m_bMap3DShow = !m_bMap3DShow;
	}
}

void AgriSoilTestingDistributeWidget::on_checkBox_coordinate_stateChanged(int nStates)
{
	if (m_heatMap3D)
	{
		m_heatMap3D->setCoordinateShowOrHide(!!nStates);
	}
}

void AgriSoilTestingDistributeWidget::on_checkBox_grid_stateChanged(int nStates)
{
	if (m_heatMap3D)
	{
		m_heatMap3D->setGridLineShowOrHide(!!nStates);
	}
}

void AgriSoilTestingDistributeWidget::on_comboBox_dismap_type_currentTextChanged(int index)
{
	if (!m_soilTestingDataset) return;

	ui->combo_dismap_sidType->clear();

	DistributeMapType disMap_type = static_cast<DistributeMapType>(ui->combo_dismap_type->itemData(index).value<int>());

	if (m_mDistributeMapBitIndex.count(disMap_type) > 0) {

		const auto &disMapList = m_mDistributeMapBitIndex.at(disMap_type);
		for (int i = 0; i < disMapList.size(); ++i)
		{
			if (const auto &disData = disMapList[i])
			{
				ui->combo_dismap_sidType->addItem(disMapList[i]->display_name, QVariant::fromValue(*(disData.get())));
			}
		}

		ui->combo_dismap_sidType->setCurrentIndex(0);
	}
}

