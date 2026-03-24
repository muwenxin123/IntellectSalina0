#ifndef YOLOIMAGEMEMORYMANGE_H
#define YOLOIMAGEMEMORYMANGE_H

#include <atomic>
#include <QMutex>
#include <memory>
#include <QVector>
#include <QMutexLocker>
#include <QSharedMemory>
#include <QSystemSemaphore>

#define YOLO_SHARED_IMAGE_MAX (600 * 1024)

namespace yoloImage {
#pragma pack(push)
#pragma pack(1)
struct ComponentInfo
{
	char isValid = 1;
	char isDelete = 0;
	int szIdentitySize = 0;
	int szImageSize = 0;
	char szIdentity[128] = { 0 };
	char szImage[YOLO_SHARED_IMAGE_MAX] = { 0 };
};
#pragma pack(pop)

struct QComponentInfo
{
	QComponentInfo() {}
	QComponentInfo(const char* szIdentity, int identitySize, const char* szImage, int imageSize)
		: imageByte(szImage, imageSize)
	{
		identity = QString::fromLocal8Bit(QByteArray(szIdentity, identitySize));
	}
	QString identity;
	QByteArray imageByte;
};

	class ComponentShareMemoryManage
	{
	public:
		explicit ComponentShareMemoryManage(const QString& key, size_t Size);
		virtual ~ComponentShareMemoryManage();

		// 设置数据
		bool SetValue(const QString& key, const ComponentInfo& val);

		// 标识为Delete
		bool MarkDelete(const QString& key);

		// 获取数据
		bool GetComponentInfo(const QString& key, ComponentInfo&val);

		// 获取所有组件信息
		bool GetAllComponentInfo(QList<QComponentInfo>& list);

		// 大小
		int Size();

		// 是否满
		bool IsFull();

		// 刷新并删除标记为Delete的
		void Reflush();

	private:
		bool Lock();
		void UnLock();
		quint32 ReadUint32(char* ptr);
		void WriteUint32(char*ptr, quint32 val);
		bool IsContainsKey(const QString& key);

	private:
        QString m_shmKey;												// 共享内存键值
        QString m_semKey;												// 信号量键值
        QSharedMemory m_shm;									// 共享内存
        QSystemSemaphore m_sem;  							// 信号
        size_t m_shmSize = 0;											// 共享内存大小
		ComponentInfo* m_pComponent = nullptr;		// 充当临时变量，防止栈溢出
	};

	class YoloImageServiceMemoryManage : public ComponentShareMemoryManage {
	public:
		YoloImageServiceMemoryManage();
		virtual ~YoloImageServiceMemoryManage();

		static YoloImageServiceMemoryManage& Get();
	};

	class YoloImageClientMemoryManage : public ComponentShareMemoryManage
	{
	public:
		YoloImageClientMemoryManage(const QString& key);
		virtual ~YoloImageClientMemoryManage();
	};
};

typedef std::shared_ptr<yoloImage::YoloImageClientMemoryManage>     YoloImageClientMemoryManageSPtr;
typedef std::weak_ptr<yoloImage::YoloImageClientMemoryManage>       YoloImageClientMemoryManageWPtr;
#endif
