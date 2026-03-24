#ifndef AGRIMULTISPECTRALSENSORUTIL_H_
#define AGRIMULTISPECTRALSENSORUTIL_H_
 
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

#include "AgriMultispectralSensorDecl.h"

const double SQM_PER_MU = 666.6667; // 1 亩 = 666.6667 平方米

using json = nlohmann::json;

namespace AgriMultispectralSensorUtil
{
	/* json 序列化*/
	json SerializeToJson(const rttr::instance& obj);

	/* json 反序列化*/
	template <typename T>
	T DeserializeFromJson(const json& j)
	{
		T obj;
		rttr::type type = rttr::type::get<T>();
		for (auto& prop : type.get_properties())
		{
			const std::string propertyName = prop.get_name().to_string();
			auto it = j.find(propertyName);
			if (it != j.end())
			{
				rttr::variant value;
				if (prop.get_type() == rttr::type::get<int>())
				{
					value = it->get<int>();
				}
				else if (prop.get_type() == rttr::type::get<float>())
				{
					value = it->get<float>();
				}
				else if (prop.get_type() == rttr::type::get<double>())
				{
					value = it->get<double>();
				}
				else if (prop.get_type() == rttr::type::get<std::string>())
				{
					value = it->get<std::string>();
				}
				else if (prop.get_type() == rttr::type::get<bool>()) {
					value = it->get<bool>();
				}

				if (value.is_valid()){
					prop.set_value(obj, value);
				}
				else{
					// warning
				}
			}
		}

		return obj;
	}

	/* 倾斜摄影数据添加一个osgb格式的索引文件，生成后就可以通过OSG直接加载整个倾斜摄影模型数据。*/
	void CreateObliqueIndexes(std::string fileDir);

	/* QString转QJsonObject*/
	QJsonObject StringToJson(const QString jsonString);

	/* QJson转QString*/
	QString JsonToString(const QJsonObject& jsonObject);

	/* 将 ISO 8601 格式（2024-10-23T03:05:19.000）字符串转换为 "yyyyMMddTHHmmss" 格式*/
	QString FormatToCompactISO(const QString& isoInput);

	void AddModelsToEarth(const QString &tilesDirPath, const QString &earthFilePath, const QString &urlPrefix);

	/* 计算多边形重心 */
	bool CalcPolygonCenterPoint(const QVector<QPointF>& zonePoints, QPointF& point);

	/* 检测多边形包含关系 */
	bool IsPolygonCompletelyInside(const QPolygonF& outer, const QPolygonF& inner);

	// **计算交集**
	QPolygonF CalculateIntersection(const QPolygonF& polygon1, const QPolygonF& polygon2);

	/* 计算多边形区域面积 */
	bool CalcPolygonArea(const QVector<QPointF>& zonePoints, double& area);

	/* 平方米转亩 */
	inline double sqmToMu(double sqm) { return sqm / SQM_PER_MU; }

	/* 亩转平方米 */
	inline double muToSqm(double mu) { return mu * SQM_PER_MU; }
}
#endif  // AgriMultispectralSensorUtil_H