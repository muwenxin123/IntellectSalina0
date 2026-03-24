#ifndef AGRIDATAANALYSISDBHANDLER_H_
#define AGRIDATAANALYSISDBHANDLER_H_
 
#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "AgriDataAnalysisPredictDecl.h"

class AgriDataAnalysisDBHandler : public QObject
{
	Q_OBJECT

public:
	explicit AgriDataAnalysisDBHandler(QObject *parent = nullptr);
	~AgriDataAnalysisDBHandler();

public:
	QSharedPointer<QSqlQuery> getQueryOperator();

public:
	/* 数据集 */
	AgriPestDatasetPtrVec GetPestDatasetList(const QStringList& fieldList) const;
	bool AddPestDataset(const AgriPestDatasetPtr& pestDataset);
	bool UpdatePestDataset(const AgriPestDatasetPtr& pestDataset);
	bool DeletePestDataset(const QString& strId);

	/* 采集点 */
	AgriPestCollectionPtrVec GetPestCollectionList(const QString& sDatasetId) const;
	AgriPestCollectionPtrVec GetPestCollectionPagedData(const QString& sDatasetId, int nPage, int nPageSize) const;
	int GetPestCollectionCount(const QString& sDatasetId) const;
	bool AddPestCollection(const AgriPestCollectionPtr& pestCollection);
	bool UpdatePestCollection(const AgriPestCollectionPtr& pestCollection);
	bool DeletePestCollection(const QString& strId);

	/* 采集图像 */
	AgriPestPhotoIndeficationPtrVec GetPestPhotoIndeficationList(const QString& sFieldId)const;

	/* 分布图 */
	AgriPestDistributeMapPtrVec GetPestDistributeMap(const QString& sDatasetId) const;
	bool AddPestDistributeMap(const AgriPestDistributeMapPtr& pestDisMap);
	bool UpdatePestDistributeMap(const AgriPestDistributeMapPtr& pestDisMap);
	bool DeletePestDistributeMap(const QString& sDisMapId);

	/* Test数据导入 */
	bool BatchInsertPestCollection(const AgriPestCollectionPtrVec& collectInfoVec);

private:
	void InitDataBase();
	void CloseDatabase();

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
	QString						connectionName;
};

#endif  // AgriDataAnalysisDBHandler_H