#include "AgriPowerMachine.h"


PowerMachine::PowerMachine(int id, const QString& name, const QString& engineModel, double power)
	: id(id), name(name), engineModel(engineModel), power(power) 
{

}

int PowerMachine::getId() const 
{
	return id; 
}

QString PowerMachine::getName() const 
{
	return name; 
}

QString PowerMachine::getType() const 
{
	return "PowerMachine"; 
}
