#include "YoloImageMemoryManage.h"

#include <QCoreApplication>

namespace yoloImage {
	ComponentShareMemoryManage::ComponentShareMemoryManage(const QString& key, size_t size)
		: m_shmKey(key + "YoloImageManage")
		, m_semKey(key + "YoloImageManage_sem")
		, m_shm(key)
		, m_sem(m_semKey, 1)
		, m_shmSize(size)
		, m_pComponent(new ComponentInfo)
	{
		if (!m_shm.attach())
		{
            m_shm.create(static_cast<int>(m_shmSize));
			Lock();
			memset(m_shm.data(), 0, m_shmSize);
			WriteUint32(static_cast<char*>(m_shm.data()), 0);
			UnLock();
		}
	}

	ComponentShareMemoryManage::~ComponentShareMemoryManage()
	{
		m_shm.detach();
		if (nullptr != m_pComponent)
		{
			delete m_pComponent;
			m_pComponent = new ComponentInfo();
		}
	}

	bool ComponentShareMemoryManage::SetValue(const QString& key, const ComponentInfo& val)
	{
		bool isOk = false;
		if (!Lock()) return isOk;

		char* base = static_cast<char*>(m_shm.data());
		quint32 count = ReadUint32(base);
		char* ptr = base + sizeof(quint32);
		for (quint32 i = 0;  i < count; i++)
		{
			auto compPtr = ptr + i * sizeof(ComponentInfo);
			memcpy(m_pComponent, compPtr, sizeof(ComponentInfo));
			if (key == QString::fromLocal8Bit(m_pComponent->szIdentity))
			{
				memcpy(compPtr, &val, sizeof(ComponentInfo));
				isOk = true;
				break;
			}
		}

		// 为查找到则添加
		if (!isOk)
		{
			if ((count + 1) * sizeof(ComponentInfo) > m_shmSize)
				return isOk;

			auto compPtr = ptr + count * sizeof(ComponentInfo);
			memcpy(compPtr, &val, sizeof(ComponentInfo));
			count += 1;
			WriteUint32(base, count);
		}
		UnLock();
		return isOk;
	}

	bool ComponentShareMemoryManage::MarkDelete(const QString& key)
	{
		bool isOk = false;
		if (!Lock()) return isOk;

		char* base = static_cast<char*>(m_shm.data());
		quint32 count = ReadUint32(base);
		char* ptr = base + sizeof(quint32);
		for (quint32 i = 0; i < count; i++)
		{
			auto compPtr = ptr + i * sizeof(ComponentInfo);
			memcpy(m_pComponent, compPtr, sizeof(ComponentInfo));
			if (key == QString::fromLocal8Bit(m_pComponent->szIdentity))
			{
				if (!m_pComponent->isDelete)
				{
					m_pComponent->isDelete = 1;
					memcpy(compPtr, m_pComponent, sizeof(ComponentInfo));
				}

				isOk = true;
				break;
			}
		}
		UnLock();
		return isOk;
	}

	bool ComponentShareMemoryManage::GetComponentInfo(const QString& key, ComponentInfo&val)
	{
		bool isOk = false;
		if (!Lock()) return isOk;
		char* base = static_cast<char*>(m_shm.data());
		quint32 count = ReadUint32(base);
		char* ptr = base + sizeof(quint32);
		for (quint32 i = 0; i < count; i++)
		{
			auto compPtr = ptr + i * sizeof(ComponentInfo);
			memcpy(m_pComponent, compPtr, sizeof(ComponentInfo));
			if (key == QString::fromLocal8Bit(m_pComponent->szIdentity) && m_pComponent->isValid)
			{
				memcpy(&val, m_pComponent, sizeof(ComponentInfo));
				m_pComponent->isValid = 0;
				memcpy(compPtr, m_pComponent, sizeof(ComponentInfo));
				isOk = true;
				break;
			}
		}
		UnLock();
		return isOk;
	}

	bool ComponentShareMemoryManage::GetAllComponentInfo(QList<QComponentInfo>& list)
	{
		bool isOk = false;
		list.clear();
		if (!Lock())	return isOk;

		char* base = static_cast<char*>(m_shm.data());
		quint32 count = ReadUint32(base);
		char* ptr = base + sizeof(quint32);
		for (quint32 i = 0; i < count; i++)
		{
			auto compPtr = ptr + i * sizeof(ComponentInfo);
			memcpy(m_pComponent, compPtr, sizeof(ComponentInfo));
			if (m_pComponent->isValid)
			{
				m_pComponent->isValid = 0;
				memcpy(compPtr, m_pComponent, sizeof(ComponentInfo));
				list.append(std::move(QComponentInfo{m_pComponent->szIdentity, m_pComponent->szIdentitySize, m_pComponent->szImage,
				m_pComponent->szImageSize}));

			}
		}

		isOk = true;
		UnLock();
		return isOk;
	}

