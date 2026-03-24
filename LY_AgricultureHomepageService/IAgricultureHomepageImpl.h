#ifndef LY_AgricultureHomepageImpl_H
#define LY_AgricultureHomepageImpl_H

#include <QMutex>


#include "DataManage/AgriSevice/IAgricultureHomePageService.h"
#include "DataManage/AgriFarm/AgriHomePageDecl.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "vector3.h"
#include "map3d/IEarth.h"
#include "map3d\IGraphics.h"
#include <map3d/EarthEvent.h>
#include <QUuid>
#include "TinyXML\tinyxml.h"

namespace qnzkna
{
namespace IAgricultureHomePage
{

class IAgricultureHomepageImpl: public IAgricultureHomePageService
{

public:
	IAgricultureHomepageImpl();

	virtual ~IAgricultureHomepageImpl(void);

	void OnCreate();

	void InitDataBase();
	/* 盐田场列表 */
	virtual AgriFarmInfoVector GetFarmInfoList() const;
	/* 盐田列表 */
	virtual AgriFieldInfoModelVec GetFieldInfoList(QString &farm_id);
	/*传感器分组*/
	virtual AgriAgriSensorGroupModelVec GetSensorGroupList(QString &farm_id);
	/*传感器添加分组*/
	virtual bool  AddSensorGroup(AgriSensorGroupModel &fieldInfo);
	/*传感器分组删除*/
	virtual bool  DeleteSensorGroup();
	/*获取传感器类型*/
	virtual  SensorTypeModelVec GetSensorTypeList(QString &field_id) ;
	/**添加传感器类型*/
	virtual  bool AddSensorType(SensorTypeModel &model) ;
	/**删除传感器类型*/
	virtual  bool DeleteSensorType(SensorTypeModel &model);
	/*获取所有的传感器*/
	virtual  SensorListModelVec GetSensorList(QString &sensorTypeID);
	virtual  SensorListModelVec GetSensorListWithFieldID(QString &FieldID);
	/*添加传感器*/
	virtual  bool AddSensor(SensorListModel &model) ;
	/*删除传感器*/
	virtual  bool DeleteSensor(SensorListModel &model);
	/*修改传感器*/
	virtual  bool UpdateSensor(SensorListModel &model);
	/* 根据盐田获取传感器*/
	virtual  std::vector<QString> GetSensorWithField(QString &field_id);
	/*盐田直接添加传感器*/
	virtual  bool AddSensorWithField(QString &field_id, QString &mointID);
	/*盐田直接删除传感器*/
	virtual  bool DeleteSensorWithField( QString &mointID);
	//检查相同的传感器
	virtual  bool CheckSampleSensor(SensorListModel &model) ;
	/*获取识别图片*/
	virtual PestPhotoInficationVec GetPestHarmData(const QString &timeStr);
	/*添加识别图片*/
	virtual bool AddPestHarmSqlData(FarmPestPhotoInfication &listModel);
	/*获取采集图片时间*/
	virtual std::vector<QString> GetPestHarmTime();

	bool IAgricultureHomepageImpl::clearLocalFile();
	/*写入文件*/
	virtual void writeStructLocalFile(std::vector<SensorListModel> tempVec);
	/*读取文件*/
	virtual SensorListModel * readStructLocalFile(SensorListModel &model);
	/*修改文件*/
	virtual void editStructLocalFile(SensorListModel &m);
	/*删除文件*/
	virtual void deleteStructLocalFile(SensorListModel &m);
	/*添加文件数据*/
	virtual void addStructLocalFile(SensorListModel &m);

	virtual std::map<QString, SensorListModelVec> GetSensorListMap();
	virtual std::map<QString, SensorListModelVec> GetSensorMap();
	////====================绘制================
	//显示传感器
	virtual void showSensor(bool isShow);
	//显示标牌
	virtual void showBrand(bool isShow);
	//显示视场
	virtual void showView(bool isShow);
	//绘制传感器图片
	virtual void draw3DMointorImg(SensorListModel &model);
	//绘制传感器标牌
	virtual void drawMointer3dText(SensorListModel &model);
	//删除标牌
	void clearnMointer3dText();
	//删除单个传感器
	virtual void clearSingle3DMointerImg(SensorListModel &model);
	//删除传感器
	void clearAll3DMointerImg();
	//清除视场
	void clearn3DMap(SensorListModel &model);
	//绘制三菱锥
	void convert3DMap(casic_vector3d curPos,
		double heading, double fuyang,
		double pianzhuan, float angle_l, 
		float angle_w, float dis_m, QColor zoneColor, SensorListModel model);
	std::vector<IUserMesh2*> createDetectionZone(float angle_l, float angle_w, float dis_m);
	void convert3DCube(double lon, double lat, double  local_height);

	//显示传感器模型
	void showSensorModel(bool isShow);
	//绘制三维传感器模型
	void draw3DSensorModel(SensorListModel &sensorModel);
	//删除三维传感器模型
	void clearAll3DSensorModel();
private:

	bool			isOpen;
	QSqlDatabase				db;
    mutable QMutex      sMutex;

	QCString m_strDataDir;
	//std::vector<IText*>m_3DLocPositionMesh2Vec;
	//std::vector<ILabel*>m_3DTakeOffMeshVec;

	SensorListModel m_sensorListModel; //记录选中的传感器
	AgriAgriSensorGroupModelVec m_group_list;
	//增删查改的数据
	std::map<QString, SensorListModelVec> m_sensorListMap;
	//原始数据 增查改的数据(不带删除的数据)
	std::map<QString, SensorListModelVec> m_origin_sensor_list_map;

	std::vector<ILabel*>m_image3DVec;

	std::map<QUuid, std::string>				m_mapLabelPath;
	//std::map<QString, QUuid>				m_labelIDmap;

	std::map<QString, ILabel *>					m_map3DImage;
	std::map<QUuid, ILabel *>			m_wormLabelMap;

	std::vector<IText *>                     m_map3DTextVec;

	//三菱锥相关
	std::map<QString, Uav3D> coneUav3DpointMap;
	std::map<QString, std::vector<IUserMesh2*>> cone3DpointMap;

	TiXmlDocument* tinyXmlDoc = nullptr;
};

}
}

#endif
