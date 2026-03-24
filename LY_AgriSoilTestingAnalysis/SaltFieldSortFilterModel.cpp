#include "SaltFieldSortFilterModel.h"
#include "CommDelegate.h"

SaltFieldSortFilterModel::SaltFieldSortFilterModel(QObject* pParent /*= nullptr*/)
	:QSortFilterProxyModel(pParent), m_filterString("")
{

}

SaltFieldSortFilterModel::~SaltFieldSortFilterModel()
{

}

void SaltFieldSortFilterModel::setTimeFilterColumn(int columnIndex)
{
	m_timeFilterColumnIndex = columnIndex;
}

void SaltFieldSortFilterModel::setFilterString(const QString& filterStr, QDateTime& startTime, QDateTime& endTime)
{
	m_filterString = filterStr;
	m_startTime = startTime;
	m_endTime = endTime;
	invalidateFilter();
}

bool SaltFieldSortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent)const
{
	if (m_filterString.isEmpty() && (!m_startTime.isValid() || !m_endTime.isValid()))
		return true;

	QModelIndex index;
	const QAbstractItemModel* pModel = sourceModel();

	bool isTimeValid = true;
	if (m_startTime.isValid() && m_endTime.isValid() && m_timeFilterColumnIndex > 0)
	{
		isTimeValid = false;
		index = pModel->index(source_row, m_timeFilterColumnIndex, source_parent);
		QString data = pModel->data(index, filterRole()).toString();
		QDateTime time = pModel->data(index, DELEGATE_DATA_ROLE).toDateTime();
		if (time >= m_startTime && time <= m_endTime)
		{
			isTimeValid = true;
		}
	}

	if (isTimeValid && !m_filterString.isEmpty())
	{
		for (int col = 0; col < pModel->columnCount(); col++)
		{
			index = pModel->index(source_row, col, source_parent);
			QString data = pModel->data(index, filterRole()).toString();
			if (!m_filterString.isEmpty() && data.contains(m_filterString, Qt::CaseInsensitive))
				return true;
		}
	}
	else if(isTimeValid) {
		return true;
	}

	return false;
}
