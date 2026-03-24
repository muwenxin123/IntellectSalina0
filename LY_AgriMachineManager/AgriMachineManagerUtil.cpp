
#include <rttr/registration>
#include <rttr/type>
#include <QFile>
#include <QXmlStreamWriter>
#include <iostream>

#include "AgriMachineManagerUtil.h"

QString AgriMachineManagerUtil::getTablenameByGuid(const QString& strGuid)
{
	int lastUnderscoreIndex = strGuid.lastIndexOf('_');
	return strGuid.left(lastUnderscoreIndex);
}

QString AgriMachineManagerUtil::getMachineEnumTablenameByKey(const QString& strKey)
{
	return AGRI_ENUM_MACHINE_PREFIX + strKey;
}

void AgriMachineManagerUtil::generateDefaultXmlConfiguration(const rttr::type& objType, const std::string& fileName/* = "machineDetailsConfig.xml"*/)
{
	QFile file(QString::fromStdString(fileName));
	if (!file.open(QIODevice::WriteOnly)) {
		std::cerr << "文件打开失败: " << fileName << std::endl;
		return;
	}

	QXmlStreamWriter xmlWriter(&file);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();

	xmlWriter.writeStartElement("configuration");

	for (auto& prop : objType.get_properties()) {
		xmlWriter.writeStartElement("field");
		xmlWriter.writeAttribute("name", QString::fromStdString(prop.get_name().to_string()));
		xmlWriter.writeAttribute("display_name", QString::fromStdString(prop.get_name().to_string())); // 默认显示名与字段名相同
		xmlWriter.writeAttribute("visible", "true"); // 默认显示
		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement();
	xmlWriter.writeEndDocument();

	file.close();
	std::cout << "XML配置生成成功: " << fileName << std::endl;
}

QString AgriMachineManagerUtil::formatEnumToStr(const AgriEnumInfoVec& vecEnum)
{
	QString result;

	for (size_t i = 0; i < vecEnum.size(); ++i) {
		const AgriEnumInfo& enumInfo = vecEnum[i];

		// "id:description"
		result += QString::number(enumInfo.id) + ":" + QString::fromStdString(enumInfo.description);

		if (i < vecEnum.size() - 1) {
			result += ",";
		}
	}

	return result;
}

QString AgriMachineManagerUtil::FetchValueByEnumStr(const QString& strEnums, const QString& strEnumId)
{
	QMap<QString, QString> enumMap;

	QStringList enumPairs = strEnums.split(",");

	for (const QString& pair : enumPairs) {
		QStringList keyValue = pair.split(":");
		if (keyValue.size() == 2) {
			QString enumId = keyValue[0];
			enumMap[enumId] = keyValue[1];
		}
	}

	if (enumMap.contains(strEnumId)) {
		return enumMap[strEnumId];
	}
	else {
		return strEnumId;
	}
}

