#include "AgriMachine.h"


AgriMachine::AgriMachine(int id, const QString& name) : id(id), name(name) 
{

}

void AgriMachine::setPowerMachine(std::shared_ptr<PowerMachine> power) 
{
	powerMachine = power;
}

void AgriMachine::setWorkMachine(std::shared_ptr<WorkMachine> work)
{
	workMachine = work;
}

std::shared_ptr<PowerMachine> AgriMachine::getPowerMachine() const 
{
	return powerMachine;
}

std::shared_ptr<WorkMachine> AgriMachine::getWorkMachine() const 
{
	return workMachine;
}

QString AgriMachine::getAssemblyDescription() const 
{
	return QString("农机[%1] 装配：动力机械[%2], 作业机械[%3]")
		.arg(name)
		.arg(powerMachine ? powerMachine->getName() : "未装配")
		.arg(workMachine ? workMachine->getName() : "未装配");
}
