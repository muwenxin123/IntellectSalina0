#ifndef AGRIMACHINE_h__
#define AGRIMACHINE_h__

#include <QString>
#include <memory>
#include "AgriPowerMachine.h"
#include "AgriWorkMachine.h"

class AgriMachine {
public:
	AgriMachine(int id, const QString& name);

	void setPowerMachine(std::shared_ptr<PowerMachine> power);
	void setWorkMachine(std::shared_ptr<WorkMachine> work);

	std::shared_ptr<PowerMachine> getPowerMachine() const;
	std::shared_ptr<WorkMachine> getWorkMachine() const;

	QString getAssemblyDescription() const;

private:
	int id;
	QString name;
	std::shared_ptr<PowerMachine> powerMachine;
	std::shared_ptr<WorkMachine> workMachine;
};


#endif // AGRIMACHINE_h__
