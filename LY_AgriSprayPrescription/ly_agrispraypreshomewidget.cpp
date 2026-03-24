#include "ly_agrispraypreshomewidget.h"
#include "ui_ly_agrispraypreshomewidget.h"
#include "LY_AgriSprayPrescriptionActivator.h"
#include <QApplication>
#include <QHeaderView>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QSettings>
#include <QDebug>
#include <QCheckBox>
#include <QTableWidgetItem>
#include "DataManage/AgriFarm/AgriSprayPrescriptDecl.h"
#include "LY_SprayPrescriptionUntil.h"
#include "LyMessageBox.h"
#include <random>
#include "ly_customslider.h"
#include "LY_AgriSprayPrescriptionInterface.h"

using namespace AgriSprayPrescriptionDecl;

class LY_AgriSprayTreeWidget;


LY_AgriSprayPresHomeWidget::LY_AgriSprayPresHomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_AgriSprayPresHomeWidget)
{
    ui->setupUi(this);
    m_sIconPath = (":/images/mointor/");
	setupUI();
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
			ui->farm_name_L->setText(m_farmName);
			m_pestDataVec.clear();
			m_pestDataVec = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->GetPestList(QStringList(m_pFieldInfo->id));
			m_listVec.clear();
			m_listVec = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->GetSprayPrescription(m_pFieldInfo->id);
			ui->data_source_box->clear();
			for (auto pest : m_pestDataVec)
			{
				ui->data_source_box->addItem(pest.name);
			}
			ui->slider->setupSlider({ 5 });
			// 更新表格展示数据
			setListData();
		}
		clearnBox();
	});

}
void LY_AgriSprayPresHomeWidget::updataCurActiveFarm()
{
	//ui->field_tree_widget->init();
	m_pAgrisprayComboBox->initData();
}

 void LY_AgriSprayPresHomeWidget::setupUI()
 {
	 ui->pyfk_box->addItem("");
	 ui->pyfk_box->addItem("4");
	 ui->pyfk_box->addItem("6");
	 QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	 ini.beginGroup("Agri");
	 QString  m_currentFarm = ini.value("FarmID", "").toString();
	 AgriFieldInfoVec		m_vecFieldList;
	 LY_AgriSprayPrescriptionActivator::Get().getPIAgriFarmManagerService()->GetFieldInfoList(m_vecFieldList);
	 m_farmName = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->GetFarmName(m_currentFarm);

	 QStringList header_list;
	 header_list << tr2("序号") << tr2("盐田名称") << tr2("处方图名称") << tr2("虫害分布图") << tr2("作物类型")<< tr2("作业方式");
	
	 ui->tableWidget->setColumnCount(6);
	 ui->tableWidget->setHorizontalHeaderLabels({
		 tr2("序号"),
		 tr2("盐田名称"),
		 tr2("处方图名称"),
		 tr2("虫害分布图"),
		 tr2("作物类型"),
		 tr2("作业方式"),
	 });

         ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
         ui->tableWidget->verticalHeader()->setVisible(false);
         ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
         ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
         connect(ui->tableWidget, &QTableWidget::cellClicked, this, &LY_AgriSprayPresHomeWidget::onCellClicked);
         connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &LY_AgriSprayPresHomeWidget::onCellDoubleClicked);

		connect(ui->data_source_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_dataSourceComboBoxChanged(const int &)));
		 connect(ui->map_combox, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_mapComboBoxChanged(const int &)));
	
		// 连接单击信号，监听用户的单击操作
		connect(ui->tableWidget, &QTableWidget::cellClicked, this, &LY_AgriSprayPresHomeWidget::onCellClicked);
		connect(ui->ptn_add_dataset, &QPushButton::clicked, this, &LY_AgriSprayPresHomeWidget::on_ptn_add_dataset_clicked);
		connect(ui->ptn_update_dataset, &QPushButton::clicked, this, &LY_AgriSprayPresHomeWidget::on_ptn_update_dataset_clicked);
		connect(ui->slider, &LY_CustomSlider::intervalsChanged, this, &LY_AgriSprayPresHomeWidget::on_sliderValuesChanged);
		connect(ui->cancle_btn, &QPushButton::clicked, this, &LY_AgriSprayPresHomeWidget::on_show_hide_clicked);

		ui->field_tree_widget->hide();
		m_pAgrisprayComboBox = new AgrisprayComboBox(this);
		auto vLayout =(QVBoxLayout*)(ui->main_widget->layout());
		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(new QLabel(tr2("当前盐田：")), 0);
		hlayout->addWidget(m_pAgrisprayComboBox, 1);
		vLayout->insertLayout(0, hlayout);

		connect(m_pAgrisprayComboBox, &AgrisprayComboBox::setupField, this, &LY_AgriSprayPresHomeWidget::addField);
 }
 void LY_AgriSprayPresHomeWidget::addField(QString &text, QString & field_id)
 {

	 ui->cropland_name_L->setText(text);
	 m_currentFieldID = field_id;
 }
 void  LY_AgriSprayPresHomeWidget::setListData()
 {

	if (m_listVec.size() == 0)
	{
		//m_type = PrescretionConcolType::PrescretionConcolType_ADD;
		QStringList header_list;
		ui->tableWidget->clearContents();
		ui->tableWidget->setRowCount(0);
		return;
	}
	ui->tableWidget->setRowCount(m_listVec.size());
	int index = 0;
	int row = 1;
	for (auto model : m_listVec)
	{
		QTableWidgetItem *item = new QTableWidgetItem(QString::number(row));
		QTableWidgetItem *item2 = new QTableWidgetItem(m_pFieldInfo->name);
		QTableWidgetItem *item3 = new QTableWidgetItem(model.map_name);

		ui->tableWidget->setItem(index, 0, item);
		//ui->tableWidget->setCellWidget(index, 1, radioWidget);

		item->setTextAlignment(Qt::AlignCenter);
		item2->setTextAlignment(Qt::AlignCenter);
		item3->setTextAlignment(Qt::AlignCenter);

		ui->tableWidget->setItem(index, 1, item2);
		ui->tableWidget->setItem(index, 2, item3);

		for (auto pest : m_pestDataVec)
		{
			if (model.data_source_id == pest.guid)
			{
				QTableWidgetItem *item3 = new QTableWidgetItem(LY_SprayPrescriptionUntil::CropTypeToString(static_cast<AgriSprayPrescriptionDecl::CropType>(pest.crop_type)));
				QTableWidgetItem *item4 = new QTableWidgetItem(pest.name);
				ui->tableWidget->setItem(index, 3, item4);
				ui->tableWidget->setItem(index, 4, item3);

				item3->setTextAlignment(Qt::AlignCenter);
				item4->setTextAlignment(Qt::AlignCenter);
				break;
			}
		}
		QTableWidgetItem *item9 = new QTableWidgetItem(LY_SprayPrescriptionUntil::WorkStatueToString(static_cast<WorkType>(model.work_type)));
		ui->tableWidget->setItem(index, 5, item9);
		item9->setTextAlignment(Qt::AlignCenter);
		index++;
		row++;
	}
 }
 void LY_AgriSprayPresHomeWidget::on_dataSourceComboBoxChanged(const int &)
 {

	 ui->pest_stage_box->clear();
	 ui->pest_type_box->clear();
	 ui->crop_type_box->clear();
	 ui->pes_map_box->clear();

	 ui->first_area_L->setText(tr2("亩"));
	 ui->second_area_L->setText(tr2("亩"));
	 ui->three_area_L->setText(tr2("亩"));
	 ui->four_area_L->setText(tr2("亩"));
	 ui->five_area_L->setText(tr2("亩"));
	
	 for (auto model : m_pestDataVec) {

		 if (model.name == ui->data_source_box->currentText())
		 {
			 addPestData(model);
			 break;
		 }

	 }
 }

 void LY_AgriSprayPresHomeWidget::on_mapComboBoxChanged(const int &)
 {
	 std::vector<QGroupBox *> tempVec = { ui->first_group_box, ui->second_group_box, ui->three_group_box, ui->four_group_box, ui->five_group_box };

	 int value = ui->map_combox->currentText().toInt();
	 std::vector<std::pair<int, int>> intervals;
	 ui->highL->setText(QString::number(m_pestMaoInfo.diffuse_layer));

	 for (size_t i = 0; i < tempVec.size(); i++)
	 {
		 tempVec[i]->setFixedHeight(64);
		 tempVec[i]->setMaximumHeight(64);
		 tempVec[i]->hide();
		 QSizePolicy policy = tempVec[i]->sizePolicy();
		 policy.setRetainSizeWhenHidden(true);
		 tempVec[i]->setSizePolicy(policy);
	 }

	 //一级一下单独处理
	 if (m_pestMaoInfo.diffuse_layer == 1)
	 {
		 ui->slider->setEnabled(false);
		 ui->slider->setMaxValue(5);
		 ui->slider->setupSlider({5});
		 intervals.push_back(std::make_pair(1, m_pestMaoInfo.diffuse_layer));

		 on_sliderValuesChanged(intervals);

		 tempVec[0]->show();
		 return;
	 }
	 ui->slider->setEnabled(true);

	 switch (value)
	 {
	 case 1:
	 {
		 ui->slider->setEnabled(false);
		 intervals.push_back(std::make_pair(1, m_pestMaoInfo.diffuse_layer));
		 ui->slider->setMaxValue(m_pestMaoInfo.diffuse_layer);
		 ui->slider->setupSlider({ m_pestMaoInfo.diffuse_layer });
		 tempVec[0]->show();
		 on_sliderValuesChanged(intervals);
	 }
	 break;
	 case 2:
	 {
		 ui->slider->setEnabled(true);

		 if (m_pestMaoInfo.diffuse_layer == 2)
		 {

			 ui->slider->setMaxValue(2);
			 ui->slider->setupSlider({ 2 });
			 intervals.push_back(std::make_pair(1, 1));
			 intervals.push_back(std::make_pair(1, 2));
			 on_sliderValuesChanged(intervals);

			 tempVec[0]->show();
			 tempVec[1]->show();
			 break;
		 }

		 if (m_pestMaoInfo.diffuse_layer == 5)
		 {
			 ui->slider->setupSlider({ 3 });
			 intervals.push_back(std::make_pair(1, 3));
			 intervals.push_back(std::make_pair(3, 5));
		 }
		 else  if (m_pestMaoInfo.diffuse_layer == 10)
		 {
			 ui->slider->setupSlider({ 5 });
			 intervals.push_back(std::make_pair(1, 5));
			 intervals.push_back(std::make_pair(5, 10));
		 }
		 on_sliderValuesChanged(intervals);
	 }
	 break;
	 case 3:
		 ui->slider->setEnabled(true);
		 if (m_pestMaoInfo.diffuse_layer == 5)
		 {
			 ui->slider->setupSlider({ 2,4 });
			 intervals.push_back(std::make_pair(1, 2));
			 intervals.push_back(std::make_pair(2, 4));
			 intervals.push_back(std::make_pair(4, 5));
		 }
		 else if (m_pestMaoInfo.diffuse_layer == 10)
		 {
			 ui->slider->setupSlider({ 4,7 });
			 intervals.push_back(std::make_pair(1, 4));
			 intervals.push_back(std::make_pair(5, 7));
			 intervals.push_back(std::make_pair(7, 10));
		 }

		 on_sliderValuesChanged(intervals);

		 break;
	 case 4:
	 {
		 ui->slider->setEnabled(true);


		 if (m_pestMaoInfo.diffuse_layer == 5)
		 {

			 ui->slider->setupSlider({ 2,3,4 });
			 intervals.push_back(std::make_pair(1, 1));
			 intervals.push_back(std::make_pair(1, 2));
			 intervals.push_back(std::make_pair(2, 3));
			 intervals.push_back(std::make_pair(3, 4));
		 }
		 else if (m_pestMaoInfo.diffuse_layer == 10)
		 {
			 ui->slider->setupSlider({ 2,4,8 });
			 intervals.push_back(std::make_pair(1, 2));
			 intervals.push_back(std::make_pair(2, 4));
			 intervals.push_back(std::make_pair(4, 8));
			 intervals.push_back(std::make_pair(8, 10));
		 }
		 on_sliderValuesChanged(intervals);
	 }
	 break;
	 case 5:
	 {
		 ui->slider->setEnabled(true);

		 if (m_pestMaoInfo.diffuse_layer == 5)
		 {
			 ui->slider->setupSlider({ 1,2,3,4 });
			 intervals.push_back(std::make_pair(1, 1));
			 intervals.push_back(std::make_pair(1, 2));
			 intervals.push_back(std::make_pair(2, 3));
			 intervals.push_back(std::make_pair(3, 4));
			 intervals.push_back(std::make_pair(4, 5));
		 }
		 else if (m_pestMaoInfo.diffuse_layer == 10)
		 {
			 ui->slider->setupSlider({ 2,4,6,8 });
			 intervals.push_back(std::make_pair(1, 2));
			 intervals.push_back(std::make_pair(2, 4));
			 intervals.push_back(std::make_pair(4, 6));
			 intervals.push_back(std::make_pair(6, 8));
			 intervals.push_back(std::make_pair(8, 10));
		 }

		 on_sliderValuesChanged(intervals);
	 }
	 break;
	 default:
		 break;
	 }
	 if (m_pestMaoInfo.diffuse_layer > 2)
	 {
		 for (size_t i = 0; i < tempVec.size(); i++)
		 {
			 if (i < value)
			 {
				 tempVec[i]->show();
			 }
			 else
			 {
				 tempVec[i]->hide();
			 }
		 }
	 }
 }

 void LY_AgriSprayPresHomeWidget::on_sliderValuesChanged(const std::vector<std::pair<int, int>>& intervals)
 {
	 firstLeveVec.clear();
	 secondLeveVec.clear();
	 threeLeveVec.clear();
	 fourLeveVec.clear();
	 fiveLeveVec.clear();
	 pestLevVec.clear();

	 int i = 0;
	 for (auto tempValue : intervals)
	 {
		 int index;

		 if (i == 0)
		 {
			 ui->fiest_pest_leve->setText(QString("%1-%2").arg(tempValue.first).arg(tempValue.second));
			 for (size_t i = 1; i <= tempValue.second; i++)
			 {
				 firstLeveVec.push_back(i);
			 }
		 }
		 else if (i == 1)
		 {
			 ui->second_pest_leve->setText(QString("%1-%2").arg(tempValue.first + 1).arg(tempValue.second));
			 for (size_t i = tempValue.first + 1; i <= tempValue.second; i++)
			 {
				 secondLeveVec.push_back(i);
			 }
		 }
		 else if (i == 2)
		 {
			 ui->three_pest_leve->setText(QString("%1-%2").arg(tempValue.first + 1).arg(tempValue.second));
			 for (size_t i = tempValue.first + 1; i <= tempValue.second; i++)
			 {
				 threeLeveVec.push_back(i);
			 }
		 }
		 else if (i == 3)
		 {
			 ui->four_pest_leve->setText(QString("%1-%2").arg(tempValue.first + 1).arg(tempValue.second));
			 for (size_t i = tempValue.first + 1; i <= tempValue.second; i++)
			 {
				 fourLeveVec.push_back(i);
			 }
		 }
		 else if (i == 4)
		 {
			 ui->five_pest_leve->setText(QString("%1-%2").arg(tempValue.first + 1).arg(tempValue.second));
			 for (size_t i = tempValue.first + 1; i <= tempValue.second; i++)
			 {
				 fiveLeveVec.push_back(i);
			 }
		 }
		 i++;
	 }

	 if (firstLeveVec.size() > 0)
	 {
		 pestLevVec.push_back(firstLeveVec);
	 }
	 if (secondLeveVec.size() > 0)
	 {
		 pestLevVec.push_back(secondLeveVec);
	 }
	 if (threeLeveVec.size() > 0)
	 {
		 pestLevVec.push_back(threeLeveVec);
	 }
	 if (fourLeveVec.size() > 0)
	 {
		 pestLevVec.push_back(fourLeveVec);
	 }
	 if (fiveLeveVec.size() > 0)
	 {
		 pestLevVec.push_back(fiveLeveVec);
	 }


 }
 
 void LY_AgriSprayPresHomeWidget::changeEvent(QEvent *event)
 {
     QWidget::changeEvent(event);
 }

 void LY_AgriSprayPresHomeWidget::showEvent(QShowEvent *event)
 {
	 m_show = true;
	 isClickCompute = false;
 }

 void LY_AgriSprayPresHomeWidget::hideEvent(QHideEvent *event)
 {
	 QMutexLocker locker(&sMutex);
	 m_show = false;
	 isClickCompute = false;
	
	 m_map3D.cleanMap();
	 m_pAgrisprayComboBox->clearZoneHighLight();
	 m_type = PrescretionConcolType::PrescretionConcolType_NONE;

 }
 void LY_AgriSprayPresHomeWidget::drawDynamic(QPainter &painter, IProj *pProj)
 {
	 if (pProj == nullptr)
	 {
		 return;
	 }

	 if (m_show)
	 {
		 //ui->field_tree_widget->zoneHighLight(painter, pProj);
		 m_pAgrisprayComboBox->zoneHighLight(painter, pProj);
	 }
 }


 void LY_AgriSprayPresHomeWidget::onCellClicked(int row, int column)
 {
	 setComboxStatus(false);
	 QTableWidgetItem *item = ui->tableWidget->item(row, column);
	 if (item) {
		 m_selectedCheckBoxRow = row;
		 setupItemData(row);
	 }

 }
 void LY_AgriSprayPresHomeWidget::onCellDoubleClicked(int row, int column)
 {

 }
 //暂时没用
 void LY_AgriSprayPresHomeWidget::onCheckboxChanged(int row, int col, int state)
 {

	 //check单选
	 if (m_selectedCheckBoxRow > 0)
	 {
		 QWidget *container = qobject_cast<QWidget*>(ui->tableWidget->cellWidget(m_selectedCheckBoxRow, 1));
		 if (container) {
			 QCheckBox *otherCheckBox = container->findChild<QCheckBox*>();
			 if (otherCheckBox) {
				 otherCheckBox->setChecked(false);
			 }
		 }
	 }
	 bool checked = (state == Qt::Checked);

	 if (checked) {

		 m_selectedCheckBoxRow = row;
		 setupItemData(row);
	 }	
 }

 //设置单个Item的数据
 void LY_AgriSprayPresHomeWidget::setupItemData(int row)
 {
	 if (row < 0)
	 {
		 return;
	 }
	 QTableWidgetItem *item = ui->tableWidget->item(row, 0);
	 if (item)
	 {
		 if (row >= m_listVec.size())
		 {
			 return;
		 }
		 AgriSprayPrescriptionModel dataset = m_listVec.at(row);
		 for (auto pest : m_pestDataVec)
		 {
			 if (dataset.data_source_id == pest.guid)
			 {
				 addPestData(pest);

				 ui->spray_edit->setText(dataset.map_name);
				 ui->map_combox->setCurrentText(QString::number(dataset.map_level));
				// qDebug() << "====11:" << QString::number(dataset.pyfk);
				 ui->pyfk_box->setCurrentText(QString::number(dataset.pyfk));
				 break;
			 }
		 }
		 int index = 0;
		 std::vector<QLabel *> tempVec = { ui->fiest_pest_leve,ui->second_pest_leve,ui->three_pest_leve, ui->four_pest_leve, ui->five_pest_leve };
		 for (auto const &intervals : dataset.leve_arr) {
			 for (size_t i = 0; i < tempVec.size(); i++)
			 {
				 if (index == i)
				 {
					 QLabel *tempL = tempVec.at(i);
					 tempL->setText(QString("%1-%2").arg(intervals.front()).arg(intervals.back()));
					 break;
				 }
			 }
			 index++;
		 }
		 
		std::vector<QLabel *> tempValueVec = {ui->first_area_L, ui->second_area_L, ui->three_area_L, ui->four_area_L, ui->five_area_L};
		 int j = 0;
		 for (auto const &value : dataset.leve_value)
		 {
			 for (size_t i = 0; i < tempValueVec.size(); i++)
			 {
				 if (j == i)
				 {
					 QLabel *tempL = tempValueVec.at(i);
					 tempL->setText(QString::number(value, 'f', 4) + tr2("亩"));
					 break;
				 }
			 }
			 j++;
		 }
		// qDebug() << dataset.leve_arr << dataset.leve_value;
		 //计算类型
		 if (dataset.compute_type == 2)
		 {
			 //TODO::
			 // ui->tableWidget->selectedItems(1);
		 }
	 }
 }

 void LY_AgriSprayPresHomeWidget::addPestData(AgriPestDataset pest)
 {
	 if (pest.guid == nullptr)
	 {
		 return;
	 }
	 ui->spray_edit->setText(tr2("处方图_") + pest.name);
	 m_currentGuid = pest.guid;

	 m_pestMaoInfo = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->GetPestInfoMap(pest.guid);

	 if (m_pestMaoInfo.diffuse_layer >= 5)
	 {
		 leveValue = 5;
	 }
	 else
	 {
		 leveValue = m_pestMaoInfo.diffuse_layer;
	 }
	 ui->map_combox->clear();
	 for (int i = 1; i <= leveValue; i++)
	 {
		 ui->map_combox->addItem(QString::number(i));
	 }

	 // 一级
	 QList<QPair<QString, std::string>> legendData1 = {
		 { "#3EA044", "1" }
	 };
	 // 二级
	 QList<QPair<QString, std::string>> legendData2 = {
		 { "#3EA044", "1" },{ "#8BC34A", "2" }
	 };
	 // 三级
	 QList<QPair<QString, std::string>> legendData3 = {
		 { "#3EA044", "1" },{ "#8BC34A", "2" },{ "#FFC107", "3" }
	 };
	 // 四级
	 QList<QPair<QString, std::string>> legendData4 = {
		 { "#3EA044", "1" },{ "#8BC34A", "2" },{ "#FFC107", "3" },{ "#FF5722", "4" }
	 };
	 // 五级
	 QList<QPair<QString, std::string>> legendData5 = {
		 { "#3EA044", "1" },{ "#8BC34A", "2" },{ "#FFC107", "3" },{ "#FF5722", "4" },{ "#C62828", "5" }
	 };

	 m_mDiffusionLayerHexColor.insert(std::make_pair(1, legendData1));
	 m_mDiffusionLayerHexColor.insert(std::make_pair(2, legendData2));
	 m_mDiffusionLayerHexColor.insert(std::make_pair(3, legendData3));
	 m_mDiffusionLayerHexColor.insert(std::make_pair(4, legendData4));
	 m_mDiffusionLayerHexColor.insert(std::make_pair(5, legendData5));

	 // 十六进制的 RGB 颜色值osgEarth::Symbology::Color 对象进行存储
	 for (const auto& pair : m_mDiffusionLayerHexColor)
	 {
		 std::vector<osgEarth::Symbology::Color> osgColor;
		 for (const auto& legend : pair.second)
		 {
			 osgEarth::Symbology::Color c = hexToColor(legend.first.toStdString());
			 osgColor.emplace_back(c);
		 }
		 m_mDiffusionLayerOsgColor.insert(std::make_pair(pair.first, osgColor));
	 }

	 ui->pest_stage_box->addItem(LY_SprayPrescriptionUntil::PestDiseaseToString(static_cast<PestDisease>(pest.pest_disease)));
	 ui->pest_type_box->addItem(LY_SprayPrescriptionUntil::PestCategoryToString(static_cast<PestCategory>(pest.pest_category)));
	 ui->crop_type_box->addItem(LY_SprayPrescriptionUntil::CropTypeToString(static_cast<AgriSprayPrescriptionDecl::CropType>(pest.crop_type)));
	 ui->pes_map_box->addItem(m_pestMaoInfo.name);
	 if (pest.crop_type == 1)
	 {
		 //TODO: 生长阶段
		 auto pestVec = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->GetGrowthStageList();
		 for (auto m : pestVec) {

		 }
	 }
	 ui->crop_type_box->setCurrentText(LY_SprayPrescriptionUntil::CropTypeToString(static_cast<AgriSprayPrescriptionDecl::CropType>(pest.crop_type)));
	 ui->data_source_box->setCurrentText(pest.name);
	 ui->pest_type_box->setCurrentText(LY_SprayPrescriptionUntil::PestCategoryToString(static_cast<PestCategory>(pest.pest_category)));
	 ui->pest_stage_box->setCurrentText(LY_SprayPrescriptionUntil::PestDiseaseToString(static_cast<PestDisease>(pest.pest_disease)));
	 ui->pes_map_box->setCurrentText(m_pestMaoInfo.name);

	 on_mapComboBoxChanged(0);

 }
 void LY_AgriSprayPresHomeWidget::on_ptn_add_dataset_clicked()
 {
	 setComboxStatus(true);
	 m_type = PrescretionConcolType::PrescretionConcolType_ADD;
	 clearnBox();
 }

 void LY_AgriSprayPresHomeWidget::on_ptn_update_dataset_clicked()
 {
	 m_type =PrescretionConcolType::PrescretionConcolType_EDIT;
	 if (m_selectedCheckBoxRow < 0)
	 {
		 return;
	 }
	 setComboxStatus(true);
	 setupItemData(m_selectedCheckBoxRow);
 }

 void LY_AgriSprayPresHomeWidget::on_ptn_delete_dataset_clicked()
 {

	 int row = ui->tableWidget->currentRow();
	 if (row >= 0) {

		 QTableWidgetItem *item = ui->tableWidget->item(row, 0);
		 if (item)
		 {
			 if (row >= m_listVec.size())
			 {
				 return;
			 }
			 AgriSprayPrescriptionModel dataset = m_listVec.at(row);

			 if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除处方图[%1]？").arg(dataset.map_name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			 {
				 bool isSuccess = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->DeleteSprayPrescription(dataset.guid);
				 if (!isSuccess)
				 {
					 LyMessageBox::warning(this, "错误", "数据库删除失败！");
				 }
				 else
				 {
					 ui->tableWidget->removeRow(row); // 删除该行
					 clearnBox();

				 }
			 }
		 }
	 }
	 else {
		 LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行进行删除"));
	 }

 }
 void LY_AgriSprayPresHomeWidget::clearnBox()
 {
	 ui->data_source_box->setCurrentIndex(-1);
	 ui->data_source_box->setCurrentText("");
	 ui->pes_map_box->setCurrentIndex(-1);
	 ui->pes_map_box->setCurrentText("");

	 ui->crop_type_box->setCurrentIndex(-1);
	 ui->crop_type_box->setCurrentText("");
	 ui->spray_edit->setText("");
	 ui->pest_type_box->setCurrentIndex(-1);
	 ui->pest_type_box->setCurrentText("");
	 ui->pest_stage_box->setCurrentIndex(-1);
	 ui->pest_stage_box->setCurrentText("");
 }

 void LY_AgriSprayPresHomeWidget::on_ptn_export_dataset_clicked()
 {

 }
