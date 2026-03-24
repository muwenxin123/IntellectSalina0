#ifndef UAVFertilizeTask_h__
#define UAVFertilizeTask_h__

#include <iostream>
#if ( defined(_WIN32) || defined(_WINDOWS_) || defined(__WINDOWS_) )
#include <windows.h>
#endif

#include "ITask.h"

class UAVFertilizeTask : public ITask
{
public:
	UAVFertilizeTask();
	virtual ~UAVFertilizeTask();

	virtual int doWork();

	virtual int onCanceled(void);

	virtual int onCompleted(int);
};
#endif // UAVFertilizeTask_h__
