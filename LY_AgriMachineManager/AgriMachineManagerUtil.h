#ifndef AGRIMACHINEMANAGERUTIL_H_
#define AGRIMACHINEMANAGERUTIL_H_

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

namespace AgriMachineManagerUtil
{
	// 按照规则解析机器信息表名 [uuid = 表名 + id]
	QString getTablenameByGuid(const QString& strGuid);

	// 按照规则解析机器枚举表名 [表名 = "agri_enum_machine" + {key}]
	QString getMachineEnumTablenameByKey(const QString& strKey);

	// 根据RTTR 获取结构体属性信息，生成默认XML配置
	void generateDefaultXmlConfiguration(const rttr::type& objType, const std::string& fileName = "machineDetailsConfig.xml");

	// 格式化枚举列表为字符串 [id_1:description_1, id_2:description_2, ...]
	QString formatEnumToStr(const AgriEnumInfoVec& vecEnum);

	// 解析枚举字符串查找对应值
	QString FetchValueByEnumStr(const QString& strEnums, const QString& strEnumId);

}

#endif  // AgriMachineManagerUtil_H