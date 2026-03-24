#ifndef AGRIMACHINEFAULTMESSAGE_H_
#define AGRIMACHINEFAULTMESSAGE_H_
 
#include <QWidget>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

namespace Ui
{
	class AgriMachineFaultMessage;
}

class AgriMachineFaultMessage : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMachineFaultMessage(QWidget *parent = nullptr);
	~AgriMachineFaultMessage();
public:

private:
	Ui::AgriMachineFaultMessage *ui;
};
#endif  // AgriMachineFaultMessage_H