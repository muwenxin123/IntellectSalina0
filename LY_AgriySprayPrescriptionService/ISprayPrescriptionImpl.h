#ifndef IPOSITIONSERVICEIMPL_H
#define IPOSITIONSERVICEIMPL_H

#include <QMutex>
#include <QObject>

#include "DataManage/AgriSevice/IAgriSprayPrescriptionService.h"
#include "DataManage/AgriFarm/AgriSprayPrescriptDecl.h"
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QPainter>
#include "IProj.h"
#include "QCString.h"
#include <osgEarth/ElevationQuery>
#include "vector3.h"
#include <map3d/EarthEvent.h>
#include <QUuid>
#include <map>
#include <deque>
#include "AgriDataAnalysisPredictAlgorithm.h"
#include "LY_AgriSPrayPrescriptionUntil.h"
#include "IWeaponManageService.h"
#include "IZoneManageService.h"
#include "map3d/IEarth.h"
#include "map3d\IGraphics.h"
#include "KrigingInterPolator.h"
#include "Zone\ZoneManagerInfo.h"
#include <QMutex>
#include "track.h"
#include <osg/BlendFunc>
#include <OpenThreads/ScopedLock>
#include "LY_AgriSpayPredictTool.h"

//#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
using namespace AgriSprayPrescriptionDecl;

namespace qnzkna
{
namespace AgriSprayPrescription
{
class ISprayPrescriptionImpl : public IAgriSprayPrescriptionService
{

public:
	ISprayPrescriptionImpl();

	virtual ~ISprayPrescriptionImpl(void) {};

	virtual void InitDataBase() const;
	//获取当前盐田name
	virtual QString GetFarmName(const QString& str) const;
	//获取虫害数据集
	virtual AgriPestDatasetVec GetPestList(const QStringList& fieldList) const;

	//获取生长阶段
	virtual AgriGrowthStageVec GetGrowthStageList() const;
	//获取病虫害阶段
	virtual AgriPestCollectionVec GetPestCollectionList(const QString& sDatasetId) const;
	//获取虫害图信息
	virtual AgriPestMapInfoModel GetPestInfoMap(const QString& sDatasetId) const;
	//添加处方图
	virtual bool AddSprayPrescription(AgriSprayPrescriptionModel &model) const;

	//获取处方图
	virtual AgriSprayPrescriptionVec GetSprayPrescription(const QString field_id) const;
	//修改处方图
	virtual bool UpdateSprayPrescription(AgriSprayPrescriptionModel &model) const;
	//删除处方图
	virtual bool DeleteSprayPrescription(QString &guid) const;


	//获取任务指令
	virtual bool  GetTaskInstruction() const;
	//添加任务指令
	virtual bool  AddTaskInstruction(AgriTaskInstruction &Instruction) const;
	//删除任务指令
	virtual bool  DeleteTaskInstruction() const;


	//生成喷药处方图
	virtual void generateSprayPrescriptionDiagram(std::vector<QPointF> m_boundPoings,
		                                                                         QString pest_id,
		                                                                         double pyfk = 4.0,
		                                                                         int map_leve = 5, bool isLoitering = false);
	//清理处方图
	virtual void clearSprayDiagramData();
	//显示隐藏处方图
	virtual void showSprayDiagramData(bool isShow);
	//生成处方图航线
	virtual void generateSprayDiagramFlyLine(std::vector<QPointF> m_boundPoings,
		                                                                                                        QString spray_id,
		                                                                                                        double pyfk,
		                                                                                                        double flyHeight,
		                                                                                                        bool isLoitering,
		                                                                                                        std::vector<Poi> &flyPoints,
		                                                                                                        double &flyDisance);
	//隐藏/显示 飞机航线
	virtual void setFlylineStatus(bool isShow);
	//生成二维处方图
	void create2DSprayDiagram(QPainter& painter, IProj *pProj);
	//生成肥力图航线
	virtual void generateFertilizerDiagramFlyLine(std::vector<QPointF> m_boundPoings,
		std::vector<AgriSoilTestingFertilityPoint> gridVec,
		double pyfk,
		double flyHeight,
		std::vector<AgriSoilTestingFertilityPoint> &flyPoints,
		double &flyDisancee);


	//飞机模拟播放  isLoitering ture 巡飞
	virtual void  flyRouteSimulPlayback(double flyHeight,bool isLoitering = false);
	//清除飞机3d数据
	virtual void clearn3DflyData();
	// 获取无人机状态
	virtual UavStatus getUavStatus();
	//指令播放
	virtual  void playInstructionFlying(bool isPlayOrderfilght = true);
	//获取指令集
	virtual std::vector< AirInstructionPoi>  generateInstructionPoint(double pyfk);

private:

	
	
private:
	QSqlDatabase				db;
	bool			isOpen;
    mutable QMutex      sMutex;

	LY_AgriSpayPredictTool *m_flyTool = nullptr;

	HeatMap3D  m_map3D;

	//航线生成的path
	std::vector<QPoint> m_paths;
	//处方图分布图的点
	std::vector<Poi> m_origin_poiPaths;
	
	//处方图网格生成的map
	std::map<QPoint, Poi, QPointFCompare> m_pointMap;
	//二维网格点
	//QVector<QPoint> m_gridPoints;
	//存储处方图飞机飞行带颜色方格的经纬度
	std::map<QPoint, Poi, QPointFCompare> m_flyPointMap;
	//带颜色等级方格的索引
	std::vector<QPoint> tempPoints;
	//飞机飞行点的信息
	std::vector< AirInstructionPoi>m_airInstructionPoints;


	//肥力图网格生成的map
	std::map<QPoint, AgriSoilTestingFertilityPoint, QPointFCompare> feripointMap;
	//存储肥力飞机飞行带颜色方格的经纬度
	std::map<QPoint, AgriSoilTestingFertilityPoint, QPointFCompare> m_flyFeriliPointMap;

};

}
}

#endif
