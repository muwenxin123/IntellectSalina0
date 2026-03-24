#ifndef AGRIDATAANALYSISDBHANDLER_H_
#define AGRIDATAANALYSISDBHANDLER_H_
 
#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "AgriMultispectralSensorDecl.h"


class AgriMultispectralSensorDBHandler : public QObject
{
	Q_OBJECT

public:
	explicit AgriMultispectralSensorDBHandler(QObject *parent = nullptr);
	~AgriMultispectralSensorDBHandler();

public:
	QSharedPointer<QSqlQuery> getQueryOperator();

public:
	/* 卫星数据 */
	AgriSentinelMetadataPtrVec GetSentinelMetadataList() const;
	bool AddSentinelMetadata(const AgriSentinelMetadataPtr& pestDataset);
	bool UpdateSentinelMetadata(const AgriSentinelMetadataPtr& pestDataset);
	bool DeleteSentinelMetadata(const QString& strId);

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

#endif  // AgriMultispectralSensorDBHandler_H