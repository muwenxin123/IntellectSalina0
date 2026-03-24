#include "ly_mointorviewmodel.h"
#include <QDebug>
#include "CurlUtil.h"
#include <QSettings>
#include <QThread>
#include <lycore.h>
#include <QUrl>
#include "LyMessageBox.h"
#include "QGeoCoordinate"
#include "LY_AgricultureHomepage.h"
#include "CreateShape.h"

#define PI			3.1415926
#define NPrecision	10000
#define  LOCAL_PATH_MOINTOR "../data/MointorManager.ini"
using json = nlohmann::json;


namespace GlobalUtils {

	QString GetSensorStringWithType(SensorType type)
	{
		switch (type)
		{
		case SensorType::SensorTypeType_None: return tr2("未知");
		case SensorType::SensorTypeType_Video: return  tr2("实时监控");
		case SensorType::SensorTypeType_Soil: return  tr2("土壤传感器");
		case SensorType::SensorTypeType_Weather: return  tr2("气象传感器");
		case SensorType::SensorTypeType_PestMonitor: return  tr2("虫情检测仪");
		case SensorType::SensorTypeType_KillInsects: return  tr2("杀虫仪器");
		case SensorType::SensorTypeType_Valve: return  tr2("阀门");
		case SensorType::SensorTypeType_FarmCar: return  tr2("农机");
		case SensorType::SensorTypeType_FlyStop: return  tr2("大疆机场");
		default: return  tr2("未知");
		}
	}

}
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;
	std::string* str = static_cast<std::string*>(userp);
	str->append(static_cast<char*>(contents), realsize);
	return realsize;
}

LY_MointorViewModel::LY_MointorViewModel(QObject *parent) : QObject(parent)
{
	init();
	requestSoilData();


}

void LY_MointorViewModel::init()
{
	
	m_strDataDir = tr2(LY_AgricultureHomepage::Get().GetDataDir().data());

	QThread *tch_thread = new QThread;
	tcpServer = new QTcpServer(this);
	tcpServer->moveToThread(tch_thread);//将tcp类加入到子线程内
	connect(tch_thread, &QThread::finished, this, &QObject::deleteLater);
	tch_thread->start();

	// 连接到下位机的IP和端口
	QHostAddress address("192.168.88.101");
	quint16 port = 10080;
	//tcpSocket->connectToHost(address, port); // 请替换为实际IP和端口
	if (!tcpServer->listen(address, port)) {
		qDebug() << "Server could not start!";
		return;
	}
	connect(tcpServer, &QTcpServer::newConnection, this, &LY_MointorViewModel::onNewConnection);
	//connect(tcpSocket, &QTcpSocket::readyRead, this, &LY_MointorViewModel::onReadyRead);
}

void LY_MointorViewModel::onNewConnection() {
	QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
	//clients.append(clientSocket);
	connect(clientSocket, &QTcpSocket::readyRead, this, &LY_MointorViewModel::onReadyRead);
	connect(clientSocket, &QTcpSocket::disconnected, this, &LY_MointorViewModel::onClientDisconnected);
	qDebug() << "New client connected!";
}

void LY_MointorViewModel::onReadyRead() {
	QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
	QByteArray data = clientSocket->readAll();
	sbuf = data;
 

	reciverData(data);

	// 回复客户端
	//ientSocket->write("Data received");
}



void LY_MointorViewModel::onClientDisconnected() {
	QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
	//clients.removeAll(clientSocket);
	clientSocket->deleteLater();
	qDebug() << "Client disconnected!";
}