	int ComponentShareMemoryManage::Size()
    {
        if (!Lock())	return 0;
        quint32 count = ReadUint32(static_cast<char*>(m_shm.data()));
        UnLock();
        return static_cast<int>(count);
    }

    bool ComponentShareMemoryManage::IsFull()
    {
        bool isOk = false;
        if (!Lock()) return false;

		char* base = static_cast<char*>(m_shm.data());
		quint32 count = ReadUint32(base);

		isOk = (count + 1) * sizeof(ComponentInfo) > m_shmSize;
		UnLock();

		return isOk;
	}

	void ComponentShareMemoryManage::Reflush()
	{
		bool isOk = false;
		if (!Lock()) return;

		char* base = static_cast<char*>(m_shm.data());
		quint32 count = ReadUint32(base);
		char* ptr = base + sizeof(quint32);

        quint32 curIndex = 0;
        quint32 tmpCnt = count;
        for (quint32 i = 0; i < tmpCnt; i++)
		{
			auto compPtr = ptr + i * sizeof(ComponentInfo);
			memcpy(m_pComponent, compPtr, sizeof(ComponentInfo));
			// 当前删除状态
			if (m_pComponent->isDelete)
			{
				// 当前项置为空
				memset(compPtr, 0, sizeof(ComponentInfo));
				count--;
			}
			// 当前非删除状态，但是前面已有删除内容， 则后项前移
			else if (curIndex != i)
			{
				// 后项拷贝
				auto curComptr = ptr + curIndex * sizeof(ComponentInfo);
				memcpy(&curComptr, compPtr, sizeof(ComponentInfo));
				curIndex++;

				// 当前项置0
				memset(compPtr, 0, sizeof(ComponentInfo));
			}
			// 非删除状态，curIndex == i
			else
			{
				curIndex++;
			}
		}
		WriteUint32(base, count);
		UnLock();
	}

	bool ComponentShareMemoryManage::Lock()
	{
		m_sem.acquire();
		return m_shm.lock();
	}

	void ComponentShareMemoryManage::UnLock()
	{
		m_shm.unlock();
		m_sem.release();
	}

	quint32 ComponentShareMemoryManage::ReadUint32(char* ptr)
	{
		quint32 val;
		memcpy(&val, ptr, sizeof(quint32));
		return val;
	}

	void ComponentShareMemoryManage::WriteUint32(char*ptr, quint32 val)
	{
		memcpy(ptr, &val, sizeof(quint32));
	}

	bool ComponentShareMemoryManage::IsContainsKey(const QString& key)
	{
		bool isOk = false;

		char* base = static_cast<char*>(m_shm.data());
		quint32 count = ReadUint32(base);
		char* ptr = base + sizeof(quint32);
		for (quint32 i = 0; i < count; i++)
		{
			auto compPtr = ptr + i * sizeof(ComponentInfo);
			memcpy(m_pComponent, compPtr, sizeof(ComponentInfo));
			if (m_pComponent->isValid && key == QString::fromLocal8Bit(m_pComponent->szIdentity))
			{
				isOk = true;
				break;
			}
		}
		return isOk;
	}

	YoloImageServiceMemoryManage::YoloImageServiceMemoryManage()
		: ComponentShareMemoryManage(QString::number(QCoreApplication::applicationPid()), 20 * sizeof(ComponentInfo) + sizeof(quint32) + 4)
	{

	}

	YoloImageServiceMemoryManage::~YoloImageServiceMemoryManage()
	{

	}

	YoloImageServiceMemoryManage& YoloImageServiceMemoryManage::Get()
	{
		static  YoloImageServiceMemoryManage s_service;
		return s_service;
	}

	YoloImageClientMemoryManage::YoloImageClientMemoryManage(const QString& key)
        : ComponentShareMemoryManage(key, 20 * sizeof(ComponentInfo) + sizeof(quint32) + 4)
	{
	}

	YoloImageClientMemoryManage::~YoloImageClientMemoryManage()
	{
	}
};
