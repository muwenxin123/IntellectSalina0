#ifndef UAVPatrolTask_h__
#define UAVPatrolTask_h__

#include <iostream>
#if ( defined(_WIN32) || defined(_WINDOWS_) || defined(__WINDOWS_) )
#include <windows.h>
#endif

#include "ITask.h"


class UAVPatrolTask : public ITask
{
public:
	UAVPatrolTask();
	virtual ~UAVPatrolTask();

	virtual int doWork();

	virtual int onCanceled(void);

	virtual int onCompleted(int);

};
#endif // UAVPatrolTask_h__
