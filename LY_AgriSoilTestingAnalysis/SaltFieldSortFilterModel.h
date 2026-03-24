#ifndef SALT_FILED_SORT_FILTER_MODEL_H
#define SALT_FILED_SORT_FILTER_MODEL_H

#include <QWidget>
#include <QDateTime>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class SaltFieldSortFilterModel :public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit SaltFieldSortFilterModel(QObject* pParent = nullptr);
	virtual ~SaltFieldSortFilterModel();

	void setTimeFilterColumn(int columnIndex);

	void setFilterString(const QString& filterStr, QDateTime& startTime, QDateTime& endiTime);

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent)const override;

private:
	int m_timeFilterColumnIndex = -1;
	QString m_filterString;
	QDateTime m_startTime;
	QDateTime m_endTime;
};


#endif // SALT_FILED_SORT_FILTER_MODEL_H
