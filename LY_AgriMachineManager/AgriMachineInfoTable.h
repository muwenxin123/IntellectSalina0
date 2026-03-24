#ifndef AGRIMACHINEINFOTABLE_h__
#define AGRIMACHINEINFOTABLE_h__

#include <QWidget>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"


namespace Ui
{
	class AgriMachineInfoTable;
}

class AgriMachineInfoTable : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMachineInfoTable(QWidget *parent = nullptr);
	~AgriMachineInfoTable();

	void UpdateTableData(AgriMachineInfoPtrVec machineList);

private:
	void InitTable();
	void AddOneRowTotable(AgriMachineInfoPtr machineInfo);
	void UpdateOneRowTotable(AgriMachineInfoPtr machineInfo);

private:
	Ui::AgriMachineInfoTable *ui;
};
#endif // AGRIMACHINEINFOTABLE_h__
