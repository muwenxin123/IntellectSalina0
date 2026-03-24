#ifndef Task_h__
#define Task_h__

#include <time.h>
#include <atomic>

// 任务基类
class ITask
{

protected:

	// 任务的唯一标识
	size_t                      id_;
	// 任务创建时间 （非Unix时间戳）
	clock_t                     createTime_;

private:

	static std::atomic<size_t>  nRequestID_;
	// 任务取消状态
	std::atomic<bool>           isCancelRequired_;

	// 任务等待状态
	std::atomic<bool>           isWaitRequired_;

public:

	ITask(void) :id_(nRequestID_++), isCancelRequired_(false), isWaitRequired_(true),
		createTime_(clock()) {}

	virtual ~ITask(void) {};

public:

	virtual void cleanUp();

	// 任务类虚接口，继承这个类的必须要实现这个接口
	virtual int doWork(void) = 0;

	// 任务已取消回调
	virtual int onCanceled(void) { return 1; }

	// 任务已完成
	virtual int onCompleted(int) { return 1; }

	// 任务是否超时
	virtual bool isTimeout(const clock_t& now) {
		return ((now - createTime_) > 5000);
	}

	// 获取任务ID
	size_t getID(void) { return id_; }

	// 获取任务取消状态
	bool isCancelRequired(void) { return isCancelRequired_; }

	// 设置任务取消状态
	void setCancelRequired(void) { isCancelRequired_ = true; }

	// 获取任务等待状态
	bool isWaitRequired(void) { return isWaitRequired_; }

	// 取消任务等待状态
	void setWaitRequired(void) { isWaitRequired_ = false; }
};

__declspec(selectany) std::atomic<size_t> ITask::nRequestID_ = 100000;
#endif // Task_h__
