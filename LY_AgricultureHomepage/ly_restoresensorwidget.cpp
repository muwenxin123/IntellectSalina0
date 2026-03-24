#include "ly_restoresensorwidget.h"
#include "ui_ly_restoresensorwidget.h"
#include "QCString.h"
#include <QVariant>
#include <QDebug>
#include "LyMessageBox.h"

LY_RestoreSensorWidget::LY_RestoreSensorWidget(QWidget *parent) :
	LyDialog(tr2("传感器编辑"), parent),
	ui(new Ui::LY_RestoreSensorWidget)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->setMinimumSize(400, 350);

	ui->left_list->setSelectionMode(QAbstractItemView::NoSelection); 
	ui->right_list->setSelectionMode(QAbstractItemView::NoSelection); 
	connect(ui->move_right_action, &QPushButton::clicked, this, &LY_RestoreSensorWidget::on_move_right_action);
	connect(ui->move_left_action, &QPushButton::clicked, this, &LY_RestoreSensorWidget::on_move_left_action);
	connect(ui->sure_action, &QPushButton::clicked, this, &LY_RestoreSensorWidget::on_sure_action);
	connect(ui->cancle_action, &QPushButton::clicked, this, &LY_RestoreSensorWidget::on_cancle_action);
	connect(ui->left_all, &QPushButton::clicked, this, &LY_RestoreSensorWidget::on_left_all);
	connect(ui->left_not_all, &QPushButton::clicked, this, &LY_RestoreSensorWidget::on_left_not_all);
	connect(ui->left_delete, &QPushButton::clicked, this, &LY_RestoreSensorWidget::on_left_delete);
	connect(ui->right_all, &QPushButton::clicked, this, &LY_RestoreSensorWidget::on_right_all);
	connect(ui->right_not_all, &QPushButton::clicked, this, &LY_RestoreSensorWidget::on_right_not_all);

}
void LY_RestoreSensorWidget::showEvent(QShowEvent *event)
{
	QString str = "unknow";
	std::map<QString, SensorListModelVec> tempMap = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorMap();

	auto sensorList = tempMap[str];
	for (auto &model : sensorList) {
		QListWidgetItem *item = new QListWidgetItem(model.mointName);
		item->setSizeHint(QSize(item->sizeHint().width(), 30));
		//item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
		item->setCheckState(Qt::Unchecked);
		ui->left_list->addItem(item);
		//ui->left_list->insertItem(ui->left_list->count(), item);
		item->setData(Qt::UserRole, QVariant::fromValue(model));
	}
}
void LY_RestoreSensorWidget::on_left_all()
{
		for (int i = 0; i < ui->left_list->count(); ++i) {
			QListWidgetItem *item = ui->left_list->item(i);
			item->setCheckState(Qt::Checked);
		}
}
void LY_RestoreSensorWidget::on_left_not_all()
{
	for (int i = 0; i < ui->left_list->count(); ++i) {
		QListWidgetItem *item = ui->left_list->item(i);
		item->setCheckState(Qt::Unchecked);
	}
}
void LY_RestoreSensorWidget::on_left_delete()
{

	QString strTmp = tr2("请确认是否删除");
	if (QMessageBox::StandardButton::Yes == LyMessageBox::question(this, tr2("提示"), strTmp, QMessageBox::Yes | QMessageBox::Cancel))
	{
		
		std::vector<SensorListModel> selectedRightDataList = getSelectedLeftItemsData(ui->right_list);
		bool isSuc = false;
		QString mointID = "";
		for (SensorListModel& data : selectedRightDataList)
		{
			emit deleteSensor(data.mointID, data.mointName);
			isSuc = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->DeleteSensor(data);
			if (!isSuc)
			{
				mointID = data.mointID;
			}

		}
		if (isSuc)
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("删除成功"));
			
			ui->left_list->clear();
		}
		else
		{
			QString message = tr2("删除失败的传感器ID:") + mointID;
			LyMessageBox::information(NULL, tr2("提示"), message);
		}
	}
	this->close();
}
void LY_RestoreSensorWidget::on_right_all()
{
	for (int i = 0; i < ui->right_list->count(); ++i) {
		QListWidgetItem *item = ui->right_list->item(i);
		item->setCheckState(Qt::Checked);
	}
}
void LY_RestoreSensorWidget::on_right_not_all()
{
	for (int i = 0; i < ui->right_list->count(); ++i) {
		QListWidgetItem *item = ui->right_list->item(i);
		item->setCheckState(Qt::Unchecked);
	}
}


void LY_RestoreSensorWidget::moveItems(QListWidget *src, QListWidget *dst) {
	// 按顺序处理勾选项
	for (int i = src->count() - 1; i >= 0; --i) { // 反向遍历避免索引错位
		QListWidgetItem *item = src->item(i);
		if (item->checkState() == Qt::Checked) {
			QListWidgetItem *newItem = new QListWidgetItem(*item);
			newItem->setCheckState(Qt::Unchecked); // 重置勾选状态

			dst->addItem(newItem);
			delete item; // 删除原项
		}
	}
}
void LY_RestoreSensorWidget::on_move_right_action()
{
	moveItems(ui->left_list, ui->right_list);
}
void LY_RestoreSensorWidget::on_move_left_action()
{
	moveItems(ui->right_list, ui->left_list);
}


void LY_RestoreSensorWidget::on_sure_action()
{
	QString str = "unknow";
	//SensorListModelVec vector = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorList(str);
	std::map<QString, SensorListModelVec> tempMap = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorListMap();

	SensorListModelVec vector = tempMap[str];

	std::vector<SensorListModel> selectedRightDataList = getSelectedLeftItemsData(ui->right_list);
	SensorListModelVec leftVec;
	SensorListModelVec rightVec;
	if (selectedRightDataList.size() > 0)
	{
		for (SensorListModel& data : selectedRightDataList)
		{
			for (SensorListModel& model : vector)
			{
				if (data.mointID == model.mointID)
				{
					data.sensor_type_id = model.origin_sensor_type_id;
					data.latitude = model.origin_latitude;
					data.longitude = model.origin_longitude;
					LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->UpdateSensor(data);
					rightVec.push_back(data);
				}
			}
		}
	}

	std::vector<SensorListModel> selectedLeftDataList = getSelectedLeftItemsData(ui->left_list);
	if (selectedLeftDataList.size() > 0)
	{
		for (SensorListModel& data : selectedLeftDataList)
		{
			LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->UpdateSensor(data);
			leftVec.push_back(data);
		}
	}
	this->close();
	emit getSensorList(leftVec, rightVec);

}
void LY_RestoreSensorWidget::on_cancle_action()
{
	this->close();
}
std::vector<SensorListModel> LY_RestoreSensorWidget::getSelectedLeftItemsData(QListWidget *listWidget) const
{
	std::vector<SensorListModel> selectedDataList;

	// 遍历所有项
	for (int i = 0; i < listWidget->count(); ++i) {
		QListWidgetItem *item = listWidget->item(i);
		// 检查复选框是否被选中
		if (item->checkState() == Qt::Checked) {
			// 提取绑定的 struct 数据
			SensorListModel data = item->data(Qt::UserRole).value<SensorListModel>();
			selectedDataList.push_back(data);
		}
	}

	return selectedDataList;
}
LY_RestoreSensorWidget::~LY_RestoreSensorWidget()
{
    delete ui;
}
