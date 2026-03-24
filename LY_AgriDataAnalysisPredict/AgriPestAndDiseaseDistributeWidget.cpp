#include <QPointer>

#include "LyMessageBox.h"
#include "AgriPestAndDiseaseDistributeWidget.h"
#include "ui_AgriPestAndDiseaseDistributeWidget.h"
#include "AgriAPSimMet.h"
#include "AgriPestAndDiseaseDataSetDialog.h"
#include "AgriDataAnalysisPredictUtil.h"
#include "LY_AgriDataAnalysisPredict.h"
#include "IAgriDataAnalysisPredictActivator.h"
#include "AgriPestAndDiseaseDataGenerater.h"


AgriPestAndDiseaseDistributeWidget::AgriPestAndDiseaseDistributeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriPestAndDiseaseDistributeWidget),
    m_show(false)
{
    ui->setupUi(this);

	Init();
}

AgriPestAndDiseaseDistributeWidget::~AgriPestAndDiseaseDistributeWidget()
{
    delete ui;
}

void AgriPestAndDiseaseDistributeWidget::draw2DGrid(QPainter &painter, IProj *pProj)
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

void AgriPestAndDiseaseDistributeWidget::setCurrentDataset(const AgriPestDatasetPtr& pestDataset)
{
	if (!pestDataset) return;

	// 判断是否切换
	if (m_pestDataset && (pestDataset->guid == m_pestDataset->guid)) return;

	cleanUpMap3D();

	// 网格及坐标显示关闭
	ui->checkBox_coordinate->setChecked(false);
	ui->checkBox_grid->setChecked(false);

	m_pestDataset = pestDataset;

	UpdataDistributeBasicInfo();

	UpdataDistributeMapInfo();
}

void AgriPestAndDiseaseDistributeWidget::cleanUp()
{
	// 先做置空处理
	m_pestDataset = nullptr;

	cleanUpMap3D();

	UpdataDistributeBasicInfo();

	UpdataDistributeMapInfo();
}

void AgriPestAndDiseaseDistributeWidget::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void AgriPestAndDiseaseDistributeWidget::showEvent(QShowEvent *event)
{
    m_show = true;
}

void AgriPestAndDiseaseDistributeWidget::hideEvent(QHideEvent *event)
{
    m_show = false;

	// 清理Map3D
	cleanUpMap3D();
}

void AgriPestAndDiseaseDistributeWidget::Init()
{
	m_stackedLayout = new QStackedLayout();

	// 禁用编辑
	ui->dataset_name->setReadOnly(true);
	ui->field_area->setReadOnly(true);
	ui->collect_count->setReadOnly(true);

	ui->combo_crop_type->setDisabled(true);

	// 更新下拉选择菜单
	InitDisplayCombox();

	// 初始化栅格显示图例
	InitGridDisplayLegend();

	// 初始化动态蔓延生成参数
	ui->dync_temperature->setText("24");
	ui->dync_humidity->setText("30");
	ui->dync_wind_speed->setText("5");
	ui->dync_spread_time->setText("60");
	ui->dync_record_gap->setText("10");

	// connect(ui->btn_generate, &QPushButton::clicked, this, &AgriPestAndDiseaseDistributeWidget::on_btn_generate_clicked);
	// connect(ui->btn_show_control, &QPushButton::clicked, this, &AgriPestAndDiseaseDistributeWidget::on_btn_show_control_clicked);
}

void AgriPestAndDiseaseDistributeWidget::InitDisplayCombox()
{
	for (int i = 1; i < 4; ++i)
	{
		CropType crop_type = static_cast<CropType>(i);
		ui->combo_crop_type->addItem(AgriDataAnalysisPredictUtil::CropTypeToString(crop_type), QVariant::fromValue(crop_type));
	}

	for (int i = 1; i <= 5; ++i)
	{
		GridSize size = static_cast<GridSize>(i);
		ui->combo_grid->addItem(AgriDataAnalysisPredictUtil::GridSizeToString(size), QVariant::fromValue(size));
	}

	for (int i = 1; i <= 4; ++i)
	{
		DiffusionLayer layer = static_cast<DiffusionLayer>(i);
		int layerVal = 1;
		switch (layer)
		{
		case DiffusionLayer::DIFFUSION_LAYER_1:layerVal = 1; break;
		case DiffusionLayer::DIFFUSION_LAYER_2:layerVal = 2; break;
		case DiffusionLayer::DIFFUSION_LAYER_3:layerVal = 5; break;
		case DiffusionLayer::DIFFUSION_LAYER_4:layerVal = 10; break;
		default:break;
		}
		ui->combo_diffuse->addItem(AgriDataAnalysisPredictUtil::DiffusionLayerToString(layer), QVariant::fromValue(layerVal));
	}

	connect(ui->combo_diffuse, QOverload<int>::of(&QComboBox::currentIndexChanged), m_stackedLayout, &QStackedLayout::setCurrentIndex);

	for (int i = 1; i < 3; ++i)
	{
		GenerateType gene_type = static_cast<GenerateType>(i);
		ui->combo_generate_type->addItem(AgriDataAnalysisPredictUtil::GeneTypeToString(gene_type), QVariant::fromValue(gene_type));
	}

	connect(ui->combo_generate_type, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_gene_type_currentTextChanged(int)));

	// 触发默认选中第一项
	on_comboBox_gene_type_currentTextChanged(0);

	for (int i = 0; i < 8; ++i)
	{
		WindDirection wind_direct= static_cast<WindDirection>(i);
		ui->combo_wind_direct->addItem(AgriDataAnalysisPredictUtil::WindDirectToString(wind_direct), QVariant::fromValue(wind_direct));
	}
}