void LY_MointorViewModel::reciverData(QByteArray data)
{
	qDebug() << "receive==data==:" << data;

	/*解析规则
	*其中FE DC为固定帧头，01为版本号，设备唯一id为16 6C 79 1B DF B6,本条指令的session为 00 00 01 F6，C3是命令字节，
00 30数据长度(2字节) ，后面每四个字节是一个数**/
	if (sbuf.count() == 0)
		return;

	// 提取固定帧头
	QByteArray frameHeader = data.mid(0, 2);
	if (frameHeader != QByteArray::fromHex("FE DC")) {
		qDebug() << "Invalid frame header.";
		return;
	}
	// 提取版本号
	quint8 version = static_cast<quint8>(data[2]);
	// 提取设备唯一ID
	QByteArray deviceId = data.mid(3, 6);
	// 提取Session ID
	QByteArray sessionId = data.mid(9, 4);
	// 提取命令字
	quint8 command = static_cast<quint8>(data[13]);
	// 提取数据长度
	quint16 dataLength = static_cast<quint16>(data.mid(14, 2).toHex().toUInt(nullptr, 16));

	// 输出提取的信息f
	qDebug() << "Frame Header:" << frameHeader.toHex();
	qDebug() << "Version:" << version;
	qDebug() << "Device ID:" << deviceId.toHex();
	qDebug() << "Session ID:" << sessionId.toHex();
	qDebug() << "Command:" << command;
	qDebug() << "Data Length:" << dataLength;

	int i = 16;
	if ((i + 3) < data.size())
	{

		//温度可能是负数
		QByteArray tempArray = data.mid(16, 3);
		if (tempArray.contains((static_cast<char>(0xFF))))
		{
			int signedDecimal = hexComplementToDecimal(data);
			float tem = signedDecimal / 10.0f;
			temp = QString::number(tem, 'f', 1);
		}
		else {
			quint32 tem = static_cast<quint32>(
				(static_cast<quint8>(data[i]) << 24) |
				(static_cast<quint8>(data[i + 1]) << 16) |
				(static_cast<quint8>(data[i + 2]) << 8) |
				(static_cast<quint8>(data[i + 3])));
			float temperature = tem / 10.0f;  // 转换为百分比
			temp = QString::number(temperature, 'f', 1) ;
		}
		//湿度
		quint32 hum = static_cast<quint32>(
			(static_cast<quint8>(data[i + 4]) << 24) |
			(static_cast<quint8>(data[i + 5]) << 16) |
			(static_cast<quint8>(data[i + 6]) << 8) |
			(static_cast<quint8>(data[i + 7])));
		float humidityRaw = hum / 10.0f;  // 转换为百分比
	     humidity = QString::number(humidityRaw, 'f', 1) ;

		emit receiveWeather(temp, humidity);
	}
	// //提取后续数据
	//QList<quint32> numbers;
	//for (int i = 16; i < dataLength + 16; i += 4) {
	//	if (i + 3 < data.size()) {
	//		quint32 number = static_cast<quint32>(
	//			(static_cast<quint8>(data[i]) << 24) |
	//			(static_cast<quint8>(data[i + 1]) << 16) |
	//			(static_cast<quint8>(data[i + 2]) << 8) |
	//			(static_cast<quint8>(data[i + 3]))
	//			);
	//		numbers.append(number);
	//	}
	//}

	//// 输出转换后的数字
	//qDebug() << "Numbers:";
	//for (quint32 number : numbers) {
	//	qDebug() << number;
	//}
}
int LY_MointorViewModel::hexComplementToDecimal(const QByteArray& hexData) {
	QString hexStr = QString::number(static_cast<unsigned char>(hexData.at(16)), 4).toUpper();

	// 将十六进制字符串转换为无符号整数
	bool ok;
	quint8 unsignedValue = hexStr.toUInt(&ok, 16); // 使用 quint8 解析 8 位数
	if (!ok) {
		qDebug() << "Invalid hex string.";
		return 0;
	}

	// 检查是否为负数（在 8 位补码中，最高位为 1 时表示负数）
	if (unsignedValue & 0x80) { // 8位的符号位
		return static_cast<int>(unsignedValue) - (1 << 8); // 转换为负数
	}

	return static_cast<int>(unsignedValue);
}
void LY_MointorViewModel::requestSoilData() {

	std::thread worker([=]() {
		string response = CurlUtil::Get().post(LoginUrl, "{\"loginName\": \"cq241128bjly\",\"loginPwd\": \"cq241128bjly\"}", true);

		if (response == "")
		{
			return;
		}
		auto root = json::parse(response.c_str());
		if (!root.contains("code") || !root.contains("message") || !root.contains("data")) {
			return;
		}
		auto data = root.at("data");
		if (!data.is_object()) {
			return ;
		}
		std::string token = data.at("token");
		//qDebug() << "==__:"<< QString::fromStdString(token);
		if ( token.empty()) {
			return;
		}
		emit setToken(QString::fromStdString(token));

	});

	worker.detach();

	connect(this, &LY_MointorViewModel::setToken, this, [this](QString token ) {
		token_str = token.toStdString();
	});

	// 处理所有请求
	//async_curl.perform();
	
	timer = new QTimer();
	connect(timer, &QTimer::timeout, this, &LY_MointorViewModel::timerTimeout);
	timer->start(10000); // 定时器每隔1000毫秒触发一次timeout信号
}

