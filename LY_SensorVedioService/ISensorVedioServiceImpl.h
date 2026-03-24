#ifndef _ISENSORVEDIOSERVICEIMPL_H
#define _ISENSORVEDIOSERVICEIMPL_H

#include "QSqlDatabase"
#include "QSqlQuery"
#include "QSqlRecord"
#include "QSqlError"
#include <QGeoCoordinate>

#include <QMap>
#include <memory>
#include <atomic>
#include <QQueue>
#include <QPixmap>
#include <QReadWriteLock>
#include "DataManage/ISensorVedioService.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libavutil/hwcontext.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

namespace qnzkna
{
namespace SensorVedio
{

class DataFrameInfo
{
public:
	DataFrameInfo(QByteArray& array);
	DataFrameInfo(const QByteArray& array);
	DataFrameInfo(AVFrame* pAVFrame);
	virtual ~DataFrameInfo();

	bool GetDataImage(QImage& image);
	bool GetDataPixMap(QPixmap& image);
	bool CloneData(QByteArray& array, AVFrame** avFrame);
	bool GetByteArrayData(QByteArray& array);

private:
	bool GenerateFromAvFrame(QImage& image);

private:
	QReadWriteLock m_lock;
	QImage m_image;
	bool m_isAvFrame = false;
	QByteArray m_array;
	AVFrame* m_pAVFrame = nullptr;
	Q_DISABLE_COPY(DataFrameInfo)
};

typedef std::shared_ptr<DataFrameInfo>     DataFrameInfoSPtr;
typedef std::weak_ptr<DataFrameInfo>       DataFrameInfoWPtr;

struct VedioInfo
{
	long long identity = 0;
	int timeOutCnt = 0;
	std::list<DataFrameInfoSPtr> bitMapQueue;
};

struct TargetRectInfo
{
	long long identity = 0;
	int timeOutCnt = 0;
	QMap<unsigned long long int,  XygsZC_Mark_01> targetRectInfoMap;
};

struct FriendRectInfo
{
	long long identity = 0;
	int timeOutCnt = 0;
	QMap<unsigned long long int, FriendRectScope> friendRectInfoMap;
};

class ISensorVedioServiceImpl : public ISensorVedioService
{
public:
	ISensorVedioServiceImpl(void);
    ~ISensorVedioServiceImpl(void);

	// 单例
	static ISensorVedioServiceImpl& Get();

	// 添加标识对应的播放地址(线程不安全)
	void AddUrl(long long identity, const QString& url) override;

	// 获取标识对应的Url(线程不安全)
	QString GetUrl(long long identity)override;

	// 添加数据(线程安全)
	void AddPixMap(long long identity, const QByteArray& map)override;

	// 添加数据(线程安全)
	void AddPixMap(long long identity, const char* szData, size_t len)override;

	// 添加数据(线程安全)
	void AddPixMap(long long identity, AVFrame* pt)override;

	// 同一提出最后一个数据
	void PopPixMap() override;

	// 获取最新的数据(线程安全)会有30ms以下的耗时
	bool GetLastPixMap(long long identity, QPixmap& map) override;

	// 获取最新的数据(线程安全)会有30ms以下的耗时
	bool GetLastImage(long long identity, QImage& map) override;

	// 获取最新的数据(线程安全) 较少耗时 pAvframe自行析构
	bool GetLastPixMap(long long identity, AVFrame** pAvframe, QByteArray& array)override;

	// 获取最后一帧的Byte数据
	 bool GetLastPixMapData(long long identity, QByteArray& array)override;

	// 超时处理(线程安全)
	void OnTimeOut();

	// 获取激活视频的ID(线程安全)
	void GetActiveIdentitys(QList<long long>& identityList)override;

	// 获取激活视频的区域(线程安全)
	virtual void GetComponentTargets(long long identity, QList<XygsZC_Mark_01>& targets)override;

	// 发送更新组件的目标列表(线程安全)
	virtual void SendUpdateComponentTargets(long long identity, std::list<long long>& targetIdentityList)override;

	// 发送更新组件目标信息
	virtual void SendUpdateComponentTargetData(long long identity, XygsZC_Mark_01& data)override;

	// 发送更新组件目标信息
	virtual void SendClearComponentTargets(long long identity)override;

	// 接收更新组件的目标列表
	void RecvUpdateComponentTargets(char* data, size_t size);

	// 接收更新组件目标信息
	void RecvUpdateComponentTargetData(char* data, size_t size);

	// 接收更新组件目标信息
	void RecvClearComponentTargets(char* data, size_t size);

	// 发送更新组件友方 (线程安全)
	virtual void SendUpdateComponentFriendScope(long long identity, std::list<FriendRectScope>& list)override;

	// 发送更新清空友方 (线程安全)
	virtual void SendUpdateComponentClearFriendScope(long long identity)override;

	// 获取更新组件友方(线程安全)
	virtual void GetComponentFriendScopes(long long identity, QList<FriendRectScope>& list)override;

	// 接收更新组件的友方列表
	void RecvUpdateComponentFriends(char* data, size_t size);

	// 接收更新组件友方信息
	void RecvUpdateComponentFriendData(char* data, size_t size);

	// 接收更新组件友方信息
	void RecvClearComponentFriend(char* data, size_t size);

public:
	// 清空组(线程安全)
	void RemoveComponentTargetIDs(long long identity);

	// 更新组件地方区域(线程安全)
	void UpdateComponentTargetIDs(long long identity, std::list<long long>& targetIDs);

	// 设置组件目标区域(线程安全)
	void SetComponentTargetRect(const XygsZC_Mark_01& target);

	// 清空组(线程安全)
	void RemoveComponentFriendIDs(long long identity);

	// 更新组件友方区域(线程安全)
	void UpdateComponentFrinedIDs(long long identity, std::list<long long>& targetIDs);

	// 设置组件友方区域(线程安全)
	void SetComponentFriendRect(const FriendRectScope& target);

private: 
	std::shared_ptr<std::atomic_flag>  m_flag;						// 标识
	QMap<long long, VedioInfo> m_Identity2QueueMap;					// 
	QMap<long long, DataFrameInfoSPtr> m_lastIdentityStatusMap;		//
	QMap<long long, QString> m_identity2PlayVedioUrlMap;			//
	QMap<long long, TargetRectInfo> m_IdentityTargetStatusMap;		// 目标
	QMap<long long, FriendRectInfo> m_IdentityFriendStatusMap;		// 友方状态
};
}
}

#endif
