#ifndef LY_MONITORTREEWIDGET_H
#define LY_MONITORTREEWIDGET_H
#include "ly_mointorviewmodel.h"
#include "LY_AgricultureHomepageActivator.h"
#include <QtWidgets/QWidget>
#include <QUuid>
#include <QList>
#include <QTimer>
#include <QDebug>
#include <QTreeWidget>
#include "DataManage/AgriFarm/AgriHomePageDecl.h"
#include "ly_restoresensorwidget.h"
#include <set>
#include <algorithm>
#include <QShowEvent>
#include <QTimer>

using namespace std;

 //传感器数据结构;
struct SensorDataStruct
{
    //QColor alarmColor;
    QString sensorName;
   // float sensorNumber;
};

// 节点数据结构;
struct NodeDataStruct
{
    QString strNodeName;
   // int strNodeCount;
    //QColor numberColor;
    QList<SensorDataStruct> sensorDataList;
};


class LY_MonitorTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LY_MonitorTreeWidget(QWidget *parent = nullptr);
	~LY_MonitorTreeWidget();

	LY_MointorViewModel *m_viewModel;

	void updateData();
	bool DrawDynamic(QPainter &painter, IProj *pProj);

protected:

	void showEvent(QShowEvent *event);

private:

	QString m_currentFarm;

private:
    void initWidget();
	void addData();

	//获取等级
	int getNodeLevel(QTreeWidgetItem* item) const;
	
	//添加盐田node
	//QTreeWidgetItem* createNode(AgriFieldInfoModel &model);


	//添加传感器类型
	void addSensorType(QTreeWidgetItem* parent);
	//添加传感器
	void addSensor(QTreeWidgetItem* parent);
	//删除传感器类型
	void deleteSensorType(QTreeWidgetItem* item);
	//删除传感器
	void deleteSensor(QTreeWidgetItem* item);
	//修改传感器
	void updateSensor(QTreeWidgetItem* item);


signals:

	void clickTreeItem(SensorListModel &model, int column);
	void doubleClickTreeItem(SensorListModel &model, int column);
private slots:

     void showContextMenu(const QPoint &pos);
	 void itemClicked(QTreeWidgetItem *item, int column);
	 void itemDoubleClicked(QTreeWidgetItem *item, int column);
	 void updateSensorGroup(SensorListModelVec leftVec, SensorListModelVec rightVec);
	 void deleteSensorRestor(QString mointID, QString mointName);
	 void on_time_out();


private:
	QTreeWidget *m_treeWidget;

	AgriAgriSensorGroupModelVec m_sensor_group;
	AgriSensorGroupModel m_current_fieldInfo;
	SensorListModelVec m_sensor_list;

	void runBatFile(const QString& batFilePath);


	QString GenerateUuid()
	{
		const QByteArray &ba = QUuid::createUuid().toRfc4122();
		QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
		return uid.toString(QUuid::WithoutBraces);
	}
	AgriAgriSensorGroupModelVec filterUniqueById(AgriAgriSensorGroupModelVec& sensors) {
		std::set<QString> seenIds;
		std::vector<AgriSensorGroupModel> result;
		for (const auto& sensor : sensors) {
			if (seenIds.insert(sensor.id).second) {
				result.push_back(sensor);
			}
		}
		return result;
	}

	
};






#endif // LY_MONITORTREEWIDGET_H