void LY_MointorViewModel::timerTimeout()
{
	clearn3dText();
	std::thread worker([=]() {

		if (token_str == "")
		{
			return;
		}

		QString token = QString("token:%1").arg(QString::fromStdString(token_str));
		CURLcode code;
		string respons = CurlUtil::Get().get(GetDataUrl, &code, { token.toUtf8().data() });

		if (respons == "")
		{
			return;
		}

		try {
			// 解析JSON
			json root = json::parse(respons);
			DeviceListResponse response = root.get<DeviceListResponse>();
			if (response.code != 1000)
			{
				qDebug() << "response fail" << QString::fromStdString(response.message);
				return;
			}
			for (const auto& device : response.data) {
	
				if (device.deviceType == "wormFlagship")
				{
					if (device.status == "online")
					{
						m_wormDeviceAddr = device.deviceAddr;
						PestMointer model = device.data;
						emit receivePestMointerata(model);
					}

				}
				else if (device.deviceType == "soil") {

					if (device.status == "online")
					{
						json data_array = root["data"];
						if (data_array == nullptr)
						{
							continue;
						}
						json item;
						if (data_array.size() < 2)
						{
							item = data_array[0]["data"];
						}
						else
						{
							item = data_array[1]["data"];
						}
						 
						if (item == nullptr)
						{
							continue;
						}
						for (size_t i = 0; i < item.size(); i++)
						{

							//检查索引是否越界
							if (i >= item.size()) {
								qDebug() << "Index out of bounds:" << i;
								continue;
							}
							if (item[i].is_null()) {
								qDebug() << "Item at index" << i << "is null";
								continue;
							}
							Soil model = item[i].get<Soil>();
							QVariant varValue = QVariant::fromValue(model);
							//this->m_model = model;
							emit receiveSoilData(model);
						}
					}
				}
			}
		}
		catch (const json::exception& e) {
			// qDebug() << "JSON解析错误: " << e.what();
		}
	
	
	});
	worker.detach();

}

