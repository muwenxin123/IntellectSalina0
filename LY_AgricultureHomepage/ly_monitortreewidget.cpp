#include "ly_monitortreewidget.h"
#include "LY_AgricultureHomepage.h"
#include <qstring.h>
#include "QCString.h"
#include <QHBoxLayout>
#include <QMenu>
#include "ly_mointeraddwidget.h"
#include "LyMessageBox.h"
#include <QSettings>
#include "ly_addmointtypewidget.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QObject>

LY_MonitorTreeWidget::LY_MonitorTreeWidget(QWidget *parent)
  : QWidget(parent)

{

	m_viewModel = LY_AgricultureHomepage::Get().GetViewModel();


   initWidget();
   //addData();
   this->setFixedWidth(350);

 
}
void LY_MonitorTreeWidget::on_time_out()
{

}
LY_MonitorTreeWidget::~LY_MonitorTreeWidget()
{
	qDebug() << "delete";
}
void LY_MonitorTreeWidget::showEvent(QShowEvent *event)
{
	updateData();
}
void LY_MonitorTreeWidget::updateData()
{
	addData();
}

bool LY_MonitorTreeWidget::DrawDynamic(QPainter &painter, IProj *pProj)
{
	for (auto& sensor : m_sensor_list)
	{
		QPoint point = pProj->Jwd2Screen(sensor.longitude, sensor.latitude);

		QString imgStr = "../data/Temp_Line/";
		switch (static_cast<SensorType>(sensor.typeID))
		{
		case SensorType::SensorTypeType_Video:
		{
			imgStr += "surveillance_camera";
		}
		break;

		case SensorType::SensorTypeType_Soil:
		{
			imgStr += "soil_moisture";
		}
		break;

		case SensorType::SensorTypeType_Weather:
		{
			imgStr += "weather_station";
		}
		break;

		case SensorType::SensorTypeType_PestMonitor:
		{
			imgStr += "pest_detection";
		}
		break;

		case SensorType::SensorTypeType_KillInsects:
		{
			imgStr += "drive_pest";
		}
		break;

		case SensorType::SensorTypeType_Valve:
		{
			imgStr += "irrigation_valve";
		}
		break;

		case SensorType::SensorTypeType_FarmCar:
		{
			imgStr += "entry_point";
		}
		break;
		case SensorType::SensorTypeType_FlyStop:
		{
			imgStr += "dji_airport";
		}
		break;

		default:
			break;
		}

		imgStr += "_2D.png";

		QPixmap pixmap;
		if (pixmap.load(imgStr))
		{
			painter.drawPixmap(point.x() - 15, point.y() - 15, 30, 30, pixmap);
		}
	}

	return true;
}

void LY_MonitorTreeWidget::initWidget()
{
 
	m_treeWidget = new QTreeWidget(this);
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(m_treeWidget);

	connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &LY_MonitorTreeWidget::showContextMenu);
	m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(m_treeWidget, &QTreeWidget::itemClicked, this, &LY_MonitorTreeWidget::itemClicked);
	connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &LY_MonitorTreeWidget::itemDoubleClicked);
	m_treeWidget->setColumnCount(1);
	m_treeWidget->setHeaderLabels(QStringList() << tr2("盐田列表"));

}