void AgriPestAndDiseaseDistributeWidget::InitGridDisplayLegend()
{
	// 一级扩散
	QList<QPair<QString, std::string>> legendData1 = {
		{ "#FF9800", "一级" }
	};

	// 二级扩散
	QList<QPair<QString, std::string>> legendData2 = {
		{ "#4CAF50", "一级" },{ "#FF9800", "二级" }
	};

	// 五级扩散
	QList<QPair<QString, std::string>> legendData3 = {
		{ "#4CAF50", "一级" },{ "#CDDC39", "二级" },{ "#FF9800", "三级" },{ "#E53935", "四级" },{ "#8F1D1D", "五级" }
	};

	// 十级扩散
	QList<QPair<QString, std::string>> legendData4 = {
		{ "#3EA044", "一级" },{ "#4CAF50", "二级" },{ "#8BC34A", "三级" },{ "#CDDC39", "四级" },{ "#FFC107", "五级" },
		{ "#FF9800", "六级" },{ "#FF5722", "七级" },{ "#E53935", "八级" },{ "#C62828", "九级" },{ "#8F1D1D", "十级" }
	};

	m_mDiffusionLayerHexColor.insert(std::make_pair(DiffusionLayer::DIFFUSION_LAYER_1, legendData1));
	m_mDiffusionLayerHexColor.insert(std::make_pair(DiffusionLayer::DIFFUSION_LAYER_2, legendData2));
	m_mDiffusionLayerHexColor.insert(std::make_pair(DiffusionLayer::DIFFUSION_LAYER_3, legendData3));
	m_mDiffusionLayerHexColor.insert(std::make_pair(DiffusionLayer::DIFFUSION_LAYER_4, legendData4));

	QWidget *legend1 = createLegendGroupBox(legendData1);
	QWidget *legend2 = createLegendGroupBox(legendData2);
	QWidget *legend3 = createLegendGroupBox(legendData3);
	QWidget *legend4 = createLegendGroupBox(legendData4);

	// 使用 QStackedLayout 存放
	m_stackedLayout->addWidget(legend1);
	m_stackedLayout->addWidget(legend2);
	m_stackedLayout->addWidget(legend3);
	m_stackedLayout->addWidget(legend4);

	QVBoxLayout *mainLayout = new QVBoxLayout(ui->groupBox);
	mainLayout->addLayout(m_stackedLayout);

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
}

void AgriPestAndDiseaseDistributeWidget::UpdateGridDisplayStatistic(DiffusionLayer difLayer, const std::vector<int>& difCount)
{
	QList<QPair<QString, std::string>> gridData = m_mDiffusionLayerHexColor.at(difLayer);

	if (gridData.size() != difCount.size()) return;

	for (int i = 0; i < gridData.size(); ++i)
	{
		gridData[i].second = QString::number(difCount[i]).toStdString();
	}

	// 清理栅格统计展示
	cleanUpGridDisplay();

	/*
	// 动态生成统计图
	QWidget *legend = createGridWidget(gridData);

	QVBoxLayout *mainLayout = new QVBoxLayout(ui->groupBox_2);
	mainLayout->addWidget(legend);
	mainLayout->addStretch(); // 推动内容到顶部
	*/
}

