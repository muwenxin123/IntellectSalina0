#ifndef UAVSprayTask_h__
#define UAVSprayTask_h__

#include <iostream>
#if ( defined(_WIN32) || defined(_WINDOWS_) || defined(__WINDOWS_) )
#include <windows.h>
#endif

#include "ITask.h"

class UAVSprayTask : public ITask
{
public:
	UAVSprayTask();
	virtual ~UAVSprayTask();

	virtual int doWork();

	virtual int onCanceled(void);

	virtual int onCompleted(int);
};
#endif // UAVSprayTask_h__
