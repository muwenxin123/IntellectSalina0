#ifndef LY_MOINTORVIEWMODEL_H
#define LY_MOINTORVIEWMODEL_H

#include <QObject>
#include <nlohmann/json.hpp>
#include "iostream"
#include <QMetaType>
#include <QTimer>

#include <QMutex>
#include <vector>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTcpServer>
#include <QPainter>
#include "QCString.h"
#include "map3d/IEarth.h"
#include "map3d\IGraphics.h"
#include <map3d/EarthEvent.h>
#include "vector3.h"
#include <QTimer>
#include "IProj.h"

#include "DataManage/AgriFarm/AgriHomePageDecl.h"

using nlohmann::json;
using namespace std;


const static char* url = "http://api.farm.0531yun.cn";
const static char* LoginUrl = "http://api.farm.0531yun.cn/api/v2.0/entrance/user/userLogin";
const static char* GetDataUrl = "http://api.farm.0531yun.cn/api/v2.0/entrance/device/getRealTimeData";
const static char* ControlDeviceUrl = "http://api.farm.0531yun.cn/api/v2.0/worm/device/deviceOper/wormOper";
const static char* GetWormOperationLogUrl = "http://api.farm.0531yun.cn/api/v2.0/worm/deviceData/getWormOperationLog";
const static char* GetWormWormHistoryData = "http://api.farm.0531yun.cn/api/v2.0/worm/deviceData/getWormHistoryData";
const static char* GetWormDataListURL = "http://api.farm.0531yun.cn/api/v2.0/worm/deviceData/getWormDataList";
const static char* GetListTargetEnabledNodeURL = "http://api.farm.0531yun.cn/api/v2.0/met/device/listTargetEnabledNode";
const static char* GetSoilHistoryDataListURL = "http://api.farm.0531yun.cn//api/v2.0/soil/history/getHistoryDataList";

struct PestMointer
{
	bool wormFlap = false; // 杀虫挡板 0 关 1 开
	bool shake = false; // 震动装置 0 关 1 开
	bool dryingFlap = false; // 烘干挡板 0 关 1 开
	bool insecticide = false; // 杀虫控制 0 关 1 开
	bool moveWorm = false; // 移虫挡板 0 关 1 开
	bool drying = false; // 烘干控制 0 关 1 开
	bool rainFlap = false; // 雨虫挡板 0 关 1 开
	bool attractWorm = false; // 诱虫灯开关 0 关 1 开
	bool fillLight = false; //补光灯 0 关 1开
	bool status = false;  //设备状态
	std::string  dryingTem = "";  // 烘干温度	
	std::string  insecticideTem = "";  //杀虫温度
	std::string  illum = "";  //光照度
	std::string  voltage = "";  //电压


    friend void to_json(nlohmann::json& j, const PestMointer& pm) {
    	auto boolToString = [](bool value) { return value ? "1" : "0"; };
    
    	j = nlohmann::json{
    		{ "wormFlap", boolToString(pm.wormFlap) },
    		{ "shake", boolToString(pm.shake) },
    		{ "dryingFlap", boolToString(pm.dryingFlap) },
    		{ "insecticide", boolToString(pm.insecticide) },
    		{ "moveWorm", boolToString(pm.moveWorm) },
    		{ "drying", boolToString(pm.drying) },
    		{ "rainFlap", boolToString(pm.rainFlap) },
    		{ "attractWorm", boolToString(pm.attractWorm) },
    		{ "fillLight", boolToString(pm.fillLight) },
    		{ "status", boolToString(pm.status) },
    		// 字符串字段直接赋值
    		{ "dryingTem", pm.dryingTem },
    		{ "insecticideTem", pm.insecticideTem },
    		{ "illum", pm.illum },
    		{ "voltage", pm.voltage }
    	};
    }

