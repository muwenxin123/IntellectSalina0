#include "EntityVedioData.h"

#include <QDebug>
#include <chrono>
#include <thread>
#include <QFuture>
#include <QFutureWatcher>
#include <QReadLocker>
#include <QWriteLocker>
#include <QThreadPool>

#include "EntityVedioData.h"
#include "IFunctionInterface.h"
#include "Ly_VideoRealTimePreviewInterface.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libavutil/hwcontext.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}
#include "ConvertTask.h"

EntityVedioData::EntityVedioData(long long identity)
{
	m_identity = identity;
	std::string WeaponComponentName;
	bool isGetWeaponComponentName = IVideoRealTimePreviewActivator::Get().getPWeaponManageService()->GetWeaponComponentName(WeaponComponentName, QString::number(identity).toStdString());
	if (isGetWeaponComponentName)
	{
		m_weaponName = QString::fromStdString(WeaponComponentName);
	}
	else
	{
		m_weaponName = QString::number(identity);
	}
	m_targetStructSize = sizeof(XygsZC_Mark_01);
}

EntityVedioData::~EntityVedioData()
{
	if (nullptr != m_pAvframe)
	{
		av_frame_free(&m_pAvframe);
		m_pAvframe = nullptr;
	}
}

void EntityVedioData::addShowFlag()
{
	QWriteLocker locker(&m_lock);
	m_showCount++;
}

void EntityVedioData::reduceShowFlag()
{
	QWriteLocker locker(&m_lock);
	m_showCount--;
}

bool EntityVedioData::needShow()
{
	return m_showCount > 0;
}

bool EntityVedioData::isChanged()
{
	QReadLocker locker(&m_lock);
	return m_isChanged;
}

void EntityVedioData::resetChangedFlag()
{
	QWriteLocker locker(&m_lock);
	m_isChanged = false;
}

// 获取实体ID
long long EntityVedioData::GetIdentity()
{
	QReadLocker locker(&m_lock);
	return m_identity;
}

QString EntityVedioData::GetAreaInfo()
{
	return m_areaID;
}

void EntityVedioData::onUpdateData()
{
	//static auto s_last = std::chrono::system_clock::now();
	//static auto s_now = std::chrono::system_clock::now();
	//static uint s_maxCustom = 0;
	//static uint s_maxIntervel = 0;
	//uint intervelTime = 0;
	//uint curstm = 0;
	//auto s_now = std::chrono::system_clock::now();

	auto pVideoService = IVideoRealTimePreviewActivator::Get().getPISensorVedioService();
	if (nullptr != pVideoService)
	{
		//intervelTime = (std::chrono::duration_cast<std::chrono::milliseconds>(s_now - s_last).count());

		//if (pVideoService->GetLastPixMap(m_identity, &m_pAvframe, m_picData))
		//{
		//	QWriteLocker locker(&(this->m_lock));
		//	this->m_isChanged = true;
		//}

		QPixmap map;
		if (pVideoService->GetLastPixMap(m_identity, map))
		{
			QWriteLocker locker(&(this->m_lock));
			this->m_map = map;
			this->m_isChanged = true;
		}

		//std::thread([this, pService = pVideoService](){
		////auto s_now = std::chrono::system_clock::now();
		//QPixmap map;
		//if (pService->GetLastPixMap(m_identity, map))
		//{
		//	//auto s_last = std::chrono::system_clock::now();
		//	//auto curstm = (std::chrono::duration_cast<std::chrono::milliseconds>(s_last - s_now).count());
		//	//qDebug() << "Max _Custom:" << "1"
		//	//	<< "EntityVedioData::onUpdateData()_Custom" << curstm << "-----2";
		//	//
		//	QWriteLocker locker(&(this->m_lock));
		//	this->m_map = map;
		//	this->m_isChanged = true;
		//}
		//}).detach();


		QList<XygsZC_Mark_01> targets;
		pVideoService->GetComponentTargets(m_identity, targets);
		auto targetVec = targets.toVector();
		{
			QWriteLocker locker(&m_lock);
			m_targets = targetVec.toStdVector();
			m_isChanged = true;
		}

		QList<qnzkna::SensorVedio::FriendRectScope> friendList;
		pVideoService->GetComponentFriendScopes(m_identity, friendList);
		auto friendVec = friendList.toVector().toStdVector();
		{
			QWriteLocker locker(&m_lock);
			m_friendScopeVec.swap(friendVec);
			m_isChanged = true;
		}
	}

	auto& pFunc = Ly_VideoRealTimePreviewInterface::Get().GetFunctionInterface();
	auto ret = pFunc.Invoke("Anzkna.AgriSoilAnalysisPredict.GetAreaID",  QPointF(m_lon, m_lat));
	if (!ret.isNull())
	{
		QWriteLocker locker(&m_lock);
		m_areaID = ret.toString();
	}

	//auto s_last = std::chrono::system_clock::now();
	//auto curstm = (std::chrono::duration_cast<std::chrono::milliseconds>(s_last - s_now).count());
	//qDebug() << "Max _Custom:" << "1"
	//	<< "EntityVedioData::onUpdateData()_Custom" << curstm << "-----2";
	//s_maxCustom = qMax(curstm, s_maxCustom);
	//s_maxIntervel = qMax(intervelTime, s_maxIntervel);
	//qDebug() << "Max _Custom:" << s_maxCustom
	//	<< "EntityVedioData::onUpdateData()_Custom" << curstm << "-----2";
	//qDebug() << "Max Indervel:" << s_maxIntervel
	//	<< "EntityVedioData::onUpdateData()_intervel" << intervelTime << "-----1";
}

