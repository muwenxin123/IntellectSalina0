#ifndef  SORTVIEWTHREAD_H
#define SORTVIEWTHREAD_H

#include <QMap>
#include <QObject>
#include <QTimer>
#include <QCString.h>
#include <QVector3D>
#include <QVariantMap>
#include <QGeoCoordinate>
#include "IVideoRealTimePreviewActivator.h"

struct FriendRectScope;
typedef QMap<long long, QVariantMap>  ID2DISTMAP;
Q_DECLARE_METATYPE(QList<long long>)
Q_DECLARE_METATYPE(ID2DISTMAP)

class SortViewThread : public QObject
{
	Q_OBJECT
public:
	// 相机参数
	struct CameraInfo {
		double minHFov = 0;
		double minVFov = 0;
		double maxHFov = 0;
		double maxVFov = 0;
		int maxZoom = 0;
		double hFovSpace = 0;
		double vFovSpace = 0;
	};

	SortViewThread(QObject * object = nullptr);
	virtual ~SortViewThread();
	
signals:
	void onViewOrderChanged(QList<long long> idList);

	void entityStatusChanged(QList<long long> addIdList, QList<long long> deleteIdList);

public slots:
	void onConfigureChanged();

	void onFocalZoomChanged(int imageZoom);

	void onCameraAttributeChanged(long long identity, double yawValue, double pitchValue, double rollValue);;

private:
	void Init();

	// 经纬高→ECEF
	inline QVector3D llaToEcef(double lon, double lat, double alt);

	// ECEF→ENU
	inline QVector3D ecefToEnu(const QVector3D& ecef_delta, double lon_ref, double lat_ref);

	// 主计算函数（支持pitch角）
	bool computeTargetCornersWithPitch(
		// 输入参数
		double lon_observer, double lat_observer, double alt_observer,
		double yaw_observer, double pitch_observer, // 新增pitch角（度）
		double lon_target, double lat_target, double alt_target,
		double width_target, double height_target, double yaw_target,
		double hFov, double vFov,
		int image_width, int image_height,
		// 输出参数
		qnzkna::SensorVedio::Point* top_left,
		qnzkna::SensorVedio::Point* top_right,
		qnzkna::SensorVedio::Point* bottom_left,
		qnzkna::SensorVedio::Point* bottom_right,
		double& deepScan
	);

	double CalculateDistance(double lon1, double lat1, double lon2, double lat2);
	
	double CalculateAngle(const double& baseLon, const double& baseLat, double baseAngle,
		const double& targetLon, const double& targetLat);

	double LimitAngle(double angle);

	// 获取装备在俯仰角为180的视场上的位置框
	bool CalcEquipRectInMapVideo(QGeoCoordinate lookUav, double lookHeading, double hFov, double vFov, QGeoCoordinate showUav, qnzkna::SensorVedio::FriendRectScope& scope);

	// 获取在当前倍数的实际焦距
	void GetCurFov(const CameraInfo& info, int ratio, double& targetHFov, double& targetVFov);

	// 获取友方的区域范围
	void UpdateFriendRect();

	void SortProcess();

	void UpdateEntityStatus();

private slots:
	void onTimeOut();
	
private:
	QTimer m_timer;
	int m_calcTargetDistanceCnt = 0;

	double m_cameraPitchValue = 0.0;
	double m_cameraYawValue = 0.0;
	double m_cameraRollValue = 0.0;
	int m_focalZoom = 1;
	CameraInfo m_cameraInfo;

	bool m_isUseSort = false;
	double m_sortCenterLon = 0.0;
	double m_sortCenterLat = 0.0;
	double m_sortCenterYaw = 0.0;

	bool m_isUserFriendTrack = false;
	int m_lastFriendSize = 0;
	long long m_observerID = -1;
	QMap<long long, QString> m_id2NameMap;
};
#endif // LY_VIDEOREALTIMEPREVIEW_H