void LY_MonitorTreeWidget::addData()
{
	m_treeWidget->clear();

	m_sensor_list.clear();

	// 查询当前激活农场
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Agri");
	QString m_sCurActiveFarm = ini.value("FarmID", "").toString();
	
	auto farmList = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetFarmInfoList();
	for (auto& farmInfo : farmList)
	{
		// 匹配激活农场
		if (m_sCurActiveFarm == farmInfo.id)
		{
			m_currentFarm = m_sCurActiveFarm;
			break;
		}
	}
	m_sensor_group.clear();
	AgriFieldInfoModelVec tempVec = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetFieldInfoList(m_currentFarm);

	m_sensor_group = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorGroupList(m_currentFarm);

	for (auto &model : tempVec)
	{
		/*if (m_sensor_group.size() == 0)
		{*/
			AgriSensorGroupModel groupModel;
			groupModel.id = model.id;
			groupModel.farm_id = m_currentFarm;
			groupModel.name = model.name;
			LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->AddSensorGroup(groupModel);
			m_sensor_group.push_back(groupModel);
			/*break;
		}
		for (auto &group : m_sensor_group)
		{
			if (model.id == group.id || group.id == "")
			{
				continue;
			}
			else
			{
				AgriSensorGroupModel groupModel;
				groupModel.id = model.id;
				groupModel.farm_id = m_currentFarm;
				groupModel.name = model.name;
				 LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->AddSensorGroup(groupModel);
				 m_sensor_group.push_back(groupModel);
			}
		}*/
	}

	AgriSensorGroupModel groupModel;
	groupModel.id = "unknow";
	groupModel.farm_id = m_currentFarm;
	groupModel.name = tr2("未知分组");
	LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->AddSensorGroup(groupModel);
	m_sensor_group.push_back(groupModel);
	
	
	m_sensor_group = filterUniqueById(m_sensor_group);

	//AgriAgriSensorGroupModelVec group = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorGroupList(m_currentFarm);
	//for (auto &model : group) {
	//	m_sensor_group.push_back(model);
	//}


	for (auto &model : m_sensor_group)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
		item->setText(0, model.name);
		item->setData(0, Qt::UserRole, QVariant::fromValue(model));
		m_treeWidget->addTopLevelItem(item);

		auto mointTypeList = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorTypeList(model.id);

		if (model.name == tr2("未知分组"))
		{
			auto sensorList = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorList(model.id);
			if (sensorList.size() == 0)
			{
				continue;
			}
			for (auto &sensor : sensorList)
			{
				QTreeWidgetItem *subItem = new QTreeWidgetItem(item);
				subItem->setText(0, sensor.mointName);
				subItem->setData(0, Qt::UserRole + 2, QVariant::fromValue(sensor));
				item->addChild(subItem);
			}
		}
		else
		{
			if (mointTypeList.size() == 0)
			{
				continue;
			}
			for (auto &sensorType : mointTypeList)
			{
				QTreeWidgetItem *sensorItem = new QTreeWidgetItem(item);
				sensorItem->setText(0, sensorType.name);
				sensorItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(sensorType));
				item->addChild(sensorItem);

				auto sensorList = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorList(sensorType.id);
				if (sensorList.size() == 0)
				{
					continue;
				}
				for (auto &sensor : sensorList)
				{
					QTreeWidgetItem *subItem = new QTreeWidgetItem(sensorItem);
					subItem->setText(0, sensor.mointName);
					subItem->setData(0, Qt::UserRole + 2, QVariant::fromValue(sensor));
					sensorItem->addChild(subItem);

					m_sensor_list.push_back(sensor);
				}
			}
		}
		
	}

	AgriFieldInfoModel tempModel;
	m_treeWidget->expandAll();

}
void LY_MonitorTreeWidget::itemClicked(QTreeWidgetItem *item, int column) {

	//runBatFile("./PlayVideo.bat");
	if (item == nullptr) return;
	QMenu menu(this);
	
	const int level = getNodeLevel(item);
	switch (level)
	{
	case 1:
	{
		int index = m_treeWidget->indexOfTopLevelItem(item);
		AgriSensorGroupModel model = item->data(0, Qt::UserRole).value<AgriSensorGroupModel>();
		qDebug() << "=====-----**--:" << model.name << model.farm_id << model.id;

		
	}

		break;
	case 2:break;
	case 3:
	{

		int index = m_treeWidget->indexOfTopLevelItem(item);
		//qDebug() << "Index:" << index; 
		SensorListModel model = item->data(0, Qt::UserRole + 2).value<SensorListModel>();
		//int index = item->curr
		emit clickTreeItem(model, column);
		
	}
		break;
	default:
		break;
	}
	

}

void LY_MonitorTreeWidget::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item == nullptr) return;
	QMenu menu(this);

	const int level = getNodeLevel(item);
	switch (level)
	{
	case 1:break;
	case 2:break;
	case 3:
	{

		int index = m_treeWidget->indexOfTopLevelItem(item);
		//qDebug() << "Index:" << index; 
		SensorListModel model = item->data(0, Qt::UserRole + 2).value<SensorListModel>();
		//int index = item->curr
		emit doubleClickTreeItem(model, column);

	}
	break;
	default:
		break;
	}
}