void EntityVedioData::setDataMap(QVariantMap map)
{
	QWriteLocker locker(&m_lock);
	if (map.contains("Distance"))
		m_distance = map["Distance"].toDouble();
	else
		m_distance = -1;

	if (map.contains("Lon"))
		m_lon = map["Lon"].toDouble();
	else
		m_lon = -10000;

	if (map.contains("Lat"))
		m_lat = map["Lat"].toDouble();
	else
		m_lat = -10000;

	if (map.contains("Yaw"))
	{
		m_yaw = map["Yaw"].toDouble();
	}
	else
	{
		m_yaw = 0;
	}
}

// 获取图片信息
QPixmap& EntityVedioData::GetPixMap(QPixmap& map)
{
	QReadLocker locker(&m_lock);
	map = m_map.copy();
	return map;
}

// 获取sdl数据 pAvframe需要自行析构
void EntityVedioData::GetSDLData(AVFrame** pAvframe, QByteArray& array)
{
	QReadLocker locker(&m_lock);
	if (nullptr != m_pAvframe)
		*pAvframe = av_frame_clone(m_pAvframe);
	array = m_picData;
}

// 获取目标信息
std::vector<XygsZC_Mark_01>& EntityVedioData::GetTargetInfo(std::vector<XygsZC_Mark_01>& targetInfoVec)
{
	QReadLocker locker(&m_lock);
	size_t size = m_targets.size();
	targetInfoVec.clear();
	targetInfoVec.resize(size);
	std::copy(m_targets.begin(), m_targets.end(), targetInfoVec.begin());
	return targetInfoVec;
}

// 获取目标名称
void EntityVedioData::GetEntityName(QString& weaponName)
{
	QReadLocker locker(&m_lock);
	weaponName = m_weaponName;
}

// 获取目标距离
void EntityVedioData::GetEntityDistance(double& distance)
{
	QReadLocker locker(&m_lock);
	distance = m_distance;
}

// 获取目标位置
void EntityVedioData::GetEntityPos(double& lon, double& lat)
{
	QReadLocker locker(&m_lock);
	lon = m_lon;
	lat = m_lat;
}

// 获取友方信息
std::vector<qnzkna::SensorVedio::FriendRectScope>& EntityVedioData::GetFriendInfo(std::vector<qnzkna::SensorVedio::FriendRectScope>& friendInfoVec)
{
	QReadLocker locker(&m_lock);
	size_t size = m_friendScopeVec.size();
	friendInfoVec.clear();
	friendInfoVec.resize(size);
	std::copy(m_friendScopeVec.begin(), m_friendScopeVec.end(), friendInfoVec.begin());
	return friendInfoVec;
}


double EntityVedioData::GetEntityYaw()
{
	QReadLocker locker(&m_lock);
	return m_yaw;
}

void EntityVedioData::SetCameraInfo(double yaw, double pitch, double roll)
{
	QWriteLocker locker(&m_lock);
	m_cameraPitchValue = pitch;
	m_cameraYawValue = yaw;
	m_cameraRollValue = roll;
}

void EntityVedioData::GetCameraInfo(double& yaw, double& pitch, double& roll)
{
	QReadLocker locker(&m_lock);
	pitch = m_cameraPitchValue;
	yaw = m_cameraYawValue;
	roll = m_cameraRollValue;
}

EntityDataManage& EntityDataManage::Get()
{
	static EntityDataManage s_EntityDataManage;
	return s_EntityDataManage;;
}

// 增加实体
void EntityDataManage::AddEntity(EntityVedioDataSPtr entity)
{
	if (nullptr == entity)
		return;

	if (!IsContains(entity->GetIdentity()))
	{
		QWriteLocker locker(&m_lock);
		m_id2EntityDataMap.insert(entity->GetIdentity(), entity);
	}
	else
	{
		QWriteLocker locker(&m_lock);
		m_id2EntityDataMap[entity->GetIdentity()] = entity;
	}
}

// 移除实体
void EntityDataManage::RemoveEntity(long long identity)
{
	QWriteLocker locker(&m_lock);
	auto iter = m_id2EntityDataMap.find(identity);
	if (iter != m_id2EntityDataMap.end())
	{
		m_id2EntityDataMap.erase(iter);
	}
}

// 查找实体
EntityVedioDataWPtr EntityDataManage::FindEntity(long long identity)
{
	EntityVedioDataWPtr entityPtr;

	QReadLocker locker(&m_lock);
	auto iter = m_id2EntityDataMap.find(identity);
	if (iter != m_id2EntityDataMap.end())
		entityPtr = *iter;
	
	return entityPtr;
}

// 获取实体列表
QList<EntityVedioDataSPtr> EntityDataManage::GetEntityList()
{
	QList<EntityVedioDataSPtr> list;
	QReadLocker locker(&m_lock);
	for (auto iter = m_id2EntityDataMap.begin(); iter != m_id2EntityDataMap.end(); iter++)
	{
		list.append(*iter);
	}
	return list;
}

// 是否包含
bool EntityDataManage::IsContains(long long identity)
{
	QReadLocker locker(&m_lock);
	auto iter = m_id2EntityDataMap.find(identity);
	return iter != m_id2EntityDataMap.end();
}

void EntityDataManage::ClearAll()
{
	QReadLocker locker(&m_lock);
	m_id2EntityDataMap.clear();
}

EntityDataManage::EntityDataManage()
{

}

EntityDataManage::~EntityDataManage()
{
	{
		QWriteLocker locker(&m_lock);
		m_id2EntityDataMap.clear();
	}
}
