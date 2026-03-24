#ifndef AGRIMACHINEDAILYPRODUCTREPORT_H_
#define AGRIMACHINEDAILYPRODUCTREPORT_H_
 
#include <QWidget>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

namespace Ui
{
	class AgriMachineDailyProductReport;
}

class AgriMachineDailyProductReport : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMachineDailyProductReport(QWidget *parent = nullptr);
	~AgriMachineDailyProductReport();
public:

private:
	Ui::AgriMachineDailyProductReport *ui;
};

#endif  // AgriMachineDailyProductReport_H