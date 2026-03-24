#pragma once

#include <QObject>
#include <QAbstractTableModel>
#include "IScenariosReplayThread.h"
#include <QWidget>
#include <iostream>
#include <QStandardItemModel>

using namespace std;
class ISCenairosReplayModel : public QStandardItemModel
{
	Q_OBJECT

public:
	ISCenairosReplayModel(QObject *parent = nullptr);
	~ISCenairosReplayModel();

	int rowCount(const QModelIndex &parent = QModelIndex())const override;
	int columnCount(const QModelIndex &parent = QModelIndex())const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

	void SetDeleteList(QList<int> i_List);

	void SetHeadData(QStringList i_list);

	void SetModelData(QList<LogDataInfo> model);

private:
	QList<LogDataInfo> modelData;
	QStringList headeList;
	QList<int> m_DeleteList;

};
