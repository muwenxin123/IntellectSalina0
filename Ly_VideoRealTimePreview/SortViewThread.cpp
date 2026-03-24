#include "SortViewThread.h"
#include <array>
#include <cmath>
#include <QtMath>
#include <chrono>
#include <QDebug>
#include <QThread>
#include <QSettings>
#include <QGeoCoordinate>
#include <QMatrix3x3>
#include <QThreadPool>
#include "ConvertTask.h"
#include "Ly_VideoRealTimePreview_global.h"

// WGS84椭球参数
static constexpr double WGS84_A = 6378137.0;          // 长半轴(米)
static constexpr double WGS84_F = 1.0 / 298.257223563; // 扁率
static constexpr double WGS84_E2 = 2 * WGS84_F - WGS84_F * WGS84_F; // 第一偏心率的平方

#include "EntityVedioData.h"
#include "Ly_VideoRealTimePreview.h"

struct EntityInfo {
	long long identity = 0;
	double lon = 0;
	double lat = 0;
	double alt = 0;
	double yaw = 0;
	double distanceToBase = 0.0;
	double angleToBase = 0.0;

	inline bool operator<(const EntityInfo &rhs)	 const
	{
		if (qFuzzyCompare(this->angleToBase, rhs.angleToBase)) {
			return this->distanceToBase < rhs.distanceToBase;
		}
		return this->angleToBase < rhs.angleToBase;
	}
};


SortViewThread::SortViewThread(QObject * object)
{
	connect(&m_timer, &QTimer::timeout, this, &SortViewThread::onTimeOut);

	Init();
	onConfigureChanged();

	QThread* thread = new QThread();
	this->moveToThread(thread);
	thread->start();

	m_timer.start(120);
}

SortViewThread::~SortViewThread()
{
	qDebug() << "SortViewThread_start";
	auto time = std::chrono::high_resolution_clock::now();

	if (m_timer.isActive())
	{
		m_timer.stop();
	}

	if (thread()->isRunning())
	{
		thread()->quit();
		thread()->wait();
		delete thread();
	}

	auto last = std::chrono::high_resolution_clock::now();
	double curCoustom = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - time)).count();
	qDebug() << "SortViewThread _ deleteCoustom" << curCoustom;
}

void SortViewThread::onConfigureChanged()
{
	auto pViedioService = IVideoRealTimePreviewActivator::Get().getPISensorVedioService();
	if (pViedioService == nullptr) {
		return;
	}

	QMutexLocker locker(&(Ly_VideoRealTimePreview::Get().getConfigMutex()));
	auto pConfigure = Ly_VideoRealTimePreview::Get().getConfig();
	if (nullptr == pConfigure)
		return;
	m_isUseSort = pConfigure->useSortByPosAndYawFlag;
	m_sortCenterLon = pConfigure->dLon;
	m_sortCenterLat = pConfigure->dLat;
	m_sortCenterYaw = pConfigure->dYaw;
	m_cameraPitchValue = 0.0;
	m_cameraYawValue = 0.0;
	m_cameraRollValue = 0.0;

	if (m_isUserFriendTrack != pConfigure->useFriendTrackFlag || (m_observerID != pConfigure->lObserverIdentity && m_observerID > 0))
	{
		pViedioService->SendUpdateComponentClearFriendScope(m_observerID);
		m_lastFriendSize = 0;
	}

	m_isUserFriendTrack = pConfigure->useFriendTrackFlag;
	m_observerID = pConfigure->lObserverIdentity;
}

void SortViewThread::onFocalZoomChanged(int imageZoom)
{
	m_focalZoom = imageZoom;
}

void SortViewThread::onCameraAttributeChanged(long long identity, double yawValue, double pitchValue, double rollValue)
{
	if (identity == m_observerID)
	{
		m_cameraPitchValue = pitchValue;
		m_cameraYawValue = yawValue;
		m_cameraRollValue = rollValue;
	}
}