    friend  void from_json(const nlohmann::json& j, PestMointer& pm) {
	// 布尔字段：安全转换 "0"/"1" 字符串为 bool
	auto parseBoolFromString = [&j](const char* key, bool& output) {
		if (j.contains(key) && j[key].is_string()) {
			const std::string val = j[key].get<std::string>();
			output = (val == "1");
		}
		else {
			// 字段缺失或类型错误时保持默认值
			output = false;
		}
	};

	parseBoolFromString("wormFlap", pm.wormFlap);
	parseBoolFromString("shake", pm.shake);
	parseBoolFromString("dryingFlap", pm.dryingFlap);
	parseBoolFromString("insecticide", pm.insecticide);
	parseBoolFromString("moveWorm", pm.moveWorm);
	parseBoolFromString("drying", pm.drying);
	parseBoolFromString("rainFlap", pm.rainFlap);
	parseBoolFromString("attractWorm", pm.attractWorm);
	parseBoolFromString("fillLight", pm.fillLight);
	parseBoolFromString("status", pm.status);

	// 字符串字段：直接提取（带安全判断）
	auto getStringSafe = [&j](const char* key, std::string& output) {
		if (j.contains(key)) {
			if (j[key].is_string()) {
				output = j[key].get<std::string>();
			}
			else {
				// 处理非字符串类型（如数字）
				output = std::to_string(j[key].get<int>());
			}
		}
		else {
			output.clear(); // 字段不存在时置空
		}
	};

	getStringSafe("dryingTem", pm.dryingTem);
	getStringSafe("insecticideTem", pm.insecticideTem);
	getStringSafe("illum", pm.illum);
	getStringSafe("voltage", pm.voltage);
}

};
Q_DECLARE_METATYPE(PestMointer);
//deviceType :设备类型 虫情设备普通版：worm；虫情设备旗舰版：wormFlagship；孢子设备：spore；
//气象设备：met；墒情设备：soil；智慧环控 3.0 设备：irrigation；智慧环
//控 3.1 设备：irrigation3.1；智慧环控 2.0 设备：irrigation2；摄像头：camera；
struct Device {
	std::string deviceAddr = "";
	std::string deviceName = "";
	int lat = 0;
	int lng = 0;
	std::string status = "";
	std::string deviceType = "";
	uint64_t timeStamp = 0;
	PestMointer data;         // 嵌套对象

	friend  void from_json(const nlohmann::json& j, Device& d) {
		j.at("deviceAddr").get_to(d.deviceAddr);
		j.at("deviceName").get_to(d.deviceName);
		j.at("lat").get_to(d.lat);
		j.at("lng").get_to(d.lng);
		j.at("status").get_to(d.status);
		j.at("deviceType").get_to(d.deviceType);
		j.at("timeStamp").get_to(d.timeStamp);
		j.at("data").get_to(d.data);  // 解析嵌套对象
	}
};
Q_DECLARE_METATYPE(Device);
// 最外层数据结构
struct DeviceListResponse {
	int code = 0;
	std::string  message = "";
	std::vector<Device> data;

	friend  void from_json(const nlohmann::json& j, DeviceListResponse& res) {
		j.at("code").get_to(res.code);
		j.at("message").get_to(res.message);
		j.at("data").get_to(res.data);
	}
};
Q_DECLARE_METATYPE(DeviceListResponse);


struct SoilSensorData {
	int nodeId;
	std::string deviceAddr;
	std::string nodeName;
//	bool enable;
	int nodeMold;
	int nodeType;
	int priority;
	int digits;

	// 温度相关字段
	std::string temName;
	std::string temUnit;
	double temRatio;
	double temOffset;
	double temUpperLimit;
	double temLowerLimit;

	// 湿度相关字段
	std::string humName;
	std::string humUnit;
	double humRatio;
	double humOffset;
	double humUpperLimit;
	double humLowerLimit;

	// 开关相关字段
	std::string switchOnContent;
	std::string switchOffContent;
	int switchAlarmType;

	// 报警相关字段
	//bool voiceEnable;
	//bool smsEnable;
	//bool emailEnable;
	//bool overLimitEnable;
	std::string alarmContent;

	// 可选：如果有调节信息列表
	std::vector<int> regulatingInfoList;


	NLOHMANN_DEFINE_TYPE_INTRUSIVE(SoilSensorData,
		nodeId, deviceAddr, nodeName, /*enable,*/ nodeMold, nodeType, priority, digits,
		temName, temUnit, temRatio, temOffset, temUpperLimit, temLowerLimit,
		humName, humUnit, humRatio, humOffset, humUpperLimit, humLowerLimit,
		switchOnContent, switchOffContent, switchAlarmType,
		/*voiceEnable, smsEnable, emailEnable, overLimitEnable, */alarmContent);
};
Q_DECLARE_METATYPE(SoilSensorData);
//bool wormFlap = false; // 杀虫挡板 0 关 1 开
//bool shake = false; // 震动装置 0 关 1 开
//bool dryingFlap = false; // 烘干挡板 0 关 1 开
//bool insecticide = false; // 杀虫控制 0 关 1 开
//bool moveWorm = false; // 移虫挡板 0 关 1 开
//bool drying = false; // 烘干控制 0 关 1 开
//bool rainFlap = false; // 雨虫挡板 0 关 1 开
//bool attractWorm = false; // 诱虫灯开关 0 关 1 开
//bool fillLight = false; //补光灯 0 关 1开
//bool status = false;  //设备状态
typedef enum  PEST_DEVICE_STATUS {
	PEST_DEVICE_STATUS_None,
	PEST_DEVICE_STATUS_WormFlap,
	PEST_DEVICE_STATUS_Sharke,
	PEST_DEVICE_STATUS_DryingFlap,
	PEST_DEVICE_STATUS_Insecticide,
	PEST_DEVICE_STATUS_MoveWorm,
	PEST_DEVICE_STATUS_Drying,
	PEST_DEVICE_STATUS_RainFlap,
	PEST_DEVICE_STATUS_AttractWorm,
	PEST_DEVICE_STATUS_FillLight,
	PEST_DEVICE_STATUS_Camera,
};
Q_DECLARE_METATYPE(PEST_DEVICE_STATUS);


