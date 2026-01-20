#ifndef VIDEOCONFIG_H
#define VIDEOCONFIG_H

#include <QObject>
#include <map>
#include "TinyXML/tinyxml.h"

 enum ModelType
{
	ModelType_NONE = 0,
	ModelType_FIRE ,
	ModelType_FLOWER,
	ModelType_PEST,
	ModelType_front01,
	ModelType_fire,
	ModelType_NORMAL,
	ModelType_Bridge,
	ModelType_CityManagement,
	ModelType_ForestFireSmokeDetection,
	ModelType_InsPLAD,
	ModelType_RoadDisaster,
};
extern std::vector <QString> ModelVec;

struct VideoInfoData
{
	int  id = 0;
	QString url = "";
	QString name = "";
	int model_type = 0;
	bool isOpen = true;
};
Q_DECLARE_METATYPE(VideoInfoData)

ModelType getModelTypeWithValue(int value);


extern std::vector<VideoInfoData> VideoInfoVec;
////虫模型名
//extern  std::map<int, std::string> name_map_pest;
////火模型名
extern  std::map<int, std::string> name_map_fire;
////油菜花模型
//extern  std::map<int, std::string> name_rape_flower;
//烟火模型
extern  std::map<int, std::string> name_map_fire;
//玉米田前视模型
extern  std::map<int, std::string> name_map_front01;
//桥梁检测模型
extern  std::map<int, std::string> name_map_Bridge;
//城市管理模型
extern  std::map<int, std::string> name_map_CityManagement;
//森林烟火检测模型
extern  std::map<int, std::string> name_map_ForestFireSmokeDetection;
//电网无人机检测模型
extern  std::map<int, std::string> name_map_InsPLAD;
//道路灾害模型
extern  std::map<int, std::string> name_map_RoadDisaster;

class VideoConfig : public QObject
{
    Q_OBJECT
public:
    explicit VideoConfig(QObject *parent = nullptr);

	

private:

	void readXMLFile();

private:



signals:



public slots:
};

#endif // VIDEOCONFIG_H