void AgriPestAndDiseaseDistributeWidget::UpdataDistributeBasicInfo()
{
	// 清理界面
	ui->dataset_name->clear();
	ui->field_area->clear();
	ui->collect_count->clear();

	if (m_pestDataset)
	{
		ui->dataset_name->setText(m_pestDataset->name);

		// 计算区域面积
		double area = 0.0;
		std::vector<QPointF> zonePoints;
		qnzkna::ZoneManage::CZoneBase zoneBase = LY_AgriDataAnalysisPredict::Get().GetActiveFieldZone();
		for (const auto& stZonePoint : zoneBase.stZonePointVec)
		{
			QPointF tmpPointF(stZonePoint.dLon, stZonePoint.dLat);
			zonePoints.emplace_back(tmpPointF);
		}
		AgriDataAnalysisPredictUtil::CalcPolygonArea(zonePoints,area);
		ui->field_area->setText(QString::number(AgriDataAnalysisPredictUtil::sqmToMu(area)));
		
		// 查询数据集采集点数量
		int tmpCount = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->GetPestCollectionCount(m_pestDataset->guid);
		ui->collect_count->setText(QString::number(tmpCount));

		int nIndex = ui->combo_crop_type->findData(m_pestDataset->crop_type);
		if (nIndex != -1)
		{
			ui->combo_crop_type->setCurrentIndex(nIndex);
		}
	}
}

