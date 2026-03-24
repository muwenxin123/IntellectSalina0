#ifndef _READANNWRITEXML_
#define _READANNWRITEXML_

#include "TinyXML/tinyxml2.h"
#include <QPointF>
#include <vector>
#include <string>
#include <map>

void WriteNoFlyArea2File(const char* ofileName, const std::vector<std::vector<QPointF>> areas)
{
	tinyxml2::XMLDocument* Doc = new tinyxml2::XMLDocument;
	int res = Doc->LoadFile(ofileName);
	if (res != 0)
	{
		const char* declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
		Doc->Parse(declaration);
	}
	else
	{
		Doc->Clear();
	}

	tinyxml2::XMLElement* rootElement = Doc->NewElement("AllAreas");
	Doc->InsertEndChild(rootElement);

	for (int AreaIndex = 0; AreaIndex < areas.size(); AreaIndex++)
	{
		tinyxml2::XMLElement* Area = Doc->NewElement("Area");
		Area->SetAttribute("Index", std::to_string(AreaIndex + 1).c_str());
		rootElement->InsertEndChild(Area);
		std::vector<QPointF> points = areas[AreaIndex];
		for (int i = 0; i < points.size(); i++)
		{
			tinyxml2::XMLElement* tmpP = Doc->NewElement("Point");
			tmpP->SetAttribute("Index", i + 1);
			tmpP->SetAttribute("Longitude", points[i].x());
			tmpP->SetAttribute("Latitude", points[i].y());
			Area->InsertEndChild(tmpP);
		}
	}
	Doc->SaveFile(ofileName);
	Doc->~XMLDocument();
}

void WriteKeyArea2File(const char* ofileName, const std::vector<std::vector<QPointF>> areas,
						const std::map<int, std::pair<bool, int>> keyAreaNum)
{
	tinyxml2::XMLDocument* Doc = new tinyxml2::XMLDocument;
	int res = Doc->LoadFile(ofileName);
	if (res != 0)
	{
		const char* declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
		Doc->Parse(declaration);
	}
	else
	{
		Doc->Clear();
	}

	tinyxml2::XMLElement* rootElement = Doc->NewElement("AllAreas");
	Doc->InsertEndChild(rootElement);

	for (int AreaIndex = 0; AreaIndex < areas.size(); AreaIndex++)
	{
		auto KeyAreaItor = keyAreaNum.find(AreaIndex);
		int areaNum = -1;
		if (KeyAreaItor != keyAreaNum.end())
		{
			areaNum = KeyAreaItor->second.second;
		}

		tinyxml2::XMLElement* Area = Doc->NewElement("Area");
		Area->SetAttribute("Index", AreaIndex + 1);
		Area->SetAttribute("KeyNum", areaNum);
		rootElement->InsertEndChild(Area);
		std::vector<QPointF> points = areas[AreaIndex];
		for (int i = 0; i < points.size(); i++)
		{
			tinyxml2::XMLElement* tmpP = Doc->NewElement("Point");
			tmpP->SetAttribute("Index", i + 1);;
			tmpP->SetAttribute("Longitude", points[i].x());
			tmpP->SetAttribute("Latitude", points[i].y());
			Area->InsertEndChild(tmpP);
		}
	}
	Doc->SaveFile(ofileName);
	Doc->~XMLDocument();
}

std::vector<std::vector<QPointF>> readNoFlyAreaFromFile(const char*iFileName)
{
	std::vector<std::vector<QPointF>> returnVec;
	returnVec.clear();
	tinyxml2::XMLDocument* Doc = new tinyxml2::XMLDocument;
	int res = Doc->LoadFile(iFileName);
	if (res != 0)
	{
		Doc->~XMLDocument();
		return returnVec;
	}
	else
	{
		Doc->SaveFile(iFileName);
		tinyxml2::XMLElement* rootElement = Doc->RootElement();

		tinyxml2::XMLElement* ele = rootElement->FirstChildElement();
		while (ele != NULL)
		{
			std::vector<QPointF> points;
			points.clear();
			tinyxml2::XMLElement* pointEle = ele->FirstChildElement();
			while (pointEle != NULL)
			{
				double Lon = pointEle->DoubleAttribute("Longitude");
				double lat = pointEle->DoubleAttribute("Latitude");
				points.emplace_back(QPointF(Lon, lat));
				pointEle = pointEle->NextSiblingElement();
			}
			returnVec.emplace_back(points);
			ele = ele->NextSiblingElement();
		}
		Doc->~XMLDocument();
		return returnVec;
	}
}

std::vector<std::vector<QPointF>> readKeyAreaFromFile(const char*iFileName, std::vector<int>& KeyNums)
{
	KeyNums.clear();
	std::vector<std::vector<QPointF>> returnVec;
	returnVec.clear();
	tinyxml2::XMLDocument* Doc = new tinyxml2::XMLDocument;
	int res = Doc->LoadFile(iFileName);
	if (res != 0)
	{
		Doc->~XMLDocument();
		return returnVec;
	}
	else
	{
		Doc->SaveFile(iFileName);
		tinyxml2::XMLElement* rootElement = Doc->RootElement();

		tinyxml2::XMLElement* ele = rootElement->FirstChildElement();
		while (ele != NULL)
		{
			int keyNum = ele->IntAttribute("KeyNum");
			KeyNums.emplace_back(keyNum);
			std::vector<QPointF> points;
			points.clear();
			tinyxml2::XMLElement* pointEle = ele->FirstChildElement();
			while (pointEle != NULL)
			{
				double Lon = pointEle->DoubleAttribute("Longitude");
				double lat = pointEle->DoubleAttribute("Latitude");
				points.emplace_back(QPointF(Lon, lat));
				pointEle = pointEle->NextSiblingElement();
			}
			returnVec.emplace_back(points);
			ele = ele->NextSiblingElement();
		}
		Doc->~XMLDocument();
		return returnVec;
	}
}

#endif