void SortViewThread::Init()
{
	QSettings settings("Data/conf/PlayYoloVideoPodConfig.ini", QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("PlayYoloVideoPodConfig");
	m_cameraInfo.minHFov = settings.value("ViewLeftMin", 6.8).toDouble();
	m_cameraInfo.minVFov = settings.value("ViewFrontMin", 3.9).toDouble();
	m_cameraInfo.maxHFov = settings.value("ViewLeftMax", 61.5).toDouble();
	m_cameraInfo.maxVFov = settings.value("ViewFrontMax", 6.8).toDouble();
	m_cameraInfo.maxZoom = settings.value("MaxZoom", 1).toInt();

	if (m_cameraInfo.maxZoom > 1)
	{
		m_cameraInfo.hFovSpace = (m_cameraInfo.minHFov - m_cameraInfo.maxHFov) / (m_cameraInfo.maxZoom - 1);
		m_cameraInfo.vFovSpace = (m_cameraInfo.minVFov - m_cameraInfo.maxVFov) / (m_cameraInfo.maxZoom - 1);
	}
	settings.endGroup();


	QSet<long long> idSet;
	qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
	auto pISystemMaintenanceService = IVideoRealTimePreviewActivator::Get().getISystemMaintenanceService();
	auto pService = IVideoRealTimePreviewActivator::Get().getPWeaponManageService();
	if (pService == nullptr || nullptr == pISystemMaintenanceService)
		return;

	if (pISystemMaintenanceService->GetWeaponSystems(&weaponSystemMap))
	{
		for (const auto &weaponSystemIter : weaponSystemMap)
		{
			const auto &mapWeaponComponent = weaponSystemIter.second.getMapWeaponComponent();
			for (const auto &mapWeaponComponentIter : mapWeaponComponent)
			{
				const qnzkna::WeaponManage::WeaponComponent &weaponComponent = (mapWeaponComponentIter.second);
				bool ok = false;
				const unsigned long long componentID = QString::fromStdString(weaponComponent.getComponentID()).toULongLong(&ok);
				if (ok)
				{
					switch (weaponSystemIter.second.getSystemType())
					{
					case qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_UAV:
					case qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_SOLDIER:
				//	case qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_WOLF:
						idSet.insert(componentID);
						break;
					default:
						break;
					}
				}
			}
		}
	}

	std::string weaponName;
	m_id2NameMap.clear();
	for (auto& id : idSet)
	{
		if (pService->GetWeaponComponentName(weaponName, QString::number(id).toStdString()))
		{
			m_id2NameMap.insert(id, QString::fromStdString(weaponName));
		}
		else
		{
			m_id2NameMap.insert(id, QString::number(id));
		}
	}
}

QVector3D SortViewThread::llaToEcef(double lon, double lat, double alt)
{
	// 将角度转换为弧度
	double lat_rad = qDegreesToRadians(lat);
	double lon_rad = qDegreesToRadians(lon);

	// 计算卯酉圈曲率半径
	double sin_lat = qSin(lat_rad);
	double N = WGS84_A / qSqrt(1.0 - WGS84_E2 * sin_lat * sin_lat);

	// 计算ECEF坐标
	double cos_lat = qCos(lat_rad);
	double cos_lon = qCos(lon_rad);
	double sin_lon = qSin(lon_rad);

	double X = (N + alt) * cos_lat * cos_lon;
	double Y = (N + alt) * cos_lat * sin_lon;
	double Z = (N * (1.0 - WGS84_E2) + alt) * sin_lat;

	return QVector3D(X, Y, Z);
}

QVector3D SortViewThread::ecefToEnu(const QVector3D& ecef_delta, double lon_ref, double lat_ref)
{
	const double lon_rad = qDegreesToRadians(lon_ref);
	const double lat_rad = qDegreesToRadians(lat_ref);
	const double sin_lon = std::sin(lon_rad);
	const double cos_lon = std::cos(lon_rad);
	const double sin_lat = std::sin(lat_rad);
	const double cos_lat = std::cos(lat_rad);

	return QVector3D(
		-sin_lon * ecef_delta.x() + cos_lon * ecef_delta.y(),
		-sin_lat * cos_lon * ecef_delta.x() - sin_lat * sin_lon * ecef_delta.y() + cos_lat * ecef_delta.z(),
		cos_lat * cos_lon * ecef_delta.x() + cos_lat * sin_lon * ecef_delta.y() + sin_lat * ecef_delta.z()
	);
}

/* 输入参数 */
bool SortViewThread::computeTargetCornersWithPitch(
	double lon_observer, double lat_observer, double alt_observer, 
	double yaw_observer, double pitch_observer, /* 新增pitch角（度） */ 
	double lon_target, double lat_target, double alt_target, double width_target, double height_target, 
	double yaw_target, double hFov, double vFov, int image_width, int image_height, 
	qnzkna::SensorVedio::Point* top_left, qnzkna::SensorVedio::Point* top_right, 
	qnzkna::SensorVedio::Point* bottom_left, qnzkna::SensorVedio::Point* bottom_right
	,double& deepScan) /* 输出参数 */
{
	// === 阶段1：坐标转换 ===
	double enuDxRad = qDegreesToRadians(lon_target - lon_observer);
	double enuDyRad = qDegreesToRadians(lat_target - lat_observer);
	double cHeight = alt_target - alt_observer;

	double enuDx = enuDxRad * 6371393.0 * std::cos(qDegreesToRadians(lat_observer)); // 东西x
	double enuDy = enuDyRad * 6371393.0;		// 南北y

	// === 阶段2：机体坐标系转换 ===
	const double yaw_rad = qDegreesToRadians(yaw_observer);
	const double cos_yaw = std::cos(yaw_rad);
	const double sin_yaw = std::sin(yaw_rad);

	// 计算FRD分量
	const double F = enuDy * cos_yaw + enuDx * sin_yaw - 0.2; // Forward x 机头正北
	const double R = -enuDy * sin_yaw + enuDx * cos_yaw; // Right  机翼
	const double D = -cHeight + 0.2; // Down z	// 下方


	// === 阶段3：摄像头坐标系旋转（Pitch）===
	const double pitch_rad = qDegreesToRadians(pitch_observer);
	const double cos_pitch = std::cos(pitch_rad);
	const double sin_pitch = std::sin(pitch_rad);

	// 转换为RDF坐标系 
	const double R_cam = R;									// 相机右方
	const double D_cam = F * cos_pitch - D * sin_pitch;		// 相机前方
	const double F_cam = F * sin_pitch + D * cos_pitch;	 	// 相机下方

	// 相机后方
	if (D_cam < 0) return false;

	// === 阶段4：计算目标四个角点 ===
	const double yaw_t_rad = qDegreesToRadians(yaw_target);
	const double half_w = width_target * 0.5;
	const double half_h = height_target * 0.5;

	// 预计算目标本地坐标系到ENU的旋转项
	const double cos_yaw_t = std::cos(yaw_t_rad);
	const double sin_yaw_t = std::sin(yaw_t_rad);

	// 四个角点在目标本地坐标系的偏移（顺序：TL, TR, BL, BR）
	const std::array<std::pair<double, double>, 4> local_offsets = {
		std::make_pair(-half_w, -half_h),
		std::make_pair(half_w, -half_h),
		std::make_pair(-half_w,  half_h),
		std::make_pair(half_w,  half_h)
	};

	// 转换为ENU坐标系并旋转到机体坐标系
	std::array<QVector3D, 4> corners;
	for (int i = 0; i < 4; ++i) {
		const double dx = local_offsets[i].first;
		const double dy = local_offsets[i].second;

		// 目标本地→ENU
		const double e = dx * sin_yaw_t + dy * cos_yaw_t;	// 
		const double n = dx * cos_yaw_t - dy * sin_yaw_t;	// 

		// ENU→机体坐标系（FRD）
		const double F_local = n * cos_yaw + e * sin_yaw + F;	// 前向
		const double R_local = -n * sin_yaw + e * cos_yaw + R;	// 右向
		const double D_local = D;						    // 下向

		// 机体坐标系→摄像头坐标系(RDF)（考虑pitch）
		corners[i].setX(R_local);										// 相机右方	
		corners[i].setY(F_local * cos_pitch - D_local * sin_pitch);		// 相机前方
		corners[i].setZ(F_local * sin_pitch + D_local * cos_pitch);		// 相机下方
	}

	// === 阶段5：透视投影 ===
	const double hFov_half_rad = qDegreesToRadians(hFov) * 0.5;
	const double vFov_half_rad = qDegreesToRadians(vFov) * 0.5;

	// 计算归一化图像坐标（-1到1范围）
	// 坐标为RDF
	std::array<double, 4> x_norm, y_norm;
	for (int i = 0; i < 4; ++i) {
		x_norm[i] = std::atan2(corners[i].x(), corners[i].y()) / hFov_half_rad;
		y_norm[i] = std::atan2(-corners[i].z(), corners[i].y()) / vFov_half_rad;
	}
	// 计算包围盒
	const auto x_extremes = std::minmax_element(x_norm.begin(), x_norm.end());
	const auto y_extremes = std::minmax_element(y_norm.begin(), y_norm.end());

	const double x_min = *x_extremes.first;
	const double x_max = *x_extremes.second;
	const double y_min = *y_extremes.first;
	const double y_max = *y_extremes.second;

	// 视锥体裁剪
	if (x_max < -1.0 || x_min > 1.0 || y_max < -1.0 || y_min > 1.0) {
		return false;
	}

	// === 阶段6：转换为像素坐标 ===
	const double pixel_per_x = image_width * 0.5;
	const double pixel_per_y = image_height * 0.5;

	*top_left = {
		(x_min + 1.0) * pixel_per_x,
		(1.0 - y_max) * pixel_per_y,
		0.0
	};

	*top_right = {
		(x_max + 1.0) * pixel_per_x,
		(1.0 - y_max) * pixel_per_y,
		0.0
	};

	*bottom_left = {
		(x_min + 1.0) * pixel_per_x,
		(1.0 - y_min) * pixel_per_y,
		0.0
	};

	*bottom_right = {
		(x_max + 1.0) * pixel_per_x,
		(1.0 - y_min) * pixel_per_y,
		0.0
	};

	// 大于1个像素
	double hPixCount = bottom_right->dx - top_left->dx;
	double vPixCount = bottom_right->dy - top_left->dy;
	if (hPixCount* vPixCount < 1.0)
		return false;

	// 深度比例 20m/D_cam米比例
	deepScan = 20.0 /(D_cam);
	return true;
}

double SortViewThread::CalculateDistance(double lon1, double lat1, double lon2, double lat2)
{
	QGeoCoordinate pt1(lat1, lon1);
	QGeoCoordinate pt2(lat2, lon2);
	return pt1.distanceTo(pt2);
}

double SortViewThread::CalculateAngle(const double& baseLon, const double& baseLat, double baseAngle, const double& targetLon, const double& targetLat)
{
	QGeoCoordinate pointPrev(baseLat, baseLon);
	QGeoCoordinate pointNext(targetLon, targetLat);
	double bearing = pointPrev.azimuthTo(pointNext);

	double angleDiff = LimitAngle(bearing - baseAngle);
	return angleDiff > 180.0 ? angleDiff - 360.0 : angleDiff;
}

double SortViewThread::LimitAngle(double angle)
{
	while (angle < 0)
	{
		angle += 360.0;
	}

	while (angle > 360.0)
	{
		angle -= 360.0;
	}
	return angle;
}

bool SortViewThread::CalcEquipRectInMapVideo(QGeoCoordinate lookUav, double lookHeading, double ViewLeft, double ViewFront, QGeoCoordinate showUav, qnzkna::SensorVedio::FriendRectScope& scope)
{
	double videoWidth = scope.videoWidth;
	double videoHeight = scope.videoHeight;

	//高度差
	double cHeight = lookUav.altitude() - showUav.altitude() - 0.2;
	if (cHeight <0 || cHeight > 200)
	{
		return false;
	}

	//实际覆盖距离
	double LeftAngleHalf = ViewLeft / 2;   // 30.65
	double FrontAngleHalf = ViewFront / 2; //18.75

	double longDis = std::abs(cHeight * tan(qDegreesToRadians(LeftAngleHalf))); // 长度  30.65
	double widthDis = std::abs(cHeight * tan(qDegreesToRadians(FrontAngleHalf))); // 宽度  18.75

	double enuDxRad = qDegreesToRadians(showUav.longitude() - lookUav.longitude());
	double enuDyRad = qDegreesToRadians(showUav.latitude() - lookUav.latitude());

	const double cos_yaw = std::cos(qDegreesToRadians(lookHeading));
	const double sin_yaw = std::sin(qDegreesToRadians(lookHeading));

	double enuDx = enuDxRad * 6371393.0 * std::cos(qDegreesToRadians(lookUav.latitude()));
	double enuDy = enuDyRad * 6371393.0;
	
	double dx = enuDx *cos_yaw - enuDy * sin_yaw; // （米）
	double dy = enuDx * sin_yaw + enuDy * cos_yaw - 0.2; // (米)
	if (std::fabs(dx) > longDis || std::fabs(dy) > widthDis)
		return false;

	double relateX = (videoWidth / 2.0) + (videoWidth / 2.0) / longDis * (dx - 1.0); // 960 / 实宽度m
	double relateY = (videoHeight / 2.0) - (videoHeight / 2.0) / widthDis * (dy + 1.0);
	
	//框大小-暂按2米*2米
	double rectLon = 2 / longDis * (videoWidth / 2);
	double rectWidth = 2 / widthDis *(videoHeight / 2);

	scope.rectX = relateX;
	scope.rectY = relateY;
	scope.rectWidth = rectLon;
	scope.rectHeight = rectWidth;
	return true;
}

void SortViewThread::GetCurFov(const CameraInfo& info, int ratio, double& targetHFov, double& targetVFov)
{
	targetHFov = info.maxHFov + info.hFovSpace * (ratio - 1);
	targetVFov = info.maxVFov + info.vFovSpace * (ratio - 1);
}

void SortViewThread::UpdateFriendRect()
{
	//static auto s_last = std::chrono::system_clock::now();
	//static auto s_now = std::chrono::system_clock::now();
	//static uint s_maxCustom = 0;
	//static uint s_maxIntervel = 0;

	//s_now = std::chrono::system_clock::now();
	//uint intervelTime = (std::chrono::duration_cast<std::chrono::microseconds>(s_now - s_last).count());
	//s_maxIntervel = qMax(intervelTime, s_maxIntervel);
	//qDebug() << "Max Indervel:" << s_maxIntervel
	//	<< "cur_SortViewThread::UpdateFriendRect()_intervel" << intervelTime << "-----1";

	auto pService = IVideoRealTimePreviewActivator::Get().getPIUAVRealTimeStateManageService();
	auto pViedioService = IVideoRealTimePreviewActivator::Get().getPISensorVedioService();
	auto pTaskManageService = IVideoRealTimePreviewActivator::Get().getPTaskManageService();
	auto pTargetManageService = IVideoRealTimePreviewActivator::Get().getPZKLYTargetManageService();

	// 无需显示边框
	if (!m_isUserFriendTrack || m_observerID <= 0 || nullptr == pService || nullptr == pViedioService)
		return;
	
#ifdef USE_BRIEF_CALAC

#endif
	
	qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t observerData;
	if (!pService->GetUAVRealTimeState(QString::number(m_observerID).toStdString(), observerData))
	{
		if (m_lastFriendSize != 0)
		{
			pViedioService->SendUpdateComponentClearFriendScope(m_observerID);
			m_lastFriendSize = 0;
		}
		return;
	}

	QGeoCoordinate lookUav(observerData.telemetryInfo.position.lat, observerData.telemetryInfo.position.lng, observerData.telemetryInfo.relativeGroundHeigth);
	double lookHeading = observerData.telemetryInfo.angleofRudder;
	
	double targetHFov = 0.0;
	double targetVFov = 0.0;
	GetCurFov(m_cameraInfo, m_focalZoom, targetHFov, targetVFov);
	if (targetHFov < 0 || targetHFov < 0)
	{ 
		return;
	}

	std::list<qnzkna::SensorVedio::FriendRectScope> targetList;
	qnzkna::SensorVedio::FriendRectScope targetData;
	targetData.componentID = m_observerID;
	targetData.videoWidth = 1920;
	targetData.videoHeight = 1080;

	std::map<std::string, qnzkna::UAVRealTimeStateManage::TelemetryInformation> targetMap;
	pService->GetUAVRealTimeStateList(&targetMap);

	for (auto iter = targetMap.begin(); iter != targetMap.end(); iter++)
	{
		long long targetID = QString::fromStdString(iter->first).toLongLong();
		auto& info = iter->second.m_telemetryInfo;
		if (m_observerID == targetID)
			continue;
		
		QGeoCoordinate friendTarget(info.telemetryInfo.position.lat, info.telemetryInfo.position.lng, info.telemetryInfo.relativeGroundHeigth);
#ifdef USE_BRIEF_CALAC
		if (CalcEquipRectInMapVideo(lookUav, lookHeading, targetHFov, targetVFov, friendTarget, targetData))
#else
		if (computeTargetCornersWithPitch(
			observerData.telemetryInfo.position.lng, observerData.telemetryInfo.position.lat, qMax(0.0, observerData.telemetryInfo.relativeGroundHeigth),
			observerData.telemetryInfo.angleofRudder + m_cameraYawValue, m_cameraPitchValue,
			info.telemetryInfo.position.lng, info.telemetryInfo.position.lat, qMax(0.0, info.telemetryInfo.relativeGroundHeigth),
			2, 2, info.telemetryInfo.angleofRudder, targetHFov, targetVFov, targetData.videoWidth, targetData.videoHeight,
			&(targetData.topLeft), &(targetData.topRight), &(targetData.bottomLeft), &(targetData.bottomRight), targetData.deepScan))
#endif
		{
			targetData.targetID = targetID;
			strcpy(targetData.targetName, m_id2NameMap[targetID].toLocal8Bit().data());
			targetData.targetHeight = qMax(0.0, info.telemetryInfo.relativeGroundHeigth);
			targetData.distance = lookUav.distanceTo(friendTarget);
			targetList.push_back(targetData);
		}
	}
//
//	if (nullptr != pTaskManageService && nullptr != pTargetManageService)
//	{
//		auto targetDataList = pTaskManageService->GetRelationTarget(pTaskManageService->GetActivatingActionID());
//		for (auto& relateTargetInfo : targetDataList)
//		{
//			auto targetInfo = pTargetManageService->GetTarget(relateTargetInfo.GetTargetID());
//			if (targetInfo.GetID().empty())
//				continue;
//
//			QGeoCoordinate targetPos(targetInfo.GetLat(), targetInfo.GetLon(), targetInfo.GetHeight());
//#ifdef USE_BRIEF_CALAC
//			if (CalcEquipRectInMapVideo(lookUav, lookHeading, targetHFov, targetVFov, targetPos, targetData))
//#else
//			if (computeTargetCornersWithPitch(
//				observerData.telemetryInfo.position.lng, observerData.telemetryInfo.position.lat, qMax(0.0, observerData.telemetryInfo.relativeGroundHeigth),
//				observerData.telemetryInfo.angleofRudder + m_cameraYawValue, m_cameraPitchValue,
//				targetInfo.GetLon(), targetInfo.GetLat(), qMax(0.0, targetInfo.GetHeight()),
//				2, 2, 0, targetHFov, targetVFov, targetData.videoWidth, targetData.videoHeight,
//				&(targetData.topLeft), &(targetData.topRight), &(targetData.bottomLeft), &(targetData.bottomRight),targetData.deepScan))
//#endif
//			{
//				targetData.targetID = qHash(QString::fromStdString(targetInfo.GetID()));
//				strcpy(targetData.targetName, QString::fromStdString(targetInfo.GetName()).toLocal8Bit().data());
//				targetData.targetHeight = qMax(0.0, targetInfo.GetHeight());
//				targetData.distance = lookUav.distanceTo(targetPos);
//				targetData.sideType = 1;
//				targetList.push_back(targetData);
//			}
//		}
//	}
//
	if (!targetList.empty())
	{
		targetList.sort([](const qnzkna::SensorVedio::FriendRectScope& prevData, const qnzkna::SensorVedio::FriendRectScope& nextData)->bool {
			return prevData.targetHeight > nextData.targetHeight;
		});

		pViedioService->SendUpdateComponentFriendScope(m_observerID, targetList);
		m_lastFriendSize = targetList.size();
	}
	else 
	{
		if (m_lastFriendSize != 0)
		{
			pViedioService->SendUpdateComponentClearFriendScope(m_observerID);
			m_lastFriendSize = 0;
		}
	}

	//s_last = std::chrono::system_clock::now();
	//uint curstm = (std::chrono::duration_cast<std::chrono::microseconds>(s_last - s_now).count());
	//s_maxCustom = qMax(curstm, s_maxCustom);
	//qDebug() << "Max _Custom:" << s_maxCustom
	//	<< "SortViewThread::UpdateFriendRect()_Custom" << curstm << "-----2";
}

void SortViewThread::SortProcess()
{
	auto pService = IVideoRealTimePreviewActivator::Get().getPIUAVRealTimeStateManageService();
	auto pViedioService = IVideoRealTimePreviewActivator::Get().getPISensorVedioService();
	if (nullptr == pService || nullptr == pViedioService)
		return;

	QList<long long> idList;
	pViedioService->GetActiveIdentitys(idList);
	QList<EntityInfo> entityList;
	for (auto& id : idList)
	{
		EntityInfo entity;
		entity.identity = id;
		qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t info;
		if (pService->GetUAVRealTimeState(QString::number(id).toStdString(), info))
		{

			entity.lon = info.telemetryInfo.position.lng;
			entity.lat = info.telemetryInfo.position.lat;
			entity.angleToBase = CalculateAngle(m_sortCenterLon, m_sortCenterLat, m_sortCenterYaw,
				entity.lon, entity.lat);
			entity.yaw = info.telemetryInfo.angleofRudder;
			entity.distanceToBase = CalculateDistance(m_sortCenterLon, m_sortCenterLat, entity.lon, entity.lat);
		}
		entityList.push_back(entity);
	}
	std::sort(entityList.begin(), entityList.end());

	idList.clear();
	ID2DISTMAP id2DisMap;
	for (auto& entity : entityList)
	{
		idList.push_back(entity.identity);
		QVariantMap tmpInfoMap;
		tmpInfoMap["Distance"] = entity.distanceToBase;
		tmpInfoMap["Lon"] = entity.lon;
		tmpInfoMap["Lat"] = entity.lat;
		tmpInfoMap["Yaw"] = entity.yaw;
		id2DisMap.insert(entity.identity, tmpInfoMap);
	}

	for (auto iter = id2DisMap.begin(); iter != id2DisMap.end(); iter++)
	{
		auto pWEntity = EntityDataManage::Get().FindEntity(iter.key());
		auto pEntity = pWEntity.lock();
		if (pEntity)
		{
			pEntity->setDataMap(iter.value());
		}
	}

	if (!m_isUseSort)
		idList.clear();
	emit onViewOrderChanged(idList);
}

void SortViewThread::UpdateEntityStatus()
{
	QList<long long> idList;
	auto pViedioService = IVideoRealTimePreviewActivator::Get().getPISensorVedioService();
	if (nullptr != pViedioService)
	{
		pViedioService->GetActiveIdentitys(idList);

		QMap<long long, bool> processMap;
		for (auto& id : idList)
			processMap[id] = true;

		QList<long long> addList;
		QList<long long> deleteList;

		// 更新现有的
		auto pEntityList = EntityDataManage::Get().GetEntityList();
		for (auto iter_wgt = pEntityList.begin(); iter_wgt != pEntityList.end(); iter_wgt++)
		{
			auto identity = (*iter_wgt)->GetIdentity();
			auto iter = processMap.find(identity);
			if (iter != processMap.end())
			{
				iter.value() = false;
				if ((*iter_wgt)->needShow())
				{
					// 目前只显示当前的，数量耗时不高于7个 总耗时80ms，所以处理的过来不需要占用线程吃
					//QThreadPool::globalInstance()->start(new ConvertTask([pData = (*iter_wgt)]() {
					//	(pData)->onUpdateData();
					//}));
					(*iter_wgt)->onUpdateData();
				}
			}
			else
			{
				deleteList.append(identity);
			}
		}

		// 更新新增的
		for (auto iter = processMap.begin(); iter != processMap.end(); iter++)
		{
			if (*iter)
			{
				addList.append(iter.key());
			}
		}

		if (!deleteList.isEmpty() || !addList.isEmpty())
		{
			emit entityStatusChanged(addList, deleteList);
		}
	}
}

void SortViewThread::onTimeOut()
{
	//static auto s_last = std::chrono::system_clock::now();
	//static auto s_now = std::chrono::system_clock::now();
	//static uint s_maxCustom = 0;
	//static uint s_maxIntervel = 0;

	if ((m_calcTargetDistanceCnt) % 2 == 0)
		UpdateFriendRect();

	if (m_calcTargetDistanceCnt % 5 == 0)
	{
		m_calcTargetDistanceCnt = 0;
		SortProcess();
	}

	//s_now = std::chrono::system_clock::now();
	//uint intervelTime = (std::chrono::duration_cast<std::chrono::milliseconds>(s_now - s_last).count());
	//s_maxIntervel = qMax(intervelTime, s_maxIntervel);
	// qDebug() << "Max Indervel:" << s_maxIntervel << "cur_SortViewThread::onTimeOut()_intervel" << intervelTime << "-----1";
	
	UpdateEntityStatus();

	//s_last = std::chrono::system_clock::now();
	//uint curstm = (std::chrono::duration_cast<std::chrono::milliseconds>(s_last - s_now).count());
	//s_maxCustom = qMax(curstm, s_maxCustom);
	// qDebug() << "Max _Custom:" << s_maxCustom << "SortViewThread::onTimeOut()_Custom" << curstm << "-----2";

	++m_calcTargetDistanceCnt;
}