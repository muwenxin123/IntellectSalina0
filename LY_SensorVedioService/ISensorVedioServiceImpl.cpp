#include "ISensorVedioServiceImpl.h"
#include <QDb.h>
#include <QCString.h>
#include <QGeoCoordinate>

#include <QReadLocker>
#include <QWriteLocker>
#include <time.h>
#include <BufferQt.h>
#include <QDebug>
#include "LY_SensorVedioService_global.h"
#include "LY_SensorVedioService.h"
#include "SensorSocketSender.h"

#define  MAX_QUEUE_SIZE  1
#define  MAX_TIME_OUT_CNT  10

// 单兵系统在主函数已经连接数据库，所以此时不需要在连接数据库
qnzkna::SensorVedio::ISensorVedioServiceImpl::ISensorVedioServiceImpl(void)
	:m_flag(new std::atomic_flag ATOMIC_FLAG_INIT)

{
}

qnzkna::SensorVedio::ISensorVedioServiceImpl::~ISensorVedioServiceImpl(void)
{

}

qnzkna::SensorVedio::ISensorVedioServiceImpl& qnzkna::SensorVedio::ISensorVedioServiceImpl::Get()
{
	static ISensorVedioServiceImpl s_ISensorVedioServiceImpl;
	return s_ISensorVedioServiceImpl;
}

// 添加标识对应的播放地址(线程不安全)
void qnzkna::SensorVedio::ISensorVedioServiceImpl::AddUrl(long long identity, const QString& url)
{
	m_identity2PlayVedioUrlMap[identity] = url;
}

// 获取标识对应的Url(线程不安全)
QString qnzkna::SensorVedio::ISensorVedioServiceImpl::GetUrl(long long identity)
{
	QString retString;
	auto iter = m_identity2PlayVedioUrlMap.find(identity);
	if (iter != m_identity2PlayVedioUrlMap.end())
	{
		retString = *iter;
	}
	return retString;
}