void AgriPestAndDiseaseDistributeWidget::UpdataDistributeMapInfo()
{
	ui->analysis_name->clear();
	ui->growth_stage->clear();
	ui->combo_grid->setCurrentIndex(1);

	// 更新栅格图标展示
	ui->combo_diffuse->setCurrentIndex(1);
	emit ui->combo_diffuse->currentIndexChanged(ui->combo_diffuse->currentIndex());

	// 清理栅格展示信息
	cleanUpGridDisplay();

	if (m_pestDataset)
	{
		// 设置默认名称
		ui->analysis_name->setText(tr2("分布图_") + m_pestDataset->name);

		// 查询分布图信息
		auto disMapList = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->GetPestDistributeMap(m_pestDataset->guid);
		if (disMapList.size())
		{
			m_distributeMap = disMapList[disMapList.size() - 1];

			// 基本信息设置
			ui->analysis_name->setText(m_distributeMap->name);
			ui->growth_stage->setText(tr2("返青期")); // TODO: 生长阶段设置

			int nIndex = ui->combo_grid->findData(m_distributeMap->grid_unit);
			if (nIndex != -1)
			{
				ui->combo_grid->setCurrentIndex(nIndex);
			}	

			nIndex = ui->combo_diffuse->findData(static_cast<int>(m_distributeMap->diffuse_layer));
			if (nIndex != -1)
			{
				ui->combo_diffuse->setCurrentIndex(nIndex);
			}

			// 栅格信息统计
			std::vector<int> statisticnNmbers;
			for (const QString &numStr : m_distributeMap->distribute_statistic.split(",")) {
				statisticnNmbers.push_back(numStr.toInt());
			}

			DiffusionLayer diffuseLayer = AgriDataAnalysisPredictUtil::GetDiffusionLayer(m_distributeMap->diffuse_layer);
			UpdateGridDisplayStatistic(diffuseLayer, statisticnNmbers);
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

QWidget* AgriPestAndDiseaseDistributeWidget::createLegendGroupBox(const QList<QPair<QString, std::string>>& legendData, QWidget *parent /*= nullptr*/)
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

QWidget* AgriPestAndDiseaseDistributeWidget::createGridWidget(const QList<QPair<QString, std::string>>& gridData, QWidget *parent /*= nullptr*/)
{
	QWidget *groupBox = new QWidget(parent);
	QGridLayout *gridLayout = new QGridLayout();

	gridLayout->setSpacing(50);
	gridLayout->setContentsMargins(10, 10, 10, 10);

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

void AgriPestAndDiseaseDistributeWidget::cleanUpGridDisplay()
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

void AgriPestAndDiseaseDistributeWidget::cleanUpMap3D()
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

void AgriPestAndDiseaseDistributeWidget::on_btn_generate_clicked()
{
	if (!m_pestDataset)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("未选中数据集！"));
		return;
	}

	// 获取当前激活区域
	auto activeZone = LY_AgriDataAnalysisPredict::Get().GetActiveFieldZone();

	std::vector<QPointF> boundPoings;

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

	///////////////////////////////////////////////////////////////////////////////////////////
	/* 生成随机采样点到数据库
	AgriPestAndDiseaseDataGenerater dataGenerater;
	if (dataGenerater.generatePestDataCollection(m_pestDataset->guid, boundPoings, 50))
	{
		LyMessageBox::information(this, tr2("成功"), tr2("批量生成采集成功！"));
	}
	return;
	 */
	///////////////////////////////////////////////////////////////////////////////////////////

	double _spacing = 1.0; // 单位（米）
	int _equalCount = 0, _disCount = 1;

	/*
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(1, 10);
	*/

	// 获取选项信息
	int selectSpace = ui->combo_grid->itemData(ui->combo_grid->currentIndex()).value<int>(); //dist(gen);
	int selectDisCount = ui->combo_diffuse->itemData(ui->combo_diffuse->currentIndex()).value<int>(); //dist(gen);

	_spacing *= selectSpace;
	_disCount *= (selectDisCount-1);

	m_dGridSpace = _spacing;
	m_nLayerCount = _disCount;

	// 清理Map3D
	cleanUpMap3D();

	m_heatMap3D = new HeatMap3D;

	m_heatMap3D->setPixelSpacing(_spacing);
	m_heatMap3D->setEqualCount(_equalCount);
	m_heatMap3D->setDisCount(_disCount);

	qDebug() << "MapNode Params Coefficient : " << QString("_spacing: %1, _equalCount: %2, _disCount: %3")
		.arg(_spacing)
		.arg(_equalCount)
		.arg(_disCount);

	// 更新颜色列表
	DiffusionLayer difLayer = static_cast<DiffusionLayer>(ui->combo_diffuse->currentIndex() + 1);
	auto colorLayer = m_mDiffusionLayerOsgColor.at(difLayer);
	// 插入底色
	colorLayer.insert(colorLayer.begin(), HeatMap3DUtil::hexToColor("#30913C", 0.95f));
	m_heatMap3D->updateColors(colorLayer);

	if (boundPoings.size() > 0)
	{
		std::vector<double> _lons, _lats, _values;

		// 转换层级
		auto transferLevel = [&](DiffusionLayer layer, int level)->int {
			int transedLevel = 1;

			switch (layer)
			{
			case DiffusionLayer::DIFFUSION_LAYER_1:
				transedLevel = 1;
				break;
			case DiffusionLayer::DIFFUSION_LAYER_2:
				transedLevel = int((level - 1) / 5) + 1;
				break;
			case DiffusionLayer::DIFFUSION_LAYER_3:
				transedLevel = int((level - 1) / 2) + 1;
				break;
			case DiffusionLayer::DIFFUSION_LAYER_4:
				transedLevel = level;
				break;
			default:
				break;
			}
			return transedLevel;
		};


		/* 区域内随机生成点测试
		std::vector<QPointF> vecRandomPonit = AgriDataAnalysisPredictUtil::GenerateRandomPointsInPolygon(boundPoings,1000);
		for (auto& point : vecRandomPonit){
			_lons.emplace_back(point.x());
			_lats.emplace_back(point.y());
			_values.emplace_back(static_cast<int>(transferLevel(difLayer, dist(gen))));
		}
		 */

		// 获取数据集内采样点列表
		auto pestCollectionList = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->GetPestCollectionList(m_pestDataset->guid);
		if (pestCollectionList.empty())
		{
			LyMessageBox::warning(this, tr2("警告"), tr2("数据集不存在采样点，生成失败！"));
			return;
		}
		
		for (auto& point : pestCollectionList) {
			_lons.emplace_back(point->lon);
			_lats.emplace_back(point->lat);
			_values.emplace_back(static_cast<int>(transferLevel(difLayer, point->level)));
		}

		m_heatMap3D->setBoundPoints(boundPoings);
		m_heatMap3D->setLonLatValue(_lons, _lats, _values);

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

		osgViewer::Viewer* viewer = (osgViewer::Viewer*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_OsgViewer));

		m_heatMap3D->setMapNode(mapNode);

		m_heatMap3D->setViewer(viewer);

		bool cordIsChecked = ui->checkBox_coordinate->isChecked();
		bool gridIsChecked = ui->checkBox_grid->isChecked();

		// 匹配生成方式
		int selectType = ui->combo_generate_type->itemData(ui->combo_generate_type->currentIndex()).value<int>();
		switch (static_cast<GenerateType>(selectType))
		{
		case GenerateType::GENERATE_EQUAL:
		{
			// TODO: 修改旋转角度
			double angle = osg::DegreesToRadians(10.0);
			m_heatMap3D->startCreateHeatMap(cordIsChecked, gridIsChecked, angle);
			m_heatMap3D->setGridLineShowOrHide(true);
		}
			break;
		case GenerateType::GENERATE_DYNAMIC:
		{
			HeatMap3D::SpreadInitInfo spreadInfo;
			spreadInfo.spreadtimeSec = ui->dync_spread_time->text().toDouble() * 60;
			spreadInfo.timeStepSec = ui->dync_record_gap->text().toDouble() * 60;
			spreadInfo.tempo = ui->dync_temperature->text().toDouble();
			spreadInfo.shidu = ui->dync_humidity->text().toDouble();
			spreadInfo.fengxiang = ui->combo_wind_direct->itemData(ui->combo_wind_direct->currentIndex()).value<int>();
			spreadInfo.fengsu = ui->dync_wind_speed->text().toDouble();
			m_heatMap3D->startCreateHeatMap2(spreadInfo, cordIsChecked, gridIsChecked);
		}
			break;
		default:
			break;
		}

		// 栅格显示统计
		if (m_heatMap3D)
		{
			std::vector<int> statistics;
			m_heatMap3D->getPixelStatistic(statistics);

			// 移除第一项数据(底图)
			if (!statistics.empty())
			{
				statistics.erase(statistics.begin());
			}

			UpdateGridDisplayStatistic(difLayer, statistics);
		}
	}
}

void AgriPestAndDiseaseDistributeWidget::on_btn_save_clicked()
{
	if (!m_pestDataset)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("未选中数据集！"));
		return;
	}

	AgriPestDistributeMapPtr pPestDistributeMap = std::make_shared<AgriPestDistributeMap>();
	pPestDistributeMap->name = ui->analysis_name->text();
	pPestDistributeMap->dataset_id = m_pestDataset->guid;
	pPestDistributeMap->crop_type = m_pestDataset->crop_type;
	pPestDistributeMap->growth_stage = 0; // TODO:生长阶段划分
	pPestDistributeMap->field_area = ui->field_area->text().toDouble();
	pPestDistributeMap->collect_count = ui->collect_count->text().toInt();
	pPestDistributeMap->grid_unit = ui->combo_grid->itemData(ui->combo_grid->currentIndex()).value<int>();
	pPestDistributeMap->diffuse_layer = ui->combo_diffuse->itemData(ui->combo_diffuse->currentIndex()).value<int>();

	auto joinStatistic = [&]() {
		std::vector<int> statistics;
		m_heatMap3D->getPixelStatistic(statistics);

		// 移除第一项数据(底图)
		if (!statistics.empty()){
			statistics.erase(statistics.begin());
		}

		QStringList strList;
		for (int num : statistics) {
			strList.append(QString::number(num));
		}
		pPestDistributeMap->distribute_statistic = strList.join(",");
	};

	if (m_distributeMap)
	{
		// 修改分布图
		if (m_heatMap3D){
			joinStatistic();
		}
		else{
			pPestDistributeMap->distribute_statistic = m_distributeMap->distribute_statistic;
			pPestDistributeMap->diffuse_layer = m_distributeMap->diffuse_layer; // 未重新生成，分布层数不做改变
		}

		pPestDistributeMap->guid = m_distributeMap->guid;

		bool isSuccess = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->UpdatePestDistributeMap(pPestDistributeMap);
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

		pPestDistributeMap->guid = AgriDataAnalysisPredictUtil::GenerateUuid();

		joinStatistic();

		bool isSuccess = LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->AddPestDistributeMap(pPestDistributeMap);
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

void AgriPestAndDiseaseDistributeWidget::on_btn_show_control_clicked()
{
	if (m_heatMap3D)
	{
		m_heatMap3D->setImageShowOrHide(!m_bMap3DShow);
		m_bMap3DShow = !m_bMap3DShow;
	}
}

void AgriPestAndDiseaseDistributeWidget::on_checkBox_coordinate_stateChanged(int nStates)
{
	if (m_heatMap3D)
	{
		m_heatMap3D->setCoordinateShowOrHide(!!nStates);
	}
}

void AgriPestAndDiseaseDistributeWidget::on_checkBox_grid_stateChanged(int nStates)
{
	if (m_heatMap3D)
	{
		m_heatMap3D->setGridLineShowOrHide(!!nStates);
	}
}

void AgriPestAndDiseaseDistributeWidget::on_comboBox_gene_type_currentTextChanged(int nIndex)
{
	int selectType = ui->combo_generate_type->itemData(nIndex).value<int>();

	for (int i = 0; i < ui->gridLayout_2->count(); ++i) {
		QWidget *w = ui->gridLayout_2->itemAt(i)->widget();
		if (w) {
			switch (static_cast<GenerateType>(selectType))
			{
			case GenerateType::GENERATE_EQUAL:
				w->hide();
				break;
			case GenerateType::GENERATE_DYNAMIC:
				w->setVisible(true);
				break;
			default:
				break;
			}
		}
	}
}
