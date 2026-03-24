#include "ConvertTask.h"

#include <QMutexLocker>

ConvertTask::ConvertTask(ConvertFunc func)
	:m_func(func)
{
	setAutoDelete(true);
}

ConvertTask::~ConvertTask()
{
}


void ConvertTask::run()
{
	if(m_func != nullptr)
		m_func();
}