LY_AgriSprayPresHomeWidget::~LY_AgriSprayPresHomeWidget()
{
    delete ui;
}
//绘制处方图
void  LY_AgriSprayPresHomeWidget::draw3DGridMap(AgriSprayPrescriptionModel &sprayModel)
{
	if (m_map3D.keyCounts.size() > 0)
	{
		m_map3D.cleanMap();
	}

	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	HeatMap3D map3D(mapNode);

	std::vector<QPointF> boundPoings;
	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = LY_AgriSprayPrescriptionActivator::Get().getPIZoneManageService()->GetAllZoneBase();

	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == m_currentFieldID.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			double curLon = itrZone.stRoundParameter.dLon;
			double curLat = itrZone.stRoundParameter.dLat;

			if (itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
				itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
				if (itrZone.stZonePointVec.size() > 0) {

					for (auto zone : itrZone.stZonePointVec)
					{
						QPointF pointF;
						pointF.setX(zone.dLon);
						pointF.setY(zone.dLat);
						boundPoings.push_back(pointF);
					}
				}
			}
		}
	}

	int source_box_index = ui->data_source_box->currentIndex();
	if (source_box_index < 0 || source_box_index > m_pestDataVec.size())
	{
		return;
	}
	AgriPestDataset model = m_pestDataVec.at(source_box_index);
	auto tempVec = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->GetPestCollectionList(model.guid);

	std::vector<double> _lons;
	std::vector<double> _lats;
	std::vector<double> _values;

	for (auto m : tempVec)
	{
		_lons.push_back(m.lon);
		_lats.push_back(m.lat);
		_values.push_back(static_cast<double>(m.level));
	}

	if (boundPoings.size() > 0)
	{
		map3D.setSpace(ui->pyfk_box->currentText().toDouble());
		map3D.setBoundPoints(boundPoings);

		std::vector<Poi> difPoints;

		AgriDataAnalysisPredictAlgorithm::createGridPointDiffuse(boundPoings, _lons, _lats, _values, 1.0 * (1.0 / 110000.0), m_pestMaoInfo.diffuse_layer, difPoints);

		std::vector<double> _newLons;
		std::vector<double> _newlats;
		std::vector<double> _newvalues;
		for (const auto &poi : difPoints)
		{
			_newLons.push_back(poi.lon);
			_newlats.push_back(poi.lat);
			_newvalues.push_back(poi.alt);
		}

		map3D.setLonLatValue(_newLons, _newlats, _newvalues);

		auto colorLayer = m_mDiffusionLayerOsgColor.at(sprayModel.map_level);
		// 插入底色#30913C
		colorLayer.insert(colorLayer.begin(), hexToColor("#1e6527", 0.95f));
		map3D.updateColors(colorLayer);

		double angle = osg::DegreesToRadians(10.0);
		map3D.startCreateHeatMap_forDisPosition(angle);

		map3D.setGridLineShowOrHide(true);

		m_map3D = map3D;

		//qDebug() << "=======**********=====111" << psetLeveallDataMap;
		for (const auto &psetPair : psetLeveallDataMap) {
			std::pair<std::vector<int>, int> pair = psetPair.second;
			const std::vector<int> &arr = pair.first;
			int tempValue = pair.second;
			double d1 = 0.00;

			//qDebug() << "=====**********===2222:" << arr <<  d1;
			QString str;
			QString str1;

			//qDebug() << "=======**********=====123" << map3D.keyCounts << "\n" << arr;
			for (auto it : map3D.keyCounts.keys()) {

				if (std::find(arr.begin(), arr.end(), it) != arr.end()) {
					//qDebug() << "=======**********=====333" << map3D.keyCounts;
					double d = map3D.keyCounts.value(it) * 16 / 666.67;
					d1 = d1 + d;
					str1 = QString::number(d1, 'f', 4);
					//qDebug() << "=====**********===111:" << it << d << d1;
					double d2 = d1 * static_cast<double>(tempValue);
					str = QString::number(d2, 'f', 4);
					//qDebug() << "=====**********===111:" << it << d << d1 << static_cast<double>(tempValue) << d2 << str;
				}
			}
			tempMap[arr] = str1;
			levpValueMap[arr] = str;
		}
		//qDebug() << "=====**********===444:" << tempMap;

		for (auto const &PestLevel : tempMap)
		{
			if (containsAll(firstLeveVec, PestLevel.first))
			{
				ui->first_area_L->setText(PestLevel.second + tr2("亩"));
			}else
			if (containsAll(secondLeveVec, PestLevel.first))
			{
				ui->second_area_L->setText(PestLevel.second + tr2("亩"));
			}else
			if (containsAll(threeLeveVec, PestLevel.first))
			{
				ui->three_area_L->setText(PestLevel.second + tr2("亩"));
			}else
			if (containsAll(fourLeveVec, PestLevel.first))
			{
				ui->four_area_L->setText(PestLevel.second + tr2("亩"));
			}else
			if (containsAll(fiveLeveVec, PestLevel.first))
			{
				ui->five_area_L->setText(PestLevel.second + tr2("亩"));
			}

		}
	}
}
void LY_AgriSprayPresHomeWidget::draw2DGridInfo(QPainter &painter, IProj *pProj)
{
	QMutexLocker locker(&sMutex);
	if (pProj == nullptr || !isClickCompute)
	{
		return;
	}
	m_map3D.create2DGridDrawing(painter, pProj);
}
HeatMap3D LY_AgriSprayPresHomeWidget::getHeatMap3D() const
{
	return m_map3D;
}
void LY_AgriSprayPresHomeWidget::on_sure_btn_clicked()
{
	levpValueMap.clear();

	std::vector<int> tempSpinBoxValueVec = {
       ui->first_spin_box->text().toInt(), 
		ui->second_spin_box->text().toInt(), 
		ui->three_spin_box->text().toInt(),
		ui->four_spin_box->text().toInt(),
		ui->five_spin_box->text().toInt()
	};
	
	int index = 0;
	for (auto const &item : pestLevVec)
	{
		if (index >= tempSpinBoxValueVec.size())
		{
			continue;
		}
		psetLeveallDataMap[index] = std::make_pair(item, tempSpinBoxValueVec.at(index));
		index++;
	}
	

	isClickCompute = true;
	if (m_type == PrescretionConcolType::PrescretionConcolType_ADD)
	{
		AgriSprayPrescriptionModel model;
		model.guid = LY_SprayPrescriptionUntil::GenerateUuid();
		model.map_name = ui->spray_edit->text();
		model.work_type = 1;
		model.data_source_id = m_currentGuid;
		model.field_id = m_pFieldInfo->id;
		model.compute_type = 2;
		model.map_level = ui->map_combox->currentText().toInt();
		model.pyfk = ui->pyfk_box->currentText().toInt();
		model.pygd = 10;
		model.crop_type = ui->crop_type_box->currentIndex() + 1;
		model.pest_category = ui->pest_type_box->currentIndex() + 1;
		model.pest_disease = ui->pest_stage_box->currentIndex() + 1;
		for (auto const &item : pestLevVec) {
			model.leve_arr.push_back(item);
		}
		draw3DGridMap(model);
		for (auto const &PestLevel : tempMap)
		{
			model.leve_value.push_back(PestLevel.second.toDouble());
		}
		bool isSuc = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->AddSprayPrescription(model);

		if (isSuc)
		{
			LyMessageBox::information(this, tr2("警告"), tr2("添加成功"));
			m_listVec.push_back(model);
			setListData();
		}
		else
		{
			LyMessageBox::information(this, tr2("警告"), tr2("添加失败"));
		}


	}
	else if (m_type == PrescretionConcolType::PrescretionConcolType_EDIT)
	{

		if (-1 == m_selectedCheckBoxRow)
		{
			LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行"));
			return;
		};
		if (m_selectedCheckBoxRow >= m_listVec.size())
		{
			return;
		}
		AgriSprayPrescriptionModel model = m_listVec.at(m_selectedCheckBoxRow);
		model.leve_arr.clear();
		model.leve_value.clear();
		model.map_name = ui->spray_edit->text();
		model.work_type = 1;
		model.data_source_id = m_currentGuid;
		model.field_id = m_pFieldInfo->id;
		model.compute_type = 2;
		model.map_level = ui->map_combox->currentText().toInt();
		model.pyfk = ui->pyfk_box->currentText().toInt();
		model.pygd = 10;
		model.crop_type = ui->crop_type_box->currentIndex() + 1;
		model.pest_category = ui->pest_type_box->currentIndex() + 1;
		model.pest_disease = ui->pest_stage_box->currentIndex() + 1;
		draw3DGridMap(model);
		for (auto const &item : pestLevVec) {
			model.leve_arr.push_back(item);
		}
	
		for (auto const &PestLevel : tempMap)
		{
			model.leve_value.push_back(PestLevel.second.toDouble());
		}

		bool isSucc = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->UpdateSprayPrescription(model);
		if (isSucc)
		{
			for (auto it = m_listVec.begin(); it != m_listVec.end(); ++it) {
				if (model.guid == it->guid) {  
					*it = model;
					break;  
				}
			}
			setListData();
			LyMessageBox::information(this, tr2("警告"), tr2("修改成功"));
		}
		else
		{
			LyMessageBox::information(this, tr2("警告"), tr2("修改失败"));
		}
	}
	if (-1 == m_selectedCheckBoxRow)
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行"));
		return;
	};
	if (m_selectedCheckBoxRow >= m_listVec.size())
	{
		return;
	}
	AgriSprayPrescriptionModel sprayModel = m_listVec.at(m_selectedCheckBoxRow);
	draw3DGridMap(sprayModel);

	m_type = PrescretionConcolType::PrescretionConcolType_NONE;
}

