#ifndef AGRISATELLITEPYTHONCALLBACK_H_
#define AGRISATELLITEPYTHONCALLBACK_H_
 
#include "python_c_api_wrapper.h"

#include <QObject>
#include <QThreadStorage>

class CallbackDispatcher : public QObject {
	Q_OBJECT
public:
	explicit CallbackDispatcher(QObject* parent = nullptr) : QObject(parent) {}

signals:
	void messageReceived(QString msg);
};


/*  回调函数必须是静态函数或自由函数, Python 只能接受普通函数指针，而不能直接接受成员函数指针 */
class CallbackWrapper {
public:
	/* 静态全局转发器
	QThreadStorage 本质是一个“线程->对象”字典，每个线程独立拥有 */
	static QThreadStorage<CallbackDispatcher*> dispatcherPool;

	// 静态回调函数
	static void my_callback(const char* message)
	{
		CallbackDispatcher* dispatcher = dispatcherPool.localData();
		if (dispatcher) {
			emit dispatcher->messageReceived(QString::fromUtf8(message));
		}
	}

	// 返回 PyCapsule 对象 
	static PyObject* get_callback_capsule()
	{
		return PyCapsule_New((void*)my_callback, "my_callback", nullptr);
	}
};
#endif  // AgriSatellitePythonCallback_H