#include "thread_loagWeather.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QSslSocket>
#include <QThread>
#include <QDebug>
#include <QStringList>
#include "TinyXML\tinyxml.h"
#include <QSettings>
#include <QSqlQuery>

thread_loagWeather::thread_loagWeather()
{


	m_cityVec.clear();


	TiXmlDocument docElement;
	std::string filename = "../data/CityConfig.xml";
	if (!docElement.LoadFile(filename.c_str()))
	{
		qDebug() << "open xml fail";
		return;
	}
	TiXmlElement* pRootEle = docElement.RootElement();
	if (!pRootEle)
	{
		return;
	}

	TiXmlElement* pRecordEle = pRootEle->FirstChildElement("Beijing");
	while (pRecordEle) {
		const char* city = pRecordEle->Attribute("city");
		const char* code = pRecordEle->Attribute("code");

		CityConfig cityPtr;
		cityPtr.city = city;
		cityPtr.code = code;
		m_cityVec.push_back(cityPtr);
		pRecordEle = pRecordEle->NextSiblingElement("Beijing");
	}
	


	// 查询当前激活农场
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Agri");
	QString m_currentFarm = ini.value("FarmID", "").toString();

	AgriFarmInfoVec vecFarmList = GetFarmInfoList();

	std::string city;
	for (auto& fieldInfo : vecFarmList)
	{
		// 匹配激活农场
		if (m_currentFarm == fieldInfo.id)
		{
			city = fieldInfo.name.toStdString();
			break;
		}
	}
	
	for (auto &ptr : m_cityVec)
	{
		
		if (ptr.city == city)
		{
			code = ptr.code;
			
		}
	}
}


thread_loagWeather::~thread_loagWeather()
{
}

void thread_loagWeather::working()
{
	isWorking = true;
	if (manager == NULL)
	{
		manager = new QNetworkAccessManager(this);  //新建QNetworkAccessManager对象
     

		connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));//关联信号和槽

		queryWeather();

		m_timer = new QTimer();
		connect(m_timer, &QTimer::timeout, this, [=]() {
			if (isWorking)
			{
				queryWeather();
			}
		});
		m_timer->start(600000);
		
	}

	
}
AgriFarmInfoVec  thread_loagWeather::GetFarmInfoList() const
{

	QSqlQuery query;

	AgriFarmInfoVec	farmInfoPtrVec;

	QString strSql = "SELECT `id`,  `name`, `category`, `description`, `lon`, `lat`";
	QString strGeneralSql = strSql + " FROM " + AGRI_FARM_TABLE_NAME;

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();
			QString str4 = query.value(3).toString();
			QString str5 = query.value(4).toString();
			QString str6 = query.value(5).toString();

			AgriFarmInfo pInfo;
			pInfo.id = str1;
			pInfo.name = str2;
			pInfo.category = str3;
			pInfo.description = str4;
			pInfo.lon = str5.toDouble();
			pInfo.lat = str6.toDouble();

			farmInfoPtrVec.push_back(pInfo);
		}
	}

	return farmInfoPtrVec;
}
void thread_loagWeather::setEnd()
{
	isWorking = false;
	if (m_timer != nullptr)
	{
		m_timer->stop();
	}
	
}

void thread_loagWeather::setCityCode(QString _cityCode)
{
	cityCode = _cityCode;
}

weatherData thread_loagWeather::getCurWeather()
{
	return curWeather;
}

void thread_loagWeather::queryWeather()
{

		QString questStr = "http://restapi.amap.com/v3/weather/weatherInfo?city=" + QString::fromStdString(code) + "&key=d0b4e11327345e4a90e1f75952fce81b";
		//char quest_array[256] = "http://restapi.amap.com/v3/weather/weatherInfo?city=110101&key=d0b4e11327345e4a90e1f75952fce81b";
		QNetworkRequest quest;
		//sprintf(quest_array,"%s%s",quest_array,cityName.toUtf8().data());
		quest.setUrl(QUrl(questStr));
		quest.setHeader(QNetworkRequest::UserAgentHeader, "RT-Thread ART");

		/*发送get网络请求*/
		manager->get(quest);


}


void thread_loagWeather::replyFinished(QNetworkReply *reply)
{
	this->isGetData = false;
	allinfo = reply->readAll();

	if (isLog)
	{
		qDebug() << "       KKDfjiejk ";
		qDebug() << allinfo;
	}

	QJsonParseError err;
	QJsonDocument json_recv = QJsonDocument::fromJson(allinfo.toUtf8(), &err);//解析json对象
	if (!json_recv.isNull())
	{
		QJsonObject object = json_recv.object();
		if (object.contains("lives"))
		{
			QJsonValue value = object.value("lives");  // 获取指定 key 对应的 value
			if (value.isArray())
			{
				QJsonObject object_data = value.toArray().at(0).toObject();


				curWeather.province = object_data.value("province").toString().toStdString();
				curWeather.city = object_data.value("city").toString().toStdString();
				curWeather.adcode = object_data.value("adcode").toString().toStdString();
				curWeather.weather = object_data.value("weather").toString().toStdString();
				curWeather.teamperature = object_data.value("temperature").toString().toInt();
				curWeather.teamperature_float = object_data.value("temperature_float").toString().toFloat();
				curWeather.winddirection = object_data.value("winddirection").toString().toStdString();
				QString windPowerStr = object_data.value("windpower").toString();
				int windPowerInt = windPowerStr.toInt();
				if (windPowerInt != 0)
				{
					curWeather.maxWindPower = windPowerInt;
				}
				else
				{
					windPowerStr = windPowerStr.mid(1);
					curWeather.maxWindPower = windPowerStr.toInt();
				}


				curWeather.maxWindPower = windPowerStr.toInt();
				curWeather.humidity = object_data.value("humidity").toString().toInt();
				curWeather.humidity_float = object_data.value("humidity_float").toString().toFloat();
				curWeather.reporttime = object_data.value("reporttime").toString().toStdString();
				emit signal_weatherIsQueryed();

				if (isLog)
				{
					qDebug() << QString::fromStdString(curWeather.province);
					qDebug() << QString::fromStdString(curWeather.city);
					qDebug() << QString::fromStdString(curWeather.adcode);
					qDebug() << QString::fromStdString(curWeather.weather);
					qDebug() << curWeather.teamperature;
					qDebug() << curWeather.teamperature_float;
					qDebug() << QString::fromStdString(curWeather.winddirection);
					qDebug() << curWeather.maxWindPower;
					qDebug() << curWeather.humidity;
					qDebug() << curWeather.humidity_float;
					qDebug() << QString::fromStdString(curWeather.reporttime);
				}
			}
		}


	}
}