void  LY_MointorViewModel::GetWormOperationLog()
{

	std::thread worker([=]() {
	
		QVector <WormOperationLog> vec;
		vec.clear();
		if (token_str == "")
		{
			return ;
		}
	/*	if (m_wormDeviceAddr == "")
		{
			emit errorCurrent();
			return ;
		}*/
		QString token = QString("token:%1").arg(QString::fromStdString(token_str));
		//QString token = QString("token:199691746685675501");
		CURLcode code;
		QString baseUrl(GetWormOperationLogUrl);

		QString beginTime = QDateTime::fromString("2025-04-29 14:24:16", "yyyy-MM-dd HH:mm:ss")
			.toString(Qt::ISODate);
		QString endTime = QDateTime::fromString("2025-04-30 14:24:16", "yyyy-MM-dd HH:mm:ss")
			.toString(Qt::ISODate);
		QString params = QString("deviceAddr=%1&beginTime=%2&endTime=%3")
			.arg("0108250003")
			.arg("2025-04-29")
			.arg("2025-04-30");
		QString requestUrl = baseUrl + "?" + params; //带空格单独编码
		string respons = CurlUtil::Get().get(requestUrl.toStdString().c_str(), &code, { token.toUtf8().data() });
		if (respons == "")
		{
			return;
		}
		try
		{
			json root = json::parse(respons);
			if (root == nullptr)
			{
				return ;
			}
			int code = root["code"];
			if (code != 1000)
			{
				qDebug() << "Index out of bounds:" << QString::fromStdString(root["message"]);
				return ;
			}
			json data_array = root["data"];
			if (data_array == nullptr)
			{
				
				return ;
			}
			for (size_t i = 0; i < data_array.size(); i++)
			{

				//检查索引是否越界
				if (i >= data_array.size()) {
					qDebug() << "Index out of bounds:" << i;
					continue;
				}
				if (data_array[i].is_null()) {
					qDebug() << "Item at index" << i << "is null";
					continue;
				}
				WormOperationLog model = data_array[i].get<WormOperationLog>();
				QVariant varValue = QVariant::fromValue(model);
				vec.push_back(model);
				
			}

			emit getReocdeData(vec);
		}
		catch (const std::exception&e)
		{
			qDebug() << "JSON解析错误: " << e.what();
		}

	});
	worker.detach();
}
void LY_MointorViewModel::GetWormHistoryData()
{
	std::thread worker([=]() {

		QVector <WormOperationLog> vec;
		vec.clear();
		if (token_str == "")return;

	/*	if (m_wormDeviceAddr == "")
		{
		  emit errorCurrent();
		  return ;
		}*/
		QString token = QString("token:%1").arg(QString::fromStdString(token_str));
		CURLcode code;
		QString baseUrl(GetWormWormHistoryData);

		QString beginTime = QDateTime::fromString("2025-04-08 15:51:42", "yyyy-MM-dd HH:mm:ss")
			.toString(Qt::ISODate);
		QString endTime = QDateTime::fromString("2025-05-08 15:51:42", "yyyy-MM-dd HH:mm:ss")
			.toString(Qt::ISODate);
		QString params = QString("deviceAddr=%1&beginTime=%2&endTime=%3&limit=%4&pages=%5")
			.arg("0108250003")
			.arg(beginTime)
			.arg(endTime)
			.arg(15)
			.arg(1);
		QString requestUrl = baseUrl + "?" + params; //带空格单独编码
		string respons = CurlUtil::Get().get(requestUrl.toStdString().c_str(), &code, { token.toUtf8().data() });
		if (respons == "")
		{
			return;
		}
		try
		{
			json root = json::parse(respons);
			if (root == nullptr)
			{
				return;
			}
			int code = root["code"];
			if (code != 1000)
			{
				qDebug() << "Index out of bounds:" << QString::fromStdString(root["message"]);
				return;
			}
			json data = root["data"];
			if (data == nullptr)
			{
				return;
			}
			WormHistToryData model = data.get<WormHistToryData>();
			qDebug() << "========totalPages=========:" << model.pageInfo.totalPages;
			for (auto m : model.rows)
			{
				qDebug() << "========totalPages111=========:" << QString::fromStdString(m.value);
			}

			emit getHistoryData(model);
		}
		catch (const std::exception&e)
		{
			qDebug() << "JSON解析错误: " << e.what();
		}

	});
	worker.detach();
}
void LY_MointorViewModel::GetWormDataList()
{
	std::thread worker([=]() {

		QVector <WormOperationLog> vec;
		vec.clear();
		if (token_str == "") return;

		/*if (m_wormDeviceAddr == "")
		{
		   emit errorCurrent();
		  return ;
		}*/
		QString token = QString("token:%1").arg(QString::fromStdString(token_str));
		//QString token = QString("token:93191746698371933");
		CURLcode code;
		QString baseUrl(GetWormDataListURL);
		QString beginTime = QDateTime::fromString("2025-04-08 15:51:42", "yyyy-MM-dd HH:mm:ss")
			.toString(Qt::ISODate);
		QString endTime = QDateTime::fromString("2025-05-08 15:51:42", "yyyy-MM-dd HH:mm:ss")
			.toString(Qt::ISODate);
		QString params = QString("deviceAddr=%1&beginTime=%2&endTime=%3&limit=%4&pages=%5")
			.arg("0108250003")
			.arg(beginTime)
			.arg(endTime)
			.arg(4)
			.arg(1);
		QString requestUrl = baseUrl + "?" + params; //带空格单独编码
		string respons = CurlUtil::Get().get(requestUrl.toStdString().c_str(), &code, { token.toUtf8().data() });
		if (respons == "")
		{
			return;
		}
		try
		{
			json root = json::parse(respons);
			if (root == nullptr)
			{
				return;
			}
			int code = root["code"];
			if (code != 1000)
			{
				qDebug() << "Index out of bounds:" << QString::fromStdString(root["message"]);
				return;
			}
			json data = root["data"];
			if (data == nullptr)
			{
				return;
			}
			WormRecordData model = data.get<WormRecordData>();
			//qDebug() << "========totalPages=========:" << model.pageInfo.total;
			for (auto m : model.rows)
			{
				//qDebug() << "========totalPages111=========:" << QString::fromStdString(m.deviceAddr);
			}

			emit getWormDataList(model);
		}
		catch (const std::exception&e)
		{
			qDebug() << "JSON解析错误: " << e.what();
		}

	});
	worker.detach();
}
void LY_MointorViewModel::GetSoilTargetEnabledNodeList()
{
	std::thread worker([=]() {

		QVector <SoilSensorData> vec;
		vec.clear();
		if (token_str == "") return;

		/*if (m_wormDeviceAddr == "")
		{
		emit errorCurrent();
		return ;
		}*/
		QString token = QString("token:%1").arg(QString::fromStdString(token_str));
		CURLcode code;
		QString baseUrl(GetListTargetEnabledNodeURL);
		QString params = QString("deviceAddr=%1").arg("40359823");

		QString requestUrl = baseUrl + "?" + params; //带空格单独编码
		string respons = CurlUtil::Get().get(requestUrl.toStdString().c_str(), &code, { token.toUtf8().data() });
		if (respons == "")
		{
			return;
		}
		try
		{
			json root = json::parse(respons);
			if (root == nullptr)
			{
				return;
			}
			int code = root["code"];
			if (code != 1000) return;
			json data_array = root["data"];
			if (data_array == nullptr)
			{
				return;
			}

			for (size_t i = 0; i < data_array.size(); i++)
			{

				//检查索引是否越界
				if (i >= data_array.size()) continue;
				if (data_array[i].is_null()) continue;

				SoilSensorData model = data_array[i].get<SoilSensorData>();
				vec.push_back(model);

			}
			emit getSoilSensorDataType(vec);
		}
		catch (const std::exception&e)
		{
			qDebug() << "JSON error: " << e.what();
		}

	});
	worker.detach();
}

