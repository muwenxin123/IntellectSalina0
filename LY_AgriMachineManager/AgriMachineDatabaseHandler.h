#ifndef AGRIMACHINEDATABASEHANDLER_H_
#define AGRIMACHINEDATABASEHANDLER_H_
 
#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"
#include "AgriMachineManagerUtil.h"

class AgriMachineDatabaseHandler : public QObject
{
	Q_OBJECT

public:
	explicit AgriMachineDatabaseHandler(QObject *parent = nullptr);
	~AgriMachineDatabaseHandler();

public:
	QSharedPointer<QSqlQuery> getQueryOperator();

	AgriDirMachineInfoPtrVec GetDirMachineInfoList() const;

	AgriMachineInfoPtrVec GetMachineInfoList();

	QByteArray GetAgriMachineImage(const std::string& strGuid);

	// 查询机器信息
	template <typename T>
	T FetchMachineInfo(const std::string& strGuid)
	{
		QString strMachTablename = AgriMachineManagerUtil::getTablenameByGuid(QString::fromStdString(strGuid));

		T result;
		QSqlQuery query(db);

		query.prepare("SELECT * FROM " + strMachTablename + " WHERE guid = :strGuid");
		query.bindValue(":strGuid", QString::fromStdString(strGuid));

		if (!query.exec())
		{
			return result;
		}

		if (query.next())
		{
			QSqlRecord record = query.record();
			result = MapRowToStruct<T>(record);
		}

		return result;
	};

	// 使用RTTR动态映射数据库行到结构体
	template <typename T>
	T MapRowToStruct(const QSqlRecord& record)
	{
		T instance;
		rttr::type t = rttr::type::get<T>();
		for (auto& prop : t.get_properties())
		{
			const std::string propertyName = prop.get_name().to_string();
			int index = record.indexOf(QString::fromStdString(propertyName));
			if (index != -1)
			{
				QVariant value = record.value(index);
				if (value.isValid())
				{
					if (prop.get_type() == rttr::type::get<int>())
					{
						prop.set_value(instance, value.toInt());
					}
					else if (prop.get_type() == rttr::type::get<double>())
					{
						prop.set_value(instance, value.toDouble());
					}
					else if (prop.get_type() == rttr::type::get<QString>())
					{
						prop.set_value(instance, value.toString());
					}
					else if (prop.get_type() == rttr::type::get<std::string>())
					{
						prop.set_value(instance, value.toString().toStdString());
					}
					else if (prop.get_type() == rttr::type::get<bool>()) {
						prop.set_value(instance, value.toBool());
					}
				}
			}
		}
		return instance;
	};

	// 查询机器枚举列表
	AgriEnumInfoVec FetchMachineEnumInfoList(const std::string& strKey);

private:
	void InitDataBase();
	void LoadDirDataBase();
	AgriMachineBasicInfoPtr MatchComponents(QString componentId);

private:
	QSqlDatabase				db;
	mutable QMutex  			sMutex;
	AgriDirMachineInfoPtrVec	m_AgriDirMachineInfoPtrVec;

	bool			isOpen;
};

#endif  // AgriMachineDatabaseHandler_H