bool LY_AgriSprayPresHomeWidget::containsAll(const std::vector<int>& vec1, const std::vector<int>& vec2) {
	for (int element : vec2) {
		if (std::find(vec1.begin(), vec1.end(), element) == vec1.end()) {
			return false;
		}
	}
	return true;
}
void LY_AgriSprayPresHomeWidget::draw3DGridInfo( pos3d *p3d)
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;

	auto itr2 = grid3DInfoVec.begin();
	for (; itr2 != grid3DInfoVec.end(); itr2++)
	{
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	grid3DInfoVec.clear();
	//qDebug() << "-----==============:" << p3d->latitude << p3d->longitude;

	for (auto it = m_map3D.mapInfoMap.begin(); it != m_map3D.mapInfoMap.end(); ++it) {

		osgEarth::Polygon* polygon = m_map3D.mapInfoMap[it->first];
		if (polygon == nullptr)
		{
			continue;
		}

		for (size_t i = 0; i < polygon->size(); ++i) {
			const osg::Vec3d& vertex = polygon->at(i);
			//qDebug() << "============---Vertex " << i << ": (" << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << ")";
		}


		if (polygon->contains2D(p3d->longitude, p3d->latitude))
		//if (polygon->contains2D(*polygon, *p3d))
		{
			if (pEarth->GetGraphics())
			{
				//qDebug() << "=============************====:" << it->first << it->second << levpValueMap;
			 //QString str = levpValueMap[it->first] + tr2("升/亩");
				QString str;
				for (auto const &PestLevel : levpValueMap)
				{
					if (std::find(PestLevel.first.begin(), PestLevel.first.end(), it->first) != PestLevel.first.end())
					{
						str = PestLevel.second + u8"升/亩";
					}
				}
				if (p3d->longitude > 0 && p3d->latitude > 0)
				{
					//IText* mshpText = pEarth->GetGraphics()->CreateText(tr3(str), p3d->longitude, p3d->latitude, 130, NULL, 22,
					//	TRGB(128, 255, 255, 255), TRGB(128, 128, 128, 128), TRGB(128, 128, 0, 128), 7);
					//grid3DInfoVec.push_back(mshpText);
				}

			}
		}
	}
}

