#ifndef AGRISOILANALYSISDBHANDLER_h__
#define AGRISOILANALYSISDBHANDLER_h__

#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"

using namespace AgriSoilTestingFertilizer;

class AgriSoilAnalysisDBHandler : public QObject
{
	Q_OBJECT

public:
	explicit AgriSoilAnalysisDBHandler(QObject *parent = nullptr);
	~AgriSoilAnalysisDBHandler();

public:
	QSharedPointer<QSqlQuery> getQueryOperator();

public:
	/* 测土数据集 */
	AgriSoilTestingDatasetPtr GetSoilTestingDataset(const QString& strId) const;
	AgriSoilTestingDatasetPtrVec GetSoilTestingDatasetList(const QStringList& fieldList) const;
	bool AddSoilTestingDataset(const AgriSoilTestingDatasetPtr& soilTestingDataset);
	bool UpdateSoilTestingDataset(const AgriSoilTestingDatasetPtr& soilTestingDataset);
	bool DeleteSoilTestingDataset(const QString& strId);

	/* 测土采样点 */
	AgriSoilTestingCollectionPtrVec GetSoilTestingCollectionList(const QString& sDatasetId) const;
	AgriSoilTestingCollectionPtrVec GetSoilTestingCollectionPagedData(const QString& sDatasetId, int nPage, int nPageSize) const;
	int GetSoilTestingCollectionCount(const QString& sDatasetId) const;
	bool AddSoilTestingCollection(const AgriSoilTestingCollectionPtr& soilTestingCollection);
	bool UpdateSoilTestingCollection(const AgriSoilTestingCollectionPtr& soilTestingCollection);
	bool DeleteSoilTestingCollection(const QString& strId);
	bool BatchInsertSoilTestingCollection(const AgriSoilTestingCollectionPtrVec& collectInfoVec);
	bool BatchDeleteSoilTestingCollection(const QStringList &guids);
	bool BatchDeleteSoilTestingCollection(const QString sDatasetId);

	/* 分布图 */
	AgriSoilFertilityDistributeMapPtrVec GetSoilTestingDistributeMap(const QString& sDatasetId) const;
	bool AddSoilTestingDistributeMap(const AgriSoilFertilityDistributeMapPtr& disMap);
	bool UpdateSoilTestingDistributeMap(const AgriSoilFertilityDistributeMapPtr& disMap);
	bool DeleteSoilTestingDistributeMap(const QString& sDisMapId);

	/*获取盐田的事件类型*/
	SaltFieldEventTypeInfoIDMap GetSaltFieldEventTypeIDMap() const;
	SaltFieldEventTypeInfoCodeMap GetSaltFieldEventTypeCodeMap() const;

	/*获取盐田的事件信息*/
	SaltFieldEventInfoPtr GetSaltFieldEventDataset(int id) const;
	SaltFieldEventInfoPtrVec GetSaltFieldEventDatasetList(const QStringList& fieldList) const;
	bool AddSaltFieldEventDataset(SaltFieldEventInfoPtr& eventInfo);
	bool UpdateSaltFieldEventDataset(const SaltFieldEventInfoPtr& eventInfo);
	bool DeleteSaltFieldEventDataset(int id);

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
};
#endif // AGRISOILANALYSISDBHANDLER_h__
