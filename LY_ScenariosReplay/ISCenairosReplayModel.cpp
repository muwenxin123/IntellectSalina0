#include "ISCenairosReplayModel.h"
#include <qdebug.h>
#include <QElapsedTimer>

ISCenairosReplayModel::ISCenairosReplayModel(QObject *parent)
{

}

ISCenairosReplayModel::~ISCenairosReplayModel()
{
}
int ISCenairosReplayModel::rowCount(const QModelIndex &parent) const
{
	return modelData.size();
}

int ISCenairosReplayModel::columnCount(const QModelIndex &parent) const
{
	return headeList.size();
}

QVariant ISCenairosReplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		if (section < headeList.size())
		{
			return headeList[section];
		}
	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

bool ISCenairosReplayModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return 0;
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const int row = index.row();
		switch (index.column())
		{
		case 0: modelData[row].ucType = value.toInt(); break;
		case 1:modelData[row].ucType = value.toInt(); break;
		case 2:modelData[row].ucType = value.toInt(); break;
		case 3:modelData[row].ucType = value.toInt(); break;
		case 4:modelData[row].ucType = value.toInt(); break;
		case 5:modelData[row].ucType = value.toInt(); break;
		case 6:modelData[row].ucType = value.toInt(); break;
		case 7:modelData[row].ucType = value.toInt(); break;

		}
		emit dataChanged(index, index, QVector<int>() << role);
		return true;
	}
	return false;
}

bool ISCenairosReplayModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (row < 0 || count<1 || row + count>rowCount())
		return false;

	beginRemoveRows(parent, row, row + count - 1);

	if (m_DeleteList.size() != 0)
	{
		for (int i = 0; i < m_DeleteList.size(); i++)
		{
		}
	}

	m_DeleteList.clear();
	endRemoveRows();

	return true;
}

void ISCenairosReplayModel::SetDeleteList(QList<int> i_List)
{
	m_DeleteList = i_List;
}

QVariant ISCenairosReplayModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const int row = index.row();
		switch (index.column())
		{
		case 0: return modelData.at(row).ucType;
		case 1:return modelData.at(row).ucType;
		case 2:return modelData.at(row).ucType;
		case 3:return modelData.at(row).ucType;
		case 4:return modelData.at(row).ucType;
		case 5:return modelData.at(row).ucType;
		case 6:return modelData.at(row).ucType;
		case 7:return modelData.at(row).ucType;
		}
	}
	return QVariant();
}

void ISCenairosReplayModel::SetHeadData(QStringList i_list)
{
	headeList = i_list;
}

void ISCenairosReplayModel::SetModelData(QList<LogDataInfo> model)
{
	modelData = model;
}