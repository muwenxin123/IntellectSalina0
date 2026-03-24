#ifndef COMM_FUNCTION_H
#define COMM_FUNCTION_H

#include <IFunctionInterface.h>

class GetAreaIDFunc : public IFunction
{
public:
	GetAreaIDFunc(void);
	virtual ~GetAreaIDFunc(void);
	
	virtual QVariant Handle(const QVariant &value)override;

private:
};

class AddSaltFieldEventFunc : public IFunction
{
public:
	AddSaltFieldEventFunc(void);
	virtual ~AddSaltFieldEventFunc(void);

	virtual QVariant Handle(const QVariant &value)override;

private:
};



#endif // My_Test_H
