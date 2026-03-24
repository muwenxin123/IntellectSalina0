#include "EventRecord.h"
#include "ui_EventRecord.h"
#include <QDate>
#include <QTime>
#include <QTableWidgetItem>
#include "QCString.h"
#include "ModelProcessor.h"
EventRecord::EventRecord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventRecord)
{
    ui->setupUi(this);
	initTable();//初始化表格
}

EventRecord::~EventRecord()
{
    delete ui;
}

void EventRecord::initTable() {
	QStringList headerlabels;
	headerlabels << tr2("日期") << tr2("时间") << tr2("目标类别") << tr2("型号") << tr2("位置");

	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	//设置列数
	ui->tableWidget->setColumnCount(5);
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