void LY_MointorViewModel::controlPestDevice(PEST_DEVICE_STATUS divice_type, bool isOpen)
{
	std::thread worker([=]() {
		if (token_str == "") return;

		/*if (m_wormDeviceAddr == "")
		{
			emit errorCurrent();
			return;
		}*/

		CURLcode code;
		std::string deviceAddr = m_wormDeviceAddr;
		std::string module = GetPestDeviceWithType(divice_type);
		std::string op= QString::number(isOpen).toStdString();
		std::string json_request = build_request(deviceAddr, module, op);


		QString token = QString("token:%1").arg(QString::fromStdString(token_str));
		string respons = CurlUtil::Get().post(ControlDeviceUrl, json_request.c_str(), true, &code, { token.toUtf8().data() });
		try {
			// 解析JSON
			json root = json::parse(respons);
			if (root == nullptr)
			{
				return;
			}
			int code = root["code"];
			std::string message = root["message"];
			//qDebug() << "====***==:" << code << QString::fromStdString(message);
			if (code == 1000)
			{
				emit contrlDeviceStatus(divice_type, isOpen);
			}
		}
		catch (const json::exception& e) {
			qDebug() << "JSON解析错误: " << e.what();
		}
	});
	worker.detach();
}
void LY_MointorViewModel::GetSoilHistoryDataList(QString  nodeID)
{
	std::thread worker([=]() {

		QVector <SoilHistoryData> vec;
		vec.clear();
		if (token_str == "") return;

		/*if (m_wormDeviceAddr == "")
		{
		emit errorCurrent();
		return ;
		}*/
		QString token = QString("token:%1").arg(QString::fromStdString(token_str));
		CURLcode code;
		QString baseUrl(GetSoilHistoryDataListURL);
		QString beginTime = QDateTime::fromString("2025-04-08 15:51:42", "yyyy-MM-dd HH:mm:ss")
			.toString(Qt::ISODate);
		QString endTime = QDateTime::fromString("2025-05-08 15:51:42", "yyyy-MM-dd HH:mm:ss")
			.toString(Qt::ISODate);
		QString params = QString("deviceAddr=%1&beginTime=%2&endTime=%3&nodeId=%4&limit=%5&pages=%6")
			.arg("40359823")
			.arg("1746689811425")
			.arg("1746776211425")
			.arg(nodeID)
			.arg(100)
			.arg(1);


		QString requestUrl = baseUrl + "?" + params; //带空格单独编码
		string respons = CurlUtil::Get().get(requestUrl.toStdString().c_str(), &code, { token.toUtf8().data() });
		if (respons == "")
		{
			return;
		}
		try
		{
			json root = json::parse(respons);
			if (root == nullptr)
			{
				return;
			}
			int code = root["code"];
			if (code != 1000) 
			{
				qDebug() << "===--00:" << QString::fromStdString(root["message"]);
				return;
			}
			json data = root["data"];
			if (data == nullptr)
			{
				return;
			}
			SoilHistoryData model = data.get<SoilHistoryData>();
			//qDebug() << "========totalPages=========:" << model.pageInfo.total;
			for (auto m : model.rows)
			{
				//qDebug() << "========totalPages111=========:" << QString::fromStdString(m.deviceAddr);
			}

			emit getSoilHistoryList(model);
		}
		catch (const std::exception&e)
		{
			qDebug() << "JSON error: " << e.what();
		}

	});
	worker.detach();
}
std::string LY_MointorViewModel::build_request(const std::string& deviceAddr, const std::string& module, const std::string &isOpen) {
	json request;
	request["deviceAddr"] = deviceAddr;
	request["module"] = module;
	request["opt"] = isOpen;
	return request.dump();
}