struct WormOperationLog {
	int recordId = 0;
	std::string deviceAddr = "";
	std::string value = "";
	std::string userName = "";
	std::string userId = "";
	std::string createTime = "";
	std::string ip = "";

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WormOperationLog,
		recordId, deviceAddr, value, userName, userId, createTime, ip)
};
Q_DECLARE_METATYPE(WormOperationLog);



struct DeviceGistoryInfo {
	int recordId = 0;
	std::string deviceAddr = "";
	std::string value = "";
	std::string createTime = "";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DeviceGistoryInfo, recordId, deviceAddr, value, createTime);
Q_DECLARE_METATYPE(DeviceGistoryInfo);
struct PageInfo {
	int pages = 0;
	int limit = 0;
	int totalPages = 0;
	int total = 0;
};
Q_DECLARE_METATYPE(PageInfo);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PageInfo, pages, limit, totalPages, total);


struct WormHistToryData {
	PageInfo pageInfo;
	std::vector<DeviceGistoryInfo> rows;
	friend void from_json(const json& j, WormHistToryData& w) {
		j.at("pages").get_to(w.pageInfo.pages);
		j.at("limit").get_to(w.pageInfo.limit);
		j.at("totalPages").get_to(w.pageInfo.totalPages);
		j.at("total").get_to(w.pageInfo.total);
		j.at("rows").get_to(w.rows);
	}
};
Q_DECLARE_METATYPE(WormHistToryData);




struct WormRecord {
	int64_t recordId = 0;
	std::string deviceAddr = "";
	std::string analyst = "";
	std::string createTime = "";
	std::string verifyTime = "";
	std::string remark = "";
	std::string imagesUrl = "";
	std::string analyseCoordUrl = "";
	json analyseData;  // 直接使用json类型处理复杂结构
	int isAnalyse;
	std::string imagesSize;
	json analyseCoord; // 直接使用json类型
	json wormDataAIDO; // 处理可能的null值

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WormRecord,
		recordId,
		deviceAddr,
		analyst,
		createTime,
		verifyTime,
		remark,
		imagesUrl,
		analyseCoordUrl,
		analyseData,
		isAnalyse,
		imagesSize,
		analyseCoord,
		wormDataAIDO
	)
};

Q_DECLARE_METATYPE(WormRecord);


struct WormRecordData {
	PageInfo pageInfo;
	std::vector<WormRecord> rows;

	friend void from_json(const json& j, WormRecordData& w) {
		j.at("pages").get_to(w.pageInfo.pages);
		j.at("limit").get_to(w.pageInfo.limit);
		j.at("totalPages").get_to(w.pageInfo.totalPages);
		j.at("total").get_to(w.pageInfo.total);
		j.at("rows").get_to(w.rows);
	}
};
Q_DECLARE_METATYPE(WormRecordData);

struct SoilHistoryItemData {
	int64_t historyId;
	int nodeId;
	std::string deviceAddress;
	std::string temStr;
	std::string humStr;
	float temValue;
	float humValue;
	int alarmStatus;
	int64_t recordTime;

	// 反序列化函数
	friend void from_json(const json& j, SoilHistoryItemData& d) {
		j.at("historyId").get_to(d.historyId);
		j.at("nodeId").get_to(d.nodeId);
		j.at("deviceAddress").get_to(d.deviceAddress);
		j.at("temStr").get_to(d.temStr);
		j.at("humStr").get_to(d.humStr);
		j.at("temValue").get_to(d.temValue);
		j.at("humValue").get_to(d.humValue);
		j.at("alarmStatus").get_to(d.alarmStatus);
		j.at("recordTime").get_to(d.recordTime);
	}
};
Q_DECLARE_METATYPE(SoilHistoryItemData);

struct SoilHistoryData {
	PageInfo pageInfo;
	std::vector<SoilHistoryItemData> rows;

	friend void from_json(const json& j, SoilHistoryData& w) {
		j.at("pages").get_to(w.pageInfo.pages);
		j.at("limit").get_to(w.pageInfo.limit);
		j.at("totalPages").get_to(w.pageInfo.totalPages);
		j.at("total").get_to(w.pageInfo.total);
		j.at("rows").get_to(w.rows);
	}
};
Q_DECLARE_METATYPE(SoilHistoryData);
struct Soil
{

