#ifndef AGRIMACHINEDATABASEHANDLER_H_
#define AGRIMACHINEDATABASEHANDLER_H_
 
#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "AgriFarmManagerUtil.h"

class AgriFarmDatabaseHandler : public QObject
{
	Q_OBJECT

public:
	explicit AgriFarmDatabaseHandler(QObject *parent = nullptr);
	~AgriFarmDatabaseHandler();

public:
	QSharedPointer<QSqlQuery> getQueryOperator();

public:
	/* 农场 */

	AgriFarmInfoVec GetFarmInfoList() const;
	bool AddFarmInfo(const AgriFarmInfo& farmInfo);
	bool DeleteFarmInfo(const QString& strId);


	/* 盐田 */

	AgriFieldInfoVec GetFieldInfoList() const;
	bool AddFieldInfo(const AgriFieldInfo& fieldInfo);
	bool UpdateFieldInfo(const AgriFieldInfo& fieldInfo);
	bool BatchInsertField(const AgriFieldInfoVec& fieldInfoVec);
	bool DeleteFieldInfo(const QString& strId);


	/* 传感器 */

	AgriMonitorInfoPtrVec GetMonitorInfoList();
	AgriMonitorInfoPtrVec GetRelateSensorList(const QString& fieldId);
	bool GetFieldSensorRelation(const QString& fieldId, std::vector<int>& sensorIds);
	bool UpdateFieldSensorRelation(const QString& fieldId, const std::vector<int>& delSensorIdVec, const std::vector<int>& addSensorIdVec);


	/* 种子 */

	AgriCropInfoPtrVec GetCropInfoList() const;
	AgriCropVarietyInfoPtrVec GetCropVarietyInfoList() const;


	/* 枚举信息 */

	AgriEnumInfoPtrVec FetchCropPlantStandardEnumInfoList();
	AgriEnumInfoPtrVec FetchCropSowModelEnumInfoList();


	/* 资源图片 */

	QByteArray GetAgriFarmImage(const std::string& strGuid);

private:
	AgriEnumInfoPtrVec FetchEnumInfoList(const std::string& strKey);

private:
	void InitDataBase();
	// 执行查询（INSERT, UPDATE, DELETE）
	bool executeQuery(const QString &queryStr);
	// 执行 SELECT 查询并返回结果
	QSqlQuery executeSelectQuery(const QString &queryStr);
	// 开始事务
	bool beginTransaction();
	// 提交事务
	bool commitTransaction();
	// 回滚事务
	bool rollbackTransaction();

private:
	QSqlDatabase				db;
	mutable QMutex  			sMutex;
	bool						isOpen;

	AgriEnumInfoPtrVec			m_CropPlantStandard;
	AgriEnumInfoPtrVec			m_SowModel;

	AgriMonitorInfoPtrVec		m_vecMonitorInfo;
};

#endif  // AgriFarmDatabaseHandler_H