void LY_MointorViewModel::clearn3dText() {
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	auto itr2 = m_3DLocPositionMesh2Vec.begin();
	for (; itr2 != m_3DLocPositionMesh2Vec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_3DLocPositionMesh2Vec.clear();
}
void LY_MointorViewModel::drawMointer3dText(std::string str, SensorListModel &model, int height)
{
	vector<double> vLon;
	vector<double> vLat;
	vector<double> vAlt;
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	auto itr2 = m_3DLocPositionMesh2Vec.begin();
	//for (; itr2 != m_3DLocPositionMesh2Vec.end(); itr2++)
	//{
	//	IEarth *pEarth = IEarth::GetEarth();
	//	pEarth->GetGraphics()->RemoveObject(*itr2);
	//	*itr2 = nullptr;
	//	delete *itr2;
	//}
	//m_3DLocPositionMesh2Vec.clear();

	QColor colorBombTrack2(0x800000);
	colorBombTrack2.setAlpha(125);
	QColor colorTemp2;
	colorTemp2.setRgb(colorBombTrack2.blue(), colorBombTrack2.green(), colorBombTrack2.red(), colorBombTrack2.alpha());

	unsigned int uiV = TRGB(255, 255, 255, 255);
	QColor color = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
	color.setAlpha(100);

	//std::string name = this->m_model.temName + " " +this->m_model.temValueStr;

	IText* mshpText = pEarth->GetGraphics()->CreateText(QString::fromStdString(str).toLocal8Bit(), model.longitude + 0.0004 , model.latitude, model.localHeight + height, NULL, 15,
		TRGB(128, 255, 255, 255), TRGB(128, 128, 128, 128), TRGB(128, 128, 0, 128), 7);
	if (mshpText != nullptr)
	{
		m_3DLocPositionMesh2Vec.push_back(mshpText);
	}

	/*std::string str1 = u8"气象传感器:" + fen + u8"温度:" + fen + u8"湿度:";
	IText* mshpText1 = pEarth->GetGraphics()->CreateText(QString::fromStdString(str1).toLocal8Bit(), 116.1960, 40.2642, 130, NULL, 10,
		TRGB(128, 255, 255, 255), TRGB(128, 128, 128, 128), TRGB(128, 128, 0, 128), 7);
	if (mshpText != nullptr)
	{
		m_3DLocPositionMesh2Vec.push_back(mshpText1);
	}*/
}

void LY_MointorViewModel::draw3DWormImg(double lon, double lat, double height)
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	QColor col(255, 0, 0, 255);

	char * imgStr = "Temp_Line/vermin.png";
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(QUuid::createUuid().toRfc4122(), 16));
	QString strLable = "";

	if (m_wormLabelMap.find(uid) == m_wormLabelMap.end())
	{
		std::string strImg;
		auto it = m_mapLabelPath.find(uid);
		if (it == m_mapLabelPath.end())
		{
			QString strPath = QObject::tr(m_strDataDir);
			strPath += QLatin1String(imgStr);
			QImage img = QImage(strPath);
			strPath = QObject::tr(m_strDataDir);
			strPath += QLatin1String("/temp/");
			strPath += uid.toString(QUuid::WithoutBraces);
			strPath += QLatin1String(".png");
			QFile::remove(strPath);
			bool bresult = img.save(strPath);
			strImg = tr3(strPath).data();
			m_mapLabelPath[uid] = strImg;
		}
		else
		{
			strImg = it->second;
		}

		ILabel *pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(),
			lon, lat, height, 30,30, 0); 
		if (pLabel != nullptr)
		{
			m_3DTakeOffMeshVec.push_back(pLabel);
			m_wormLabelMap[uid] = pLabel;
		}

	}
}