	std::string humValueStr = "";
	std::string nodeName = "";
	std::string temName = "";
	std::string temValueStr = "";
	std::string temUnit = "";
	std::string humUnit = "";
	std::string humName = "";
	int nodeId = 0;

	friend  void to_json(nlohmann::json& j, const Soil& p) {

		j = json{
			{ "humValueStr", p.humValueStr },
			{ "nodeName", p.nodeName },
			{ "temName", p.temName },
			{ "temValueStr", p.temValueStr },
			{ "temUnit", p.temUnit },
			{ "humUnit", p.temUnit },
			{ "humName", p.humName },
			{ "nodeId", p.nodeId }
		};
	}

	friend  void from_json(const nlohmann::json& j, Soil& p) {
		j.at("humValueStr").get_to(p.humValueStr);
		j.at("nodeName").get_to(p.nodeName);
		j.at("temName").get_to(p.temName);
		j.at("temValueStr").get_to(p.temValueStr);
		j.at("temUnit").get_to(p.temUnit);
		j.at("humName").get_to(p.humName);
		j.at("humUnit").get_to(p.humUnit);
		j.at("nodeId").get_to(p.nodeId);
	}
};
Q_DECLARE_METATYPE(Soil);



namespace GlobalUtils {

	QString GetSensorStringWithType(SensorType type);
}


class LY_MointorViewModel : public QObject
{
    Q_OBJECT
public:

    explicit LY_MointorViewModel(QObject *parent = nullptr);

	double lon;
	double lat;
	double altitude;
	double screenX;
	double screenY;
	//三维点击的
	vector<ZonePoint> m_clickPointVec;

	//控制设备
	void controlPestDevice(PEST_DEVICE_STATUS divice_type, bool isOpen);
	//获取操作记录
	void  GetWormOperationLog();
	//获取历史记录
	void GetWormHistoryData();
	//获取图片识别历史记录
	void GetWormDataList();
	//获取土壤类型历史记录
	void GetSoilTargetEnabledNodeList();
	//获取土壤历史记录
	void GetSoilHistoryDataList(QString  nodeID);

	//点击3d传感器
	void click3DPoint();
	// 绘制传感器文字
	void drawMointer3dText(std::string str, SensorListModel &model, int height);
	//绘制识别的虫子
	void draw3DWormImg(double lon, double lat, double height);
	void clearn3dText();

private:
	
	mutable QMutex  			sMutex;
	int index = 10;
	QTcpServer  *tcpServer;
	QByteArray sbuf;
	QCString  m_strDataDir;

	QTimer *timer;
	std::string  token_str;
	vector<QString>m_monitorList;
	vector<SensorListModel>m_localMointSubList;

	QString temp;
	QString humidity;
	//Soil m_model;
	std::string m_wormDeviceAddr;

	std::vector<IText*>m_3DLocPositionMesh2Vec;

	std::vector<ILabel*>m_3DTakeOffMeshVec;

	std::map<QUuid, std::string>				m_mapLabelPath;
	std::map<QUuid, ILabel *>			m_wormLabelMap;

private:

	void init();
	//请求土壤数据
	void requestSoilData();

	int hexComplementToDecimal(const QByteArray& hexData);
	void reciverData(QByteArray data);

	std::string build_request(const std::string& deviceAddr, const std::string& module, const std::string&isOpen);

	std::string GetPestDeviceWithType(PEST_DEVICE_STATUS type)
	{
		switch (type)
		{
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_WormFlap: return "wormFlap";
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_Sharke: return "shake";
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_DryingFlap: return "dryingFlap";
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_Insecticide: return "insecticide";
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_MoveWorm: return "moveWorm";
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_Drying: return "drying";
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_RainFlap: return "rainFlap";
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_AttractWorm: return "attractWorm";
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_FillLight: return "fillLight";
		case PEST_DEVICE_STATUS::PEST_DEVICE_STATUS_Camera: return "camera";
		default: return  "none";
		}
	}
	

signals:

	void receivePestMointerata(PestMointer p);
	void receiveSoilData(Soil s);
	void contrlDeviceStatus(PEST_DEVICE_STATUS type, bool isOpen);
	void receiveWeather(QString &tem, QString &hum);
	void setToken(QString token);
	//更新摄像头数据
	void updateData();
	void  errorCurrent();
	void getReocdeData(QVector <WormOperationLog>);
	void getHistoryData(WormHistToryData data);
	void getWormDataList(WormRecordData data);
	void getSoilSensorDataType(QVector <SoilSensorData>);
	void getSoilHistoryList(SoilHistoryData data);

private slots:

	void timerTimeout();
	void onNewConnection();
	void onReadyRead();
	void onClientDisconnected();


};

#endif // LY_MOINTORVIEWMODEL_H