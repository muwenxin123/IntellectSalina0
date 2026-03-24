#ifndef AGRIMACHINEMANAGERCURSTATUS_H_
#define AGRIMACHINEMANAGERCURSTATUS_H_
 
#include <QWidget>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

namespace Ui
{
	class AgriMachineManagerCurStatus;
}

class AgriMachineManagerCurStatus : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMachineManagerCurStatus(QWidget *parent = nullptr);
	~AgriMachineManagerCurStatus();
public:

private:
	Ui::AgriMachineManagerCurStatus *ui;
};
#endif  // AgriMachineManagerCurStatus_H