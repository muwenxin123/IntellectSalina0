#ifndef ENTITY_VEDIO_DATA_H
#define ENTITY_VEDIO_DATA_H

#include <vector>
#include <memory>
#include <QMap>
#include <QPixmap>
#include <QReadWriteLock>
#include  "IVideoRealTimePreviewActivator.h"

class EntityVedioData 
{
public:
    explicit EntityVedioData(long long identity);
    ~EntityVedioData();

	// 增加显示标识
	void addShowFlag();

	// 减少显示标识
	void reduceShowFlag();

	// 是否需要显示
	bool needShow();

	// 是否改变
	bool isChanged();

	// 重置改变
	void resetChangedFlag();

	// 获取实体ID
	long long GetIdentity();

	// 获取AreaInfo
	QString GetAreaInfo();
	
	// 更新容器
	void onUpdateData();

	// 设置数据
	void setDataMap(QVariantMap map);

	// 获取图片信息
	QPixmap& GetPixMap(QPixmap& map);

	// 获取sdl数据 pAvframe需要自行析构
	void GetSDLData(AVFrame** pAvframe, QByteArray& array);

	// 获取目标信息
	std::vector<XygsZC_Mark_01>& GetTargetInfo(std::vector<XygsZC_Mark_01>& targetInfoVec);

	// 获取目标名称
	void GetEntityName(QString& weaponName);

	// 获取目标距离
	void GetEntityDistance(double& distance);

	// 获取目标位置
	void GetEntityPos(double& lon, double& lat);

	// 获取友方信息
	std::vector<qnzkna::SensorVedio::FriendRectScope>& GetFriendInfo(std::vector<qnzkna::SensorVedio::FriendRectScope>& friendInfoVec);

	// 获取偏航角
	double GetEntityYaw();

	// 设置当前相机参数
	void SetCameraInfo(double yaw, double pitch, double roll);

	// 设置当前相机参数
	void GetCameraInfo(double& yaw, double& pitch, double& roll);

private:
	QReadWriteLock m_lock;
	int m_showCount = 0;
	bool m_isChanged = false;
	long long m_identity = 0;
	QPixmap m_map;
	AVFrame* m_pAvframe = nullptr;
	QByteArray m_picData;
	std::vector<XygsZC_Mark_01> m_targets;
	std::vector<qnzkna::SensorVedio::FriendRectScope> m_friendScopeVec;
	QString m_weaponName;
	QString m_areaID;
	double m_distance = -1;
	double m_lon = -10000;
	double m_lat = -10000;
	double m_yaw = 0;
	double m_cameraPitchValue = 0.0;
	double m_cameraYawValue = 0.0;
	double m_cameraRollValue = 0.0;
	size_t m_targetStructSize = 0;
};

typedef std::shared_ptr<EntityVedioData>       EntityVedioDataSPtr;
typedef std::weak_ptr<EntityVedioData>       EntityVedioDataWPtr;

class EntityDataManage
{
public:
	// 获取实体ID
	static EntityDataManage& Get();

	// 增加实体
	void AddEntity(EntityVedioDataSPtr ptr);

	// 删除实体
	void RemoveEntity(long long identity);
	
	// 查找实体
	EntityVedioDataWPtr FindEntity(long long identity);

	// 获取实体列表
	QList<EntityVedioDataSPtr> GetEntityList();

	// 是否包含
	bool IsContains(long long identity);

	// 清空所有
	void ClearAll();

private:
	EntityDataManage();
	~EntityDataManage();

private:
	QReadWriteLock m_lock;
	QMap<long long, EntityVedioDataSPtr> m_id2EntityDataMap;
};

#endif // ENTITY_VEDIO_DATA_H