void LY_MointorViewModel::click3DPoint()
{

	//IEarth *pEarth = IEarth::GetEarth();
	//if (pEarth == nullptr)
	//	return;
	//if (pUM1)
	//{
		//pEarth->GetGraphics()->RemoveObject(pUM1);
		/*	pUM1 = nullptr;
			delete pUM1;*/
	//}
	//QGeoCoordinate min1(nearlyLenSide[0][1], nearlyLenSide[0][0], nearlyLenSide[0][2]);
	//QGeoCoordinate min2(nearlyLenSide[1][1], nearlyLenSide[1][0], nearlyLenSide[1][2]);
	//QGeoCoordinate min1toMin(nearlyLenSide[2][1], nearlyLenSide[2][0], nearlyLenSide[2][2]);
	//QGeoCoordinate min2toMin(nearlyLenSide[3][1], nearlyLenSide[3][0], nearlyLenSide[3][2]);

	//double lenDir = min1.azimuthTo(min2);
	//double shortDir = min1.azimuthTo(min1toMin);

	//int targetShort = min1.distanceTo(min1toMin);
	//int targetLen = min1.distanceTo(min2);

	//int podTurnTime = fabs(minAngle - maxAngle) / podTurnSpeed * 2;

	//for (auto model : m_monitorSubList)
	//{
	//	QGeoCoordinate min1(model.latitude, model.longitude, model.localHeight);

	//	for (auto zonePoint : m_clickPointVec) {

	//		double x = zonePoint.x - model.screenX;
	//			double y = zonePoint.y - model.screenY;
	//			double c = std::sqrt(x * x + y * y);
	//			if (c < 350)
	//			{
	//				//convert3DCube(model.longitude, model.latitude, model.localHeight);
	//				break;
	//			}
	//	}
	//}

}

