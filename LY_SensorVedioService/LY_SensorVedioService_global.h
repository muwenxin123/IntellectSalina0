#pragma once
#include <QtCore/qglobal.h>

#include <atomic>
#include <thread>
#include <memory>

/*该类用于显示单兵显示编辑*/
#if defined(LY_SENSORVEDIOSERVICE_LIBRARY)
#define LY_SENSORVEDIOSERVICE_EXPORT Q_DECL_EXPORT
#else
#define LY_SENSORVEDIOSERVICE_EXPORT Q_DECL_IMPORT
#endif

class SpinLocker {
public:
	SpinLocker(std::shared_ptr<std::atomic_flag> flag) : m_flag(flag)
	{
		while (m_flag->test_and_set(std::memory_order_acquire))
			std::this_thread::yield();
	}
	~SpinLocker()
	{
		m_flag->clear(std::memory_order_release);
	}

private:
	std::shared_ptr<std::atomic_flag> m_flag;
};

namespace sensorSpace {
#pragma pack(push)
#pragma pack(1)
	struct  SensorHeader {
		char version = 0;							//  版本
		char senderUUID[64] = { 0 };		//  发送者ID
		unsigned type = 0;					    //  类型ID（备用）
		unsigned cmd = 0;					    //  指令ID（备用）
		unsigned length = 0;					//  长度
		unsigned checkVal = 0x00FF;		//  校验
		long long bodyID = 0;					//  主题对象ID
		quint64 timeStep = 0;					//  时间戳
	};
#pragma pack(pop)
};

//#define FULL_SMOOTH_IMAGE