osgEarth::Symbology::Color LY_AgriSprayPresHomeWidget::hexToColor(const std::string& hexColor, float rgb_a/* = 1.0f*/)
{
	{
		unsigned int r, g, b;
		if (sscanf(hexColor.c_str(), "#%02X%02X%02X", &r, &g, &b) == 3)
		{
			return osgEarth::Symbology::Color(r / 255.0f, g / 255.0f, b / 255.0f, rgb_a); // 透明度默认为 1.0
		}
		return osgEarth::Symbology::Color(); // 失败返回默认颜色
	}
}

void LY_AgriSprayPresHomeWidget::on_show_hide_clicked()
{
	isClickCompute = !isClickCompute;
	m_map3D.setImageShowOrHide_forDisPosition(isClickCompute);
}
void  LY_AgriSprayPresHomeWidget::setComboxStatus(bool isEnable)
{
	ui->farm_name_L->setEnabled(isEnable);
	ui->cropland_name_L->setEnabled(isEnable);
	ui->data_source_box->setEnabled(isEnable);
	ui->pes_map_box->setEnabled(isEnable);
	ui->crop_type_box->setEnabled(isEnable);
	ui->spray_edit->setEnabled(isEnable);
	ui->pest_type_box->setEnabled(isEnable);
	ui->pest_stage_box->setEnabled(isEnable);
	ui->pyfk_box->setEnabled(isEnable);
	ui->map_combox->setEnabled(isEnable);
	ui->slider->setEnabled(isEnable);
	ui->first_spin_box->setEnabled(isEnable);
	ui->second_spin_box->setEnabled(isEnable);
	ui->three_spin_box->setEnabled(isEnable);
	ui->four_spin_box->setEnabled(isEnable);
	ui->five_spin_box->setEnabled(isEnable);

}