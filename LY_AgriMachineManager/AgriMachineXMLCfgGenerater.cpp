#include <QFile>
#include <QDebug>
#include <QXmlStreamWriter>

#include "LY_AgriMachineManager.h"
#include "AgriMachineXMLCfgGenerater.h"
#include "AgriMachineManagerUtil.h"


AgriMachineXMLCfgGenerater::AgriMachineXMLCfgGenerater(QObject *parent /*= nullptr*/)
{

}

AgriMachineXMLCfgGenerater::~AgriMachineXMLCfgGenerater()
{

}

bool AgriMachineXMLCfgGenerater::generateManchineXmlCfg(const QString& filePath)
{
	QFile file(filePath);
	if (file.exists()) {
		qDebug() << "File already exists." << filePath;
		return false;
	}
	else {
		if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
			qDebug() << "File does not exist. Created and opened successfully:" << filePath;
		}
		else {
			qDebug() << "Failed to create and open file:" << filePath;
			return false;
		}
	}

	QXmlStreamWriter xmlWriter(&file);
	xmlWriter.setCodec("UTF-8");

	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();

	xmlWriter.writeStartElement("configuration");

	rttr::type base_type = rttr::type::get<AgriMachineBasicInfo>();
	rttr::array_range<rttr::type> derived_classes = base_type.get_derived_classes();

	for (const auto& derived : derived_classes)
	{
		xmlWriter.writeStartElement(QString::fromStdString(derived.get_name().to_string()));
		// 遍历结构体的属性
		for (auto& prop : derived.get_properties()) {
			xmlWriter.writeStartElement("field");
			const std::string tmpName = prop.get_name().to_string();
			xmlWriter.writeAttribute("name", QString::fromLocal8Bit(tmpName.c_str()));

			// 获取元数据中的注释
			rttr::variant display = prop.get_metadata("display");
			if (display.is_valid()) 
			{
				xmlWriter.writeAttribute("display", QString::fromLocal8Bit(display.to_string().c_str()));
			}
			rttr::variant unit = prop.get_metadata("unit");
			if (unit.is_valid())
			{
				xmlWriter.writeAttribute("unit", QString::fromLocal8Bit(unit.to_string().c_str()));
			}
			rttr::variant isEnum = prop.get_metadata("isEnum");
			if (isEnum.is_valid())
			{
				//xmlWriter.writeAttribute("isEnum", unit.to_bool() ? "true" : "false");
				AgriEnumInfoVec enumVec = LY_AgriMachineManager::Get().GetpDbHandler()->FetchMachineEnumInfoList(tmpName);
				QString enumStr = AgriMachineManagerUtil::formatEnumToStr(enumVec);
				xmlWriter.writeAttribute("enums", enumStr);
			}
			
			// 默认可见
			xmlWriter.writeAttribute("visible", "true");
			xmlWriter.writeEndElement();
		}
		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement(); // configuration
	xmlWriter.writeEndDocument();

	file.close();

	return true;
}