// 添加数据(线程安全)
void qnzkna::SensorVedio::ISensorVedioServiceImpl::AddPixMap(long long identity, const QByteArray& map)
{
	clock_t  time_begin = clock_t();
	clock_t  time_end = clock_t();
	//qDebug() << "qnzkna::SensorVedio::ISensorVedioServiceImpl::" << time_end - time_begin;
	std::list<DataFrameInfoSPtr> deleteList;
	{
		SpinLocker locker(m_flag);
		auto iter = m_Identity2QueueMap.find(identity);
		if (iter != m_Identity2QueueMap.end())
		{
			// 数据链路过长时丢弃数据
			auto& queue = iter->bitMapQueue;
			DataFrameInfoSPtr data;
			while (queue.size() > MAX_QUEUE_SIZE)
			{
				data = queue.front();
				deleteList.push_back(data);
				queue.pop_front();
			}

			if (data != nullptr)
				m_lastIdentityStatusMap[iter.key()] = data;
			iter->bitMapQueue.push_back(std::make_shared<DataFrameInfo>(std::move(map)));
			iter->timeOutCnt = 0;
		}
		else {
			QQueue<QByteArray> queue;
			VedioInfo info;
			info.identity = identity;
			info.bitMapQueue.push_back(std::make_shared<DataFrameInfo>(std::move(map)));
			m_Identity2QueueMap.insert(identity, info);
		}
	}
	deleteList.clear();
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::AddPixMap(long long identity, AVFrame* pt)
{
	std::list<DataFrameInfoSPtr> deleteList;
	{
		SpinLocker locker(m_flag);
		auto iter = m_Identity2QueueMap.find(identity);
		if (iter != m_Identity2QueueMap.end())
		{
			// 数据链路过长时丢弃数据
			auto& queue = iter->bitMapQueue;
			DataFrameInfoSPtr data;
			while (queue.size() > MAX_QUEUE_SIZE)
			{
				data = queue.front();
				deleteList.push_back(data);
				queue.pop_front();
			}
			if (data != nullptr)
				m_lastIdentityStatusMap[iter.key()] = data;

			iter->bitMapQueue.push_back(std::make_shared<DataFrameInfo>(pt));
			iter->timeOutCnt = 0;
		}
		else {
			QQueue<QByteArray> queue;
			VedioInfo info;
			info.identity = identity;
			info.bitMapQueue.push_back(std::make_shared<DataFrameInfo>(pt));
			m_Identity2QueueMap.insert(identity, info);
		}
	}
	deleteList.clear();
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::AddPixMap(long long identity, const char* szData, size_t len)
{
	AddPixMap(identity, QByteArray(szData, len));
}

// 获取最新的数据(线程安全)
bool qnzkna::SensorVedio::ISensorVedioServiceImpl::GetLastPixMap(long long identity, QPixmap& map)
{
	bool isOk = false;
	DataFrameInfoSPtr data = nullptr;
	{
		SpinLocker locker(m_flag);
		auto iter = m_lastIdentityStatusMap.find(identity);
		if (iter != m_lastIdentityStatusMap.end())
		{
			data = iter.value();
			isOk = true;
		}
	}

	if (data != nullptr)
	{
		isOk = data->GetDataPixMap(map);
	}
	return isOk;
}

bool qnzkna::SensorVedio::ISensorVedioServiceImpl::GetLastPixMapData(long long identity, QByteArray& array)
{
	bool isOk = false;
	DataFrameInfoSPtr data = nullptr;
	{
		SpinLocker locker(m_flag);
		auto iter = m_lastIdentityStatusMap.find(identity);
		if (iter != m_lastIdentityStatusMap.end())
		{
			data = iter.value();
			isOk = true;
		}
	}

	if (data != nullptr)
	{
		isOk = data->GetByteArrayData(array);
	}
	return isOk;
}

bool qnzkna::SensorVedio::ISensorVedioServiceImpl::GetLastImage(long long identity, QImage& map)
{
	bool isOk = false;
	DataFrameInfoSPtr data = nullptr;
	{
		SpinLocker locker(m_flag);
		auto iter = m_lastIdentityStatusMap.find(identity);
		if (iter != m_lastIdentityStatusMap.end())
		{
			data = iter.value();
			isOk = true;
		}
	}

	if (data != nullptr)
	{
		isOk = data->GetDataImage(map);
	}
	return isOk;
}

bool qnzkna::SensorVedio::ISensorVedioServiceImpl::GetLastPixMap(long long identity, AVFrame** pAvframe, QByteArray& array)
{
	bool isOk = false;
	DataFrameInfoSPtr data = nullptr;
	{
		SpinLocker locker(m_flag);
		auto iter = m_lastIdentityStatusMap.find(identity);
		if (iter != m_lastIdentityStatusMap.end())
		{
			data = iter.value();
			data->CloneData(array, pAvframe);
			isOk = true;
		}
	}
	return isOk;
}

// 超时处理(线程安全)
void qnzkna::SensorVedio::ISensorVedioServiceImpl::OnTimeOut()
{
	std::list<DataFrameInfoSPtr> deleteList;
	{
		SpinLocker locker(m_flag);
		for (auto iter = m_Identity2QueueMap.begin(); iter != m_Identity2QueueMap.end(); )
		{
			iter->timeOutCnt++;
			if (MAX_TIME_OUT_CNT < iter->timeOutCnt)
			{
				// 清空数据
				for (auto byteArray : iter->bitMapQueue)
					deleteList.push_back(byteArray);

				// 移除暂存的数据
				m_lastIdentityStatusMap.remove(iter->identity);
				iter = m_Identity2QueueMap.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		for (auto iter = m_IdentityTargetStatusMap.begin(); iter != m_IdentityTargetStatusMap.end(); )
		{
			iter->timeOutCnt++;
			iter->timeOutCnt++;
			if (MAX_TIME_OUT_CNT < iter->timeOutCnt)
			{
				iter = m_IdentityTargetStatusMap.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		for (auto iter = m_IdentityFriendStatusMap.begin(); iter != m_IdentityFriendStatusMap.end(); )
		{
			iter->timeOutCnt++;
			iter->timeOutCnt++;
			if (MAX_TIME_OUT_CNT < iter->timeOutCnt)
			{
				iter = m_IdentityFriendStatusMap.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}

	// array析构时间过长放在外面
	deleteList.clear();
}

// 获取激活视频的ID(线程安全)
void qnzkna::SensorVedio::ISensorVedioServiceImpl::GetActiveIdentitys(QList<long long>& identityList)
{
	identityList.clear();
	SpinLocker locker(m_flag);
	identityList = m_Identity2QueueMap.keys();
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::RemoveComponentTargetIDs(long long identity)
{
	SpinLocker locker(m_flag);
	auto iter = m_IdentityTargetStatusMap.find(identity);
	if (iter != m_IdentityTargetStatusMap.end())
	{
		iter = m_IdentityTargetStatusMap.erase(iter);
	}
}

// 更新组件列表
void qnzkna::SensorVedio::ISensorVedioServiceImpl::UpdateComponentTargetIDs(long long identity, std::list<long long>& targetIDs)
{
	QList<long long> targetList = QList<long long>::fromStdList(targetIDs);
	QSet<long long> targetSet = targetList.toSet();
	{
		SpinLocker locker(m_flag);
		auto iter = m_IdentityTargetStatusMap.find(identity);
		if (iter != m_IdentityTargetStatusMap.end())
		{
			iter->timeOutCnt = 0;
			for (auto targetIter = iter->targetRectInfoMap.begin();
				targetIter != iter->targetRectInfoMap.end();)
			{
				if (targetSet.contains(targetIter.key()))
				{
					targetIter++;
				}
				else {
					targetIter = iter->targetRectInfoMap.erase(targetIter);
				}
			}
		}
	}
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::UpdateComponentFrinedIDs(long long identity, std::list<long long>& targetIDs)
{
	QList<long long> targetList = QList<long long>::fromStdList(targetIDs);
	QSet<long long> targetSet = targetList.toSet();
	{
		SpinLocker locker(m_flag);
		auto iter = m_IdentityFriendStatusMap.find(identity);
		if (iter != m_IdentityFriendStatusMap.end())
		{
			iter->timeOutCnt = 0;
			for (auto targetIter = iter->friendRectInfoMap.begin();
				targetIter != iter->friendRectInfoMap.end();)
			{
				if (targetSet.contains(targetIter.key()))
				{
					targetIter++;
				}
				else {
					targetIter = iter->friendRectInfoMap.erase(targetIter);
				}
			}
		}
	}
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::SetComponentFriendRect(const FriendRectScope& target)
{
	SpinLocker locker(m_flag);
	auto iter = m_IdentityFriendStatusMap.find(target.componentID);
	if (iter != m_IdentityFriendStatusMap.end())
	{
		iter->timeOutCnt = 0;
		auto targetIter = (*iter).friendRectInfoMap.find(target.targetID);
		if (targetIter != (*iter).friendRectInfoMap.end())
		{
			*targetIter = target;
		}
		else
		{
			iter->friendRectInfoMap[target.targetID] = target;
		}
	}
	else
	{
		m_IdentityFriendStatusMap[target.componentID].friendRectInfoMap[target.targetID] = target;
	}
}

// 设置组件目标区域
void qnzkna::SensorVedio::ISensorVedioServiceImpl::SetComponentTargetRect(const XygsZC_Mark_01& target)
{
	SpinLocker locker(m_flag);
	auto iter = m_IdentityTargetStatusMap.find(target.componentID);
	if (iter != m_IdentityTargetStatusMap.end())
	{
		iter->timeOutCnt = 0;
		auto targetIter = (*iter).targetRectInfoMap.find(target.targetID);
		if (targetIter != (*iter).targetRectInfoMap.end())
		{
			*targetIter = target;
		}
		else 
		{
			iter->targetRectInfoMap[target.targetID] = target;
		}
	}
	else 
	{
		m_IdentityTargetStatusMap[target.componentID].targetRectInfoMap[target.targetID] = target;
	}
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::RemoveComponentFriendIDs(long long identity)
{
	SpinLocker locker(m_flag);
	auto iter = m_IdentityFriendStatusMap.find(identity);
	if (iter != m_IdentityFriendStatusMap.end())
	{
		iter = m_IdentityFriendStatusMap.erase(iter);
	}
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::GetComponentTargets(long long identity, QList<XygsZC_Mark_01>& targets)
{
	targets.clear();
	{
		SpinLocker locker(m_flag);
		auto iter = m_IdentityTargetStatusMap.find(identity);
		if (iter != m_IdentityTargetStatusMap.end())
		{
			targets = iter.value().targetRectInfoMap.values();
		}
	}
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::SendUpdateComponentTargets(long long identity,
	std::list<long long>& targetIdentityList)
{
	auto pSender = LY_SensorVedioService::Get().GetSender();
	if (nullptr != pSender)
	{
		CBuffer buff(512);
		buff << targetIdentityList;
		QByteArray array;
		array.append((char*)&identity, sizeof(identity));
		array.append(buff.Data(), buff.Size());
		pSender->AppendData(qnzkna::SensorVedio::UpdateTargets, array);
	}
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::SendUpdateComponentTargetData(long long identity, 
	XygsZC_Mark_01& data)
{
	auto pSender = LY_SensorVedioService::Get().GetSender();
	if (nullptr != pSender)
	{
		QByteArray array;
		pSender->AppendData(qnzkna::SensorVedio::UpdateTargetData, (char*)&data, sizeof(XygsZC_Mark_01));
	}
}

// 发送更新组件友方 (线程安全)
void qnzkna::SensorVedio::ISensorVedioServiceImpl::SendUpdateComponentFriendScope(long long identity, std::list<FriendRectScope>& list)
{
	auto pSender = LY_SensorVedioService::Get().GetSender();
	if (nullptr != pSender)
	{
		std::list<long long> idList;
		for (auto& data: list)
		{
			idList.push_back(data.targetID);
			QByteArray array;
			pSender->AppendData(qnzkna::SensorVedio::UpdateFriendData, (char*)&data, sizeof(FriendRectScope));
		}

		{
			CBuffer buff(512);
			buff << idList;
			QByteArray array;
			array.append((char*)&identity, sizeof(identity));
			array.append(buff.Data(), buff.Size());
			pSender->AppendData(qnzkna::SensorVedio::UpdateFriends, array);
		}
	}
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::SendUpdateComponentClearFriendScope(long long identity)
{
	auto pSender = LY_SensorVedioService::Get().GetSender();
	if (nullptr != pSender)
	{
		QByteArray array;
		array.append((char*)&identity, sizeof(identity));
		pSender->AppendData(qnzkna::SensorVedio::ClearFriends, array);
	}
}

// 获取更新组件友方(线程安全)
void qnzkna::SensorVedio::ISensorVedioServiceImpl::GetComponentFriendScopes(long long identity, QList<FriendRectScope>& list)
{
	list.clear();
	SpinLocker locker(m_flag);
	auto iter = m_IdentityFriendStatusMap.find(identity);
	if (iter != m_IdentityFriendStatusMap.end())
	{
		list = iter->friendRectInfoMap.values();
	}
}

// 接收更新组件的目标列表
void qnzkna::SensorVedio::ISensorVedioServiceImpl::RecvUpdateComponentFriends(char* data, size_t size)
{
	size_t sizelonglong = sizeof(long long);
	if (size < sizelonglong)
		return;

	long long identity = 0;
	memcpy(&identity, data, sizelonglong);

	std::list<long long> friendList;
	size_t targetSize = size - sizelonglong;
	if (targetSize > 0)
	{
		CBuffer buff(data + sizelonglong, targetSize);
		buff >> friendList;
	}

	UpdateComponentFrinedIDs(identity, friendList);
}

// 接收更新组件目标信息
void qnzkna::SensorVedio::ISensorVedioServiceImpl::RecvUpdateComponentFriendData(char* data, size_t size)
{
	if (size != sizeof(FriendRectScope))
		return;

	FriendRectScope friendData;
	memcpy(&friendData, data, size);
	SetComponentFriendRect(friendData);
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::RecvClearComponentFriend(char* data, size_t size)
{
	size_t sizelonglong = sizeof(long long);
	if (size < sizelonglong)
		return;

	long long identity = 0;
	memcpy(&identity, data, sizelonglong);
	RemoveComponentFriendIDs(identity);
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::SendClearComponentTargets(long long identity)
{
	auto pSender = LY_SensorVedioService::Get().GetSender();
	if (nullptr != pSender)
	{
		QByteArray array;
		array.append((char*)&identity, sizeof(identity));
		pSender->AppendData(qnzkna::SensorVedio::ClearTarget, array);
	}
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::RecvUpdateComponentTargets(char* data, size_t size)
{
	size_t sizelonglong = sizeof(long long);
	if (size < sizelonglong)
		return;

	long long identity = 0;
	memcpy(&identity, data, sizelonglong);

	std::list<long long> targetIdentityList;
	size_t targetSize = size - sizelonglong;
	if (targetSize > 0)
	{
		CBuffer buff(data + sizelonglong, targetSize);
		buff >> targetIdentityList;
	}

	UpdateComponentTargetIDs(identity, targetIdentityList);
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::RecvUpdateComponentTargetData(char* data, size_t size)
{
	if (size != sizeof(XygsZC_Mark_01))
		return;

	XygsZC_Mark_01 targetInfo;
	memcpy(&targetInfo, data, size);
	SetComponentTargetRect(targetInfo);
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::RecvClearComponentTargets(char* data, size_t size)
{
	size_t sizelonglong = sizeof(long long);
	if (size < sizelonglong)
		return;

	long long identity = 0;
	memcpy(&identity, data, sizelonglong);
	RemoveComponentTargetIDs(identity);
}

void qnzkna::SensorVedio::ISensorVedioServiceImpl::PopPixMap()
{
	std::list<DataFrameInfoSPtr> deleteList;
	{
		SpinLocker locker(m_flag);
		DataFrameInfoSPtr  tmpData;
		for (auto iter = m_Identity2QueueMap.begin(); iter != m_Identity2QueueMap.end(); iter++)
		{
			if (!iter->bitMapQueue.empty())
			{
				tmpData = iter->bitMapQueue.front();
				iter->bitMapQueue.pop_front();
				m_lastIdentityStatusMap[iter.key()].swap(tmpData);
				deleteList.push_back(tmpData);
			}
		}
	}

	// array析构时间过长放在外面
	deleteList.clear();
}

qnzkna::SensorVedio::DataFrameInfo::DataFrameInfo(QByteArray& array)
	: m_array(std::move(array))
	, m_isAvFrame(false)
{

}

qnzkna::SensorVedio::DataFrameInfo::DataFrameInfo(const QByteArray& array)
	: m_array(std::move(array))
	, m_isAvFrame(false)
{

}

qnzkna::SensorVedio::DataFrameInfo::DataFrameInfo(AVFrame* pAVFrame)
	: m_pAVFrame(pAVFrame)
	, m_isAvFrame(true)
{

}

qnzkna::SensorVedio::DataFrameInfo::~DataFrameInfo()
{
	QWriteLocker locker(&m_lock);
	if (!m_isAvFrame)
	{
		m_array.clear();
		m_array.squeeze();
	}

	if (nullptr != m_pAVFrame)
	{
		av_frame_free(&m_pAVFrame);
	}
}

bool qnzkna::SensorVedio::DataFrameInfo::GetDataImage(QImage& image)
{
	QReadLocker locker(&m_lock);
	if (m_image.isNull())
	{
		if (!m_isAvFrame)
		{
			if (image.loadFromData(m_array, "JPG"))
			{
				m_image = image.copy();
				return true;
			}
		}
		else
		{
			if (GenerateFromAvFrame(image))
			{
				m_image = image.copy();
				return true;
			}
		}
	}
	else
	{
		image = m_image.copy();
		return true;
	}
	return false;
}

bool qnzkna::SensorVedio::DataFrameInfo::GetDataPixMap(QPixmap& map)
{
	bool isOk = false;
	QReadLocker locker(&m_lock);
	QImage image;
	if (GetDataImage(image))
	{
		map = QPixmap::fromImage(image);
		isOk = !map.isNull();
	}
	return isOk;
}

bool qnzkna::SensorVedio::DataFrameInfo::CloneData(QByteArray& array, AVFrame** avFrame)
{
	if (nullptr != *avFrame)
	{
		av_frame_free(avFrame);
		*avFrame = nullptr;
	}
	if (nullptr != m_pAVFrame)
		*avFrame = av_frame_clone(m_pAVFrame);

	array.clear();
	if (!m_array.isEmpty())
	{
		array.resize(m_array.size());
		memcpy(array.data(), m_array.data(), m_array.size());
	}
	
	return true;
}

bool qnzkna::SensorVedio::DataFrameInfo::GetByteArrayData(QByteArray& array)
{
	array.clear();
	array.squeeze();
	if (!m_array.isEmpty())
	{
		array.resize(m_array.size());
		memcpy(array.data(), m_array.data(), m_array.size());
		return true;
	}

	if (m_image.isNull())
	{
		QImage image;
		GetDataImage(image);
	}

	if (!m_image.isNull())
	{
		QBuffer buffer(&array);
		buffer.open(QIODevice::WriteOnly);
		m_image.save(&buffer, "JPEG");
	}
	return false;
}

bool qnzkna::SensorVedio::DataFrameInfo::GenerateFromAvFrame(QImage& image)
{
	bool isOk = false;
	if (nullptr == m_pAVFrame || !m_pAVFrame->data[0])
		return isOk;

	// 单次处理为20-33ms
	auto pFrame = m_pAVFrame;
	int width = pFrame->width;
	int height = pFrame->height;
	auto pContext = sws_getContext(width, height, (AVPixelFormat)pFrame->format,
		width, height, AVPixelFormat::AV_PIX_FMT_RGB24,
		SWS_BILINEAR, nullptr, nullptr, nullptr);
	if (nullptr != pContext)
	{
		uint8_t* buffer[4];
		int  rgbLineSize[4];
		if (av_image_alloc(buffer, rgbLineSize, width, height, AVPixelFormat::AV_PIX_FMT_RGB24, 1) >= 0)
		{
			int ret = sws_scale(pContext, pFrame->data, pFrame->linesize, 0, height, buffer, rgbLineSize);
			if (ret > 0 && ret == height)
			{
				QImage imageTmp(buffer[0], width, height, rgbLineSize[0], QImage::Format_RGB888);
				image = imageTmp.copy();
				isOk = true;
			}
			av_free(buffer[0]);
		}
		sws_freeContext(pContext);
		pContext = nullptr;
	}

	return isOk;
}
