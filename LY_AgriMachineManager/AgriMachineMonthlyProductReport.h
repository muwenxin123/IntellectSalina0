#ifndef AGRIMACHINEMONTHLYPRODUCTREPORT_H_
#define AGRIMACHINEMONTHLYPRODUCTREPORT_H_
 
#include <QWidget>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

namespace Ui
{
	class AgriMachineMonthlyProductReport;
}

class AgriMachineMonthlyProductReport : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMachineMonthlyProductReport(QWidget *parent = nullptr);
	~AgriMachineMonthlyProductReport();
public:

private:
	Ui::AgriMachineMonthlyProductReport *ui;
};
#endif  // AgriMachineMonthlyProductReport_H