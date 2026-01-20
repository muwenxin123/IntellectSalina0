#include "EventRecord.h"
#include "ui_EventRecord.h"
#include <QDate>
#include <QTime>
#include <QTableWidgetItem>
#include "QCString.h"
#include "ModelProcessor.h"
#include "QTimer"
EventRecord::EventRecord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventRecord)
{
    ui->setupUi(this);
	initTable();//初始化表格

	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &EventRecord::onDetectionDataReceived);
	m_timer->start(1000);
}

EventRecord::~EventRecord()
{
    delete ui;
}

void EventRecord::initTable() {
	QStringList headerlabels;
	headerlabels << tr2("时间") << tr2("事件信息") ;

	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	//设置列数
	//ui->tableWidget->setColumnCount(2);
	ui->tableWidget->setRowCount(0);

	//设置表格属性
	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

	//ModelProcessor *mod = new ModelProcessor(this);
	//connect(mod, &ModelProcessor::dataready, this, &AddtableWidget);
	//
}

//void EventRecord::addTableRow(const QDate &date, const QTime &time, const QString &status, const QString &category, const QString &location) {
//	//获取当前行数
//	int row = ui->tableWidget->rowCount();
//	ui->tableWidget->insertRow(row);
//
//	//创建表格并添加到表格
//	ui->tableWidget->setItem(row, 0, new QTableWidgetItem(date.toString("yyyy-MM-dd")));
//	ui->tableWidget->setItem(row, 1, new QTableWidgetItem(time.toString("hh:mm:ss")));
//	ui->tableWidget->setItem(row, 2, new QTableWidgetItem(status));
//	ui->tableWidget->setItem(row, 3, new QTableWidgetItem(category));
//	ui->tableWidget->setItem(row, 4, new QTableWidgetItem(location));
//}
void EventRecord::AddtableWidget(std::vector<bbox_t> a, const VideoInfoData videoInfo) {
	for (auto iter = a.begin();
		iter != a.end(); ++iter) {
		std::stringstream ss;
		ss << std::setiosflags(std::ios::fixed) << std::setprecision(2) << iter->prob;
		std::string label = "";
		ModelType model_type = getModelTypeWithValue(videoInfo.model_type);
		switch (model_type)
		{
		case ModelType_FIRE:
		{
			label = name_map_fire[iter->obj_id] + ":" + ss.str();
		}
		break;
		//case ModelType_FLOWER:
		//{
		//	label = name_rape_flower[iter->obj_id] + ":" + ss.str();
		//}
		//break;
		//case ModelType_PEST:
		//{
		//	label = name_map_pest[iter->obj_id] + ":" + ss.str();
		//}
		//break;
		//case ModelType_NORMAL:
		//	break;
		case ModelType_fire:
		{
			label = name_map_fire[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_front01:
		{
			label = name_map_front01[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_Bridge:
		{
			label = name_map_Bridge[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_CityManagement:
		{
			label = name_map_CityManagement[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_ForestFireSmokeDetection:
		{
			label = name_map_ForestFireSmokeDetection[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_InsPLAD:
		{
			label = name_map_InsPLAD[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_RoadDisaster:
		{
			label = name_map_RoadDisaster[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_NORMAL:
			break;
		default:
			break;
		}


		int row = ui->tableWidget->rowCount(); //获取当前行数
		ui->tableWidget->insertRow(row); //插入新行

		//创建表格并添加到表格
		ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd")));
		ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QDateTime::currentDateTime().toString("hh:mm:ss")));
		//ui->tableWidget->setItem(row, 2, new QTableWidgetItem(status));
		//ui->tableWidget->setItem(row, 3, new QTableWidgetItem(category));
		//ui->tableWidget->setItem(row, 4, new QTableWidgetItem(location));
		ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(label)));

		ui->tableWidget->setItem(row, 3, new QTableWidgetItem("k401"));
		ui->tableWidget->setItem(row, 4, new QTableWidgetItem("k401"));

	}
}

//void EventRecord::setEventData(WS::EventData data)
//{
//	eventdatavec.push_back(data);
//	int i = 0;
//	int row = ui->tableWidget->rowCount(); //获取当前行数
//	ui->tableWidget->insertRow(row); //插入新行
//	//ui->tableWidget->setItem(row, 0, new QTableWidgetItem(eventdatavec[i].time.toString()));
//	//ui->tableWidget->setItem(row, 1, new QTableWidgetItem(eventdatavec[i].eventcontent));
//	i++;
//	update();
//}

void EventRecord::setEventData(const WS::EventData& data)
{
	//QReadLocker lk(&m_readLock);
	QWriteLocker lk(&m_readLock);
	// 添加到事件向量（在开头插入，保持最新数据在最前面）
	eventdatavec.push_back(data);
	if (eventdatavec.size() > 100)
	{
		eventdatavec.erase(eventdatavec.begin());
	}
	//onDetectionDataReceived();
}

void EventRecord::AddtableWidget2(std::vector<bbox_t> a,const char* str) {
	for (const auto& data : a) {
		ui->tableWidget->setItem(1, 2, new QTableWidgetItem(str));
	}
}

struct  DetectionData 
{
	//来自YOLO的bbox_t核心字段
	float x, y, w, h; //边界框坐标
	float prob; //置信度
	int classId; //对象类别ID(可映射到字符串，如“person”)

	//额外参数
	QString date; //日期字符串，如“2025-09-28”
	QDateTime timestamp; //检测时间（当前系统时间）

	QString address; //视频地址或元数据（如从videoInfo获取的文件路径）
	QString otherInfo; //其他自定义
	DetectionData(const bbox_t& bbox, const QString& vidAddress, int frameNum = 0) {
		x = bbox.x;
		y = bbox.y;
		w = bbox.w;
		h = bbox.h;
		prob = bbox.prob;
		classId = bbox.obj_id;

		timestamp = QDateTime::currentDateTime();
		date = timestamp.toString("yyyy-MM-dd");
		address = vidAddress; //从videoInfo或input获取
		otherInfo = QString("Frame: %1").arg(frameNum);
	}
};

//如果批量检测，使用QList或std::vector
using DetectionList = std::vector<DetectionData>; //或QList<DetectionData>


void EventRecord::setfeedback(const WS::DetectionData& data) {
	return ;
}

QString EventRecord::generateEventContent(const WS::EventData& eventData)
{
	// 直接使用 EventData 中已经生成的事件内容
	return eventData.eventcontent;
}

// 或者如果你需要更详细的信息，可以这样写：
QString EventRecord::generateDetailedEventContent(const WS::EventData& eventData)
{
	/*const auto& summary = eventData.detectionSummary;
	QStringList details;

	details.append(QString("检测时间: %1").arg(eventData.time.toString("hh:mm:ss")));

	if (summary.bboxCount > 0) {
		details.append(QString("发现边界框: %1个").arg(summary.bboxCount));
	}
	if (summary.polygonCount > 0) {
		details.append(QString("发现多边形区域: %1个").arg(summary.polygonCount));
	}
	if (summary.lineCount > 0) {
		details.append(QString("发现线条: %1条").arg(summary.lineCount));
	}
	if (summary.circleCount > 0) {
		details.append(QString("发现圆形: %1个").arg(summary.circleCount));
	}
	if (summary.maskCount > 0) {
		details.append(QString("发现遮罩: %1个").arg(summary.maskCount));
	}

	if (!summary.labels.isEmpty()) {
		details.append(QString("检测目标: %1").arg(summary.labels.join(", ")));
	}

	return details.join(" | ");*/
	return "";
}

void EventRecord::addDetectionEvent(const WS::DetectionData& detectionData)
{
	//// 检查是否有检测目标
	//if (!hasDetectionTargets(detectionData)) {
	//	return; // 没有检测目标，不记录事件
	//}

	//// 使用 DetectionData 创建 EventData
	//WS::EventData eventData(detectionData);

	//// 添加到事件向量
	//eventdatavec.push_back(eventData);

	//// 在表格中显示
	//int row = 0; // 总是在第一行插入
	//ui->tableWidget->insertRow(row);

	//// 设置时间列
	//ui->tableWidget->setItem(row, 0, new QTableWidgetItem(
	//	QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

	//// 设置事件信息列 - 使用 EventData 中的事件内容
	//ui->tableWidget->setItem(row, 1, new QTableWidgetItem(eventData.eventcontent));

	//// 可选：限制表格最大行数
	//const int MAX_ROWS = 1000;
	//while (ui->tableWidget->rowCount() > MAX_ROWS) {
	//	ui->tableWidget->removeRow(ui->tableWidget->rowCount() - 1);
	//}

	//// 自动滚动到最新行
	//ui->tableWidget->scrollToTop();
}

//void EventRecord::setEventData(const WS::EventData& data)
//{
//	eventdatavec.push_back(data);
//
//	// 在表格第一行插入新记录
//	int row = 0;
//	ui->tableWidget->insertRow(row);
//	ui->tableWidget->setItem(row, 0, new QTableWidgetItem(
//		QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
//	ui->tableWidget->setItem(row, 1, new QTableWidgetItem(data.eventcontent));
//
//	// 自动滚动到最新行
//	ui->tableWidget->scrollToTop();
//}

bool EventRecord::hasDetectionTargets(const WS::EventData& data)
{
	//const auto& summary = data.detectionSummary;

	//// 检查是否有任何检测目标
	//return summary.bboxCount > 0 ||
	//	summary.polygonCount > 0 ||
	//	summary.lineCount > 0 ||
	//	summary.circleCount > 0 ||
	//	summary.maskCount > 0;
	return false;
}

void EventRecord::onDetectionDataReceived()
{
	//// 检查是否有检测目标
	//if (!hasDetectionTargets(data)) {
	//	return;
	//}
	QReadLocker lk(&m_readLock);
	std::vector<WS::EventData> tempVec = eventdatavec;
	lk.unlock();
	ui->tableWidget->clearContents();
	int nR = ui->tableWidget->rowCount();
	int row = 0;
	for (auto mitor = tempVec.rbegin(); mitor != tempVec.rend();mitor++)
	{
		if (row >= nR)
		{
			ui->tableWidget->insertRow(row); //插入新行
		}
		// 设置时间列
		ui->tableWidget->setItem(row, 0, new QTableWidgetItem(mitor->time.toString("hh::mm::ss")));

		// 设置事件信息列 - 使用 EventData 中的事件内容
		ui->tableWidget->setItem(row, 1, new QTableWidgetItem(mitor->eventcontent));
		row++;
	}

	// 自动滚动到最新行（第一行）
	//ui->tableWidget->scrollToTop();

	// 可选：高亮显示最新行
	//ui->tableWidget->selectRow(0);
}