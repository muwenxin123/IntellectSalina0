#include "AgriWorkMachine.h"


WorkMachine::WorkMachine(int id, const QString& name, const QString& type, double width, double depth)
	: id(id), name(name), workType(type), width(width), depth(depth) 
{

}

int WorkMachine::getId() const 
{
	return id; 
}

QString WorkMachine::getName() const 
{
	return name; 
}

QString WorkMachine::getType() const 
{
	return "WorkMachine"; 
}