void LY_MonitorTreeWidget::showContextMenu(const QPoint &pos) {

	QTreeWidget* curTree = qobject_cast<QTreeWidget*>(sender());
	if (curTree == nullptr) return;
	QTreeWidgetItem* item = curTree->itemAt(pos);
	
	if (item == nullptr) return;
	QMenu menu(this);
	
	const int level = getNodeLevel(item);
	QAction* addAction = nullptr;
	QAction* delAction = nullptr;
	QAction* updateAction = nullptr;
	QAction* restoreAction = nullptr;

	
	switch (level)
	{
	case 1:
	{
		AgriSensorGroupModel fieldInfo = item->data(0, Qt::UserRole).value<AgriSensorGroupModel>();
		if (fieldInfo.name == tr2("未知分组"))
		{
			break;
		}
		addAction = menu.addAction(tr2("添加传感器类型"));
		connect(addAction, &QAction::triggered, [=]() { 
			addSensorType(item);
		});
	}
		break;
	case 2:
	{
		QTreeWidgetItem* item1 = item->parent();
		if (item1 != nullptr)
		{
			AgriSensorGroupModel fieldInfo = item1->data(0, Qt::UserRole).value<AgriSensorGroupModel>();
			m_current_fieldInfo = fieldInfo;
			if (fieldInfo.name == tr2("未知分组"))
			{
				restoreAction = menu.addAction(tr2("编辑"));
				connect(restoreAction, &QAction::triggered, [=]() {
					
					LY_RestoreSensorWidget *restorWidget = new LY_RestoreSensorWidget(this);
					//restorWidget->setItemVec(deleteSensorList);
					restorWidget->show();
					connect(restorWidget, &LY_RestoreSensorWidget::getSensorList, this, &LY_MonitorTreeWidget::updateSensorGroup);
					connect(restorWidget, &LY_RestoreSensorWidget::deleteSensor, this, &LY_MonitorTreeWidget::deleteSensorRestor);
				});
				break;
			}
		}
	
		addAction = menu.addAction(tr2("添加传感器"));
		delAction = menu.addAction(tr2("删除传感器类型"));
		connect(addAction, &QAction::triggered, [=]() { 
			addSensor(item);
		});
		connect(delAction, &QAction::triggered, [=]() {
			deleteSensorType(item);
		});
	}
		break;
	case 3:
	{
		delAction = menu.addAction(tr2("删除传感器"));
		updateAction = menu.addAction(tr2("修改传感器"));
		connect(delAction, &QAction::triggered, [=]() { 
			deleteSensor(item);
		});

		connect(updateAction, &QAction::triggered, [=]() {
			updateSensor(item);
		});
	}
		break;
	}
	menu.exec(m_treeWidget->viewport()->mapToGlobal(pos));
	m_treeWidget->expandAll();
}
void LY_MonitorTreeWidget::deleteSensorRestor(QString mointID, QString mointName)
{
	QList<QTreeWidgetItem*> unknownGroups = m_treeWidget->findItems(tr2("未知分组"), Qt::MatchExactly | Qt::MatchRecursive);
	foreach(QTreeWidgetItem* groupItem, unknownGroups) {
		// 2. 遍历子项，匹配名称
		for (int i = 0; i < groupItem->childCount(); ++i) {
			QTreeWidgetItem* child = groupItem->child(i);
			SensorListModel model = child->data(0, Qt::UserRole + 2).value<SensorListModel>();
			if (child->text(0) == mointName) { // 匹配目标子项

				groupItem->removeChild(child);
				delete child;
			}
		}
	}

}
void LY_MonitorTreeWidget::updateSensorGroup(SensorListModelVec leftVec, SensorListModelVec rightVec)
{
	QString field_id = m_current_fieldInfo.id;
	SensorTypeModelVec typeModelvec = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorTypeList(field_id);
	//std::set<SensorListModel> set1(leftVec.begin(), leftVec.end());
	//std::set<SensorTypeModel> set2(typeModelvec.begin(), typeModelvec.end());
	if (rightVec.size() > 0)
	{
		SensorTypeModelVec tempVec;
		for (auto &listModel : rightVec) {
			bool isFound = false;
			for (auto &typeModel : typeModelvec) {
				if (listModel.field_id == typeModel.field_id && listModel.sensor_type_id == typeModel.id)
				{
					isFound = true;
					break;
				}
			}
			if (!isFound)
			{
				SensorTypeModel typeM;
				typeM.field_id = listModel.field_id;
				typeM.id = listModel.sensor_type_id;
				typeM.name = listModel.mointType;
				typeM.type = listModel.typeID;
				tempVec.push_back(typeM);

				LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->AddSensorType(typeM);
			}
		}
	}
	//TODO:  优化
	addData();

}
void LY_MonitorTreeWidget::addSensorType(QTreeWidgetItem* parent)
{
	Ly_AddMointTypeWidget *w = new Ly_AddMointTypeWidget(this);
	w->show();

	connect(w, &Ly_AddMointTypeWidget::getMointName, [=](SensorType type) {
		SensorTypeModel model;
		model.name = GlobalUtils::GetSensorStringWithType(type);
		model.id = GenerateUuid();
		AgriSensorGroupModel fieldInfo = parent->data(0, Qt::UserRole).value<AgriSensorGroupModel>();
		model.field_id = fieldInfo.id;
		model.type = static_cast<int>(type);
		bool isSuc = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->AddSensorType(model);
		if (isSuc)
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("传感器类型添加成功"));
			QTreeWidgetItem *item = new QTreeWidgetItem(parent);
			item->setText(0, model.name);
			item->setData(0, Qt::UserRole + 1, QVariant::fromValue(model));
			parent->addChild(item);
		}
		else
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("传感器类型失败"));
		}
		
	});
}
void LY_MonitorTreeWidget::addSensor(QTreeWidgetItem* parent)
{
	LY_MointerAddWidget *addWidget = new LY_MointerAddWidget();
	addWidget->setType(1);
	addWidget->show();

	SensorTypeModel sensor_type = parent->data(0, Qt::UserRole + 1).value<SensorTypeModel>();

	addWidget->setSensorType(static_cast<SensorType>(sensor_type.type));
	connect(addWidget, &LY_MointerAddWidget::addSensor, this, [=](SensorListModel &model) {
		
		model.sensor_type_id = sensor_type.id;

		bool isS = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->CheckSampleSensor(model);
		if (!isS)
		{
			model.field_id = sensor_type.field_id;
			bool isSuc = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->AddSensor(model);
			if (isSuc)
			{
				QString field = sensor_type.field_id;
				LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->AddSensorWithField(field, model.mointID);
				QTreeWidgetItem *item = new QTreeWidgetItem(parent);
				item->setText(0, model.mointName);
				item->setData(0, Qt::UserRole + 2, QVariant::fromValue(model));
				parent->addChild(item);
				LyMessageBox::information(NULL, tr2("提示"), tr2("添加成功"));
			}
			else
			{
				LyMessageBox::information(NULL, tr2("提示"), tr2("添加失败"));
			}
		}
		else
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("添加失败,传感器名字或者ID已经存在"));
		}
	});
}
void LY_MonitorTreeWidget::deleteSensorType(QTreeWidgetItem* item)
{

	QString strTmp = tr2("请确认是否删除");
	if (QMessageBox::StandardButton::Yes == LyMessageBox::question(this, tr2("提示"), strTmp, QMessageBox::Yes | QMessageBox::Cancel))
	{
		SensorTypeModel model = item->data(0, Qt::UserRole + 1).value<SensorTypeModel>();
		bool isSuc = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->DeleteSensorType(model);
		if (isSuc)
		{
			QString field = model.field_id;
		
			QString sensor_type_id = "unknow";
			QTreeWidgetItem* unknownGroup = nullptr;
			for (int i = 0; i< m_treeWidget->topLevelItemCount(); ++i) {
				QTreeWidgetItem* item = m_treeWidget->topLevelItem(i);
				if (item->text(0) == tr2("未知分组") && item->data(0, Qt::UserRole + 1).isNull()) {
		            unknownGroup = item;
					break;
				}
			}

			SensorListModelVec sensorList =  LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorList(model.id);
			for (auto sensor: sensorList) {

				sensor.origin_sensor_type_id = sensor.sensor_type_id;
				sensor.origin_longitude = sensor.longitude;
				sensor.origin_latitude = sensor.latitude;
				sensor.sensor_type_id = sensor_type_id;
				sensor.latitude = 0.00;
				sensor.longitude = 0.00;
				//deleteSensorList.push_back(sensor);
				LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->UpdateSensor(sensor);

				LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->DeleteSensorWithField(sensor.mointID);

			
				if (unknownGroup != nullptr)
				{
					QTreeWidgetItem *item = new QTreeWidgetItem(unknownGroup);
					item->setText(0, sensor.mointName);
					item->setData(0, Qt::UserRole + 3, QVariant::fromValue(model));
					unknownGroup->addChild(item);
				}
			}

			LyMessageBox::information(NULL, tr2("提示"), tr2("删除成功"));
			delete item;
			item = nullptr;
		}
		else
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("删除失败"));
		}
	}

}
void LY_MonitorTreeWidget::deleteSensor(QTreeWidgetItem* item)
{

	QString strTmp = tr2("请确认是否删除");
	if (QMessageBox::StandardButton::Yes == LyMessageBox::question(this, tr2("提示"), strTmp, QMessageBox::Yes | QMessageBox::Cancel))
	{
		SensorListModel model = item->data(0, Qt::UserRole + 2).value<SensorListModel>();
		/*bool isSuc = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->DeleteSensor(model);
		if (isSuc)
		{*/
			
			model.origin_longitude = model.longitude;
			model.origin_latitude = model.latitude;
			model.origin_sensor_type_id = model.sensor_type_id;
			model.sensor_type_id = "unknow";
			model.latitude = 0.00;
			model.longitude = 0.00;
			//deleteSensorList.push_back(model);
			LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->UpdateSensor(model);
			LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->DeleteSensorWithField(model.mointID);
			model.origin_sensor_type_id = model.sensor_type_id;
			QTreeWidgetItem* unknownGroup = nullptr;
			for (int i = 0; i< m_treeWidget->topLevelItemCount(); ++i) {
				QTreeWidgetItem* item = m_treeWidget->topLevelItem(i);
				if (item->text(0) == tr2("未知分组") && item->data(0, Qt::UserRole + 1).isNull()) {
					unknownGroup = item;
					break;
				}
			}
			if (unknownGroup != nullptr)
			{
				QTreeWidgetItem *item = new QTreeWidgetItem(unknownGroup);
				item->setText(0, model.mointName);
				item->setData(0, Qt::UserRole + 3, QVariant::fromValue(model));
				unknownGroup->addChild(item);
			}
			LyMessageBox::information(NULL, tr2("提示"), tr2("删除成功, 添加到未知分组"));
			delete item;
			item = nullptr;
		/*}
		else
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("删除失败"));
		}*/
	}
}
void LY_MonitorTreeWidget::updateSensor(QTreeWidgetItem* item)
{
	LY_MointerAddWidget *addWidget = new LY_MointerAddWidget();
	addWidget->setType(2);
	addWidget->show();

	SensorListModel model = item->data(0, Qt::UserRole + 2).value<SensorListModel>();

	addWidget->setSensorType(static_cast<SensorType>(model.typeID));
	addWidget->setData(model);
	connect(addWidget, &LY_MointerAddWidget::updateSensor, this, [=](SensorListModel &model) {

		bool isSuc = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->UpdateSensor(model);
		if (isSuc)
		{

			item->setText(0, model.mointName);
			item->setData(0, Qt::UserRole + 2, QVariant::fromValue(model));
			LyMessageBox::information(NULL, tr2("提示"), tr2("修改成功"));
		}
		else
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("修改失败"));
		}
	
	});
}

int LY_MonitorTreeWidget::getNodeLevel(QTreeWidgetItem* item) const {
	if (!item->parent()) return 1;
	if (!item->parent()->parent()) return 2;
	return 3;
}

void LY_MonitorTreeWidget::runBatFile(const QString& batFilePath)
{
	QProcess *process = new QProcess();


	//// 连接信号以处理结果
	//QObject::connect(process, &QProcess::finished, [=](int exitCode, QProcess::ExitStatus status) {
	//	qDebug() << "BAT finished with code:" << exitCode;
	//	if (exitCode == 0) {
	//		qDebug() << "Success! Output:" << process->readAllStandardOutput();
	//	}
	//	else {
	//		qDebug() << "Error:" << process->readAllStandardError();
	//	}
	//	process->deleteLater(); // 清理内存
	//});

	// 启动进程
	process->start(batFilePath);
}

