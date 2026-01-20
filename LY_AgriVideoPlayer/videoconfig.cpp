#include "videoconfig.h"
#include <QDebug>


std::vector<VideoInfoData> VideoInfoVec;
//std::map<int, std::string> name_map_fire{ { 0, "火" },{ 1, "smoke" } };
//std::map<int, std::string> name_rape_flower{ {0, "flower"}};
//std::map<int, std::string> name_map_pest{
//	{ 0, "maize rough dwarf disease" },
//	{ 1, "puccinia polysra" },
//	{ 2, "leaf blight of corn" },
//	{ 3, "leaf spot of corn" },
//	{ 4, "Maize sheath blight" },
//	{ 5, "Maize Head Smut" },
//	{ 6, "common smut" },
//	{ 7, "Maize stalk rot" },
//	{ 8, "Bacterial stem rot of Maize" },
//	{ 9,"gray leaf spot" },
//	{ 10,"corn brown spot" },
//	{ 11,"Alternaria alternata leaf blight of Maize" },
//	{ 12,"Curvularia leaf spot of Maize" },
//	{ 13,"Corn anthracnose" },
//	{ 14,"Corn eye spot" },
//	{ 15,"Corn take all" },
//	{ 16,"Maize Downy mildew" },
//	{ 17,"Corn dry rot" },
//	{ 18,"Maize scab" },
//	{ 19,"Maize dwarf mosaic virus disease" },
//	{ 20,"Maize stripe dwarf disease" },
//	{ 21,"Empty corn stalk" },
//	{ 22,"Corn top rot" },
//	{ 23,"aphid" },
//	{ 24,"bollworm" },
//	{ 25,"Beet armyworm" },
//	{ 26,"Spodoptera litura" },
//	{ 27,"corn borer" },
//	{ 28,"armyworm" },
//	{ 29,"Spodoptera frugiperda" },
//	{ 30,"Athetis lepigone" },
//	{ 31,"thrips" },
//	{ 32,"cutworm" },
//	{ 33,"Corn sunflower mealybug" },
//	{ 34,"Scarab Beetle" },
//	{ 35,"Oriental Migratory Locust" },
//	{ 36,"Atractomorpha sinensis" },
//	{ 37,"Miliary stink bug" }
//};

std::map<int, std::string> name_map_fire{
	{ 0, "fire" },
	{ 1, "smoke" },
};
std::map<int, std::string> name_map_front01{
	{ 0, "crop" },
};
std::map<int, std::string> name_map_Bridge{
	{ 0, "Staining-or-infiltration" },
	{ 1, "Biodegradation" },
	{ 2, "Formwork-Niche" },
	{ 3, "Exposed-reinforcement-steel" },
	{ 4, "Efflorescence" },
	{ 5, "Concrete-leaching" },
	{ 6, "Crack" },
	{ 7, "Corrison" },
};

std::map<int, std::string> name_map_CityManagement{
	{ 0, "garbage" },
	{ 1, "collapsed trees" },
	{ 2, "sewage leak" },
	{ 3, "Pothole" },
};

std::map<int, std::string> name_map_ForestFireSmokeDetection{
	{ 0, "fire" },
	{ 1, "smoke" },
};

std::map<int, std::string> name_map_InsPLAD{
	{ 0, "Spiral Damper" },
	{ 1, "Stockbridge Damper" },
	{ 2, "Glass Insulator" },
	{ 3, "Glass Insulator Big Shackle" },
	{ 4, "Glass Insulator Small Shackle" },
	{ 5, "Glass Insulator Tower Shackle" },
	{ 6, "Lightning Rod Shackle" },
	{ 7, "Lightning Rod Suspension" },
	{ 8, "Tower ID Plate" },
	{ 9, "Polymer Insulator" },
	{ 10, "Polymer Insulator Lower Shackle" },
	{ 11, "Polymer Insulator Upper Shackle" },
	{ 12, "Polymer Insulator Tower Shackle" },
	{ 13, "Spacer" },
	{ 14, "Vari-grip" },
	{ 15, "Yoke" },
	{ 16, "Yoke Suspension" },
	{ 17, "sphere" },
};

std::map<int, std::string> name_map_RoadDisaster{
	{ 0, "landslide" },
	{ 1, "fallen tree" },
	{ 2, "stone" },
	{ 3, "road collapse" },
};

std::vector <QString> ModelVec{"","fire" , /* "flower" ,"pest",*/"Bridge","CityManagement","ForestFireSmokeDetection","InsPLAD" ,"RoadDisaster","fire","front01"};

ModelType getModelTypeWithValue(int value)
{
	switch (value)
	{
	case 0:return ModelType_NONE;
	case 1: return ModelType_FIRE;
	//case 2: return ModelType_FLOWER;
	//case 3: return ModelType_PEST;
	//case 4: return ModelType_NORMAL;
	case 2: return ModelType_Bridge;
	case 3: return ModelType_CityManagement;
	case 4: return ModelType_ForestFireSmokeDetection;
	case 5: return ModelType_InsPLAD;
	case 6: return ModelType_RoadDisaster;
	case 7: return ModelType_fire;
	case 8: return ModelType_front01;
	default:
		break;
	}
}
VideoConfig::VideoConfig(QObject *parent) : QObject(parent)
{
	readXMLFile();
}

void  VideoConfig::readXMLFile()
{
	VideoInfoVec.clear();
	TiXmlDocument docElement;
	std::string filename = "../data/VideoPlayerConfig.xml";
	VideoInfoData videoInfo;
	if (!docElement.LoadFile(filename.c_str()))
	{
		qDebug() << "open xml fail";
		return ;
	}
	TiXmlElement* pRootEle = docElement.RootElement();
	if (!pRootEle)
	{
		return ;
	}
	TiXmlElement* pRecordListEle = pRootEle->FirstChildElement("VideoPlayer");
	if (!pRecordListEle)
	{
		return ;
	}
//	int childCount = 0;
	TiXmlElement* pChild = pRecordListEle->FirstChildElement();

	// 遍历所有直接子元素
	
	if (pChild) {
		//childCount++;

		// 获取子元素名称
		//const char* tagName = pChild->Value();

		TiXmlElement* pRecordEle = pRecordListEle->FirstChildElement();
		while (pRecordEle) {
			const char* _id = pRecordEle->Attribute("id");
			const char* _url = pRecordEle->Attribute("url");
			VideoInfoData infoData;
			infoData.id = std::atoi(_id);
			//infoData.name = tagName;
			infoData.url = _url;

			VideoInfoVec.push_back(infoData);
			pRecordEle = pRecordEle->NextSiblingElement();

		}
		

		pChild = pChild->NextSiblingElement();
	}
}