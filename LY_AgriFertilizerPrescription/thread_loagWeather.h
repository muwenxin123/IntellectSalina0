#pragma once

#include <qobject.h>
#include <qthread.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <qstring.h>
#include <QTimer>
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
/*
"province" :"北京",
"city" :"东城区",
"adcode" :"110101",
"weather" :"多云",
"temperature" :"5",
"winddirection" :"南",
"windpower" :"≤3",
"humidity" :"31",
"reporttime" :"2024-12-04 10:37:17",
"temperature_float" :"5.0",
"humidity_float" :"31.0"
* */
struct weatherData
{
	std::string province;
	std::string city;
	std::string adcode;
	std::string weather;
	int         teamperature;
	float       teamperature_float;
	std::string winddirection;
	int         maxWindPower;
	int         humidity;
	float       humidity_float;
	std::string reporttime;
};
struct CityConfig
{
	std::string  city;
	
	std::string  code;
};

class thread_loagWeather : public QObject
{
	Q_OBJECT
public:
	thread_loagWeather();
	~thread_loagWeather();

	void working();

	void setEnd();

	void setCityCode(QString _cityCode);

	weatherData getCurWeather();

private:
	void queryWeather();//查询

private slots :
	void replyFinished(QNetworkReply *reply);//刷新的槽

protected:
	//void run();

signals:
	void signal_weatherIsQueryed();

private:

	AgriFarmInfoVec  GetFarmInfoList() const;
	weatherData curWeather;

	QTimer *m_timer;

	QNetworkAccessManager* manager = NULL;

	QString allinfo;       //所有信息

	QString  cityCode = "110101";
	bool isGetData = false;//是否成功获取数据

	bool isWorking = true;
	bool isLog = false;
	std::string code;
	std::vector<CityConfig> m_cityVec;

};

