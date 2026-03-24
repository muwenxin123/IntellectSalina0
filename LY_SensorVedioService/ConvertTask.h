#ifndef CONVERT_TASK_H
#define CONVERT_TASK_H

#include <atomic>
#include <functional>
#include <QRunnable>
#include <QMutex>
typedef std::function<void()> ConvertFunc;
class ConvertTask : public QRunnable
{
public:	
	ConvertTask(ConvertFunc func);
	virtual ~ConvertTask();
	
	void run()override;

private:
	ConvertFunc m_func;
};

#endif // VIDEOSDLCONTAINER_H
