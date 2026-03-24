#ifndef ITASKMANAGEIMPL_H
#define ITASKMANAGEIMPL_H

#include <time.h>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "DataManage/IZoneManageService.h"
#include "QPainter"
#include "map3d/IGraphics.h"
#include "aStar.h"
#include <vector>
#include "vector3.h"
#include "QGeoCoordinate"
#include "GetHeight.h"

#include "drawAble_Base.h"
#define SAFE_DISTANCE 100

namespace osgEarth
{
	namespace Annotation
	{
		class FeatureNode;
		class AnnotationEditor;
	}
}

namespace osg
{
	class Group;
}

typedef std::map<std::string, osgEarth::Annotation::FeatureNode*> FeatureNodeMap;

struct Zone_point_s
{
	double	x;
	double	y;
	bool	bDoubleFlag;
	Zone_point_s(double d1,double d2, bool bFlag = false)
	{
		x = d1;
		y = d2;
		bDoubleFlag = bFlag;
	}
	Zone_point_s()
	{
		x = 0;
		y = 0;
		bDoubleFlag = false;
	}
};

struct CircumCircle {
	double centerLon;
	double centerLat;
	double radius;
	CircumCircle(double lon, double lat, double ra) : centerLon(lon), centerLat(lat), radius(ra){}
	CircumCircle() {
		centerLon = 0;
		centerLat = 0;
		radius = 0;
	}
};

typedef std::vector<Zone_point_s> Zone_point_s_Vec;

typedef std::map<QString, IUserMesh1*>	UserMesh1Map;
typedef std::map<QString, IUserMesh2*>	UserMesh2Map;
typedef std::map<QString, IAirLine*>	UserLineMap;
typedef std::map<QString, SHPObject*>	ZoneSHPObjectMap;

namespace qnzkna
{
	namespace ZoneManage
	{
		class IZoneManagerImpl:
			public IZoneManageService
		{
		public:

			IZoneManagerImpl(void);

			~IZoneManagerImpl(void);

		private:

			QSqlDatabase							m_db;
			CZoneBaseVec							m_ZoneBaseVec_All;
			CZoneBaseVec							m_ZoneBaseVec_Activated;
			CZoneBaseVec							m_ZoneBaseVec_Cover;
			osg::Group*								m_FeatureNode_Parent = nullptr;
			FeatureNodeMap							m_FeatureNode_Cover;
			UserMesh1Map							m_UserMesh1Map;
			UserMesh2Map							m_UserMesh2Map;
			UserLineMap								m_UserLineMap;
			ZoneSHPObjectMap						m_ZoneSHPObjectMap;
			UserMesh1Map							m_UserMeshIntersectMap;
			int										m_nActivatedAcitonID = 0;
			CZoneBaseAdditionMap					m_CZoneBaseAdditionMap;
			QMutex									sMutex;

			std::vector<casic_vector3d>				m_pSearchPointVec;
			std::vector<std::vector<std::shared_ptr<AStar::PlanNode>>> m_pGridNode;
			std::vector<casic_vector3d>				m_pObstacleArea;

			static CGetHeight						s_CGetHeight;
			bool									m_replanAirwayDraw = false;

			CZoneBase								m_replanAirwayZone;
			CZoneBaseRule							m_ZoneBaseRule;

			QString									m_strZoneFilePath;

			CRelationAirlineAndZoneMap				m_RelationAirlineAndZoneMap;
			drawAble_Base*													m_drawAble_3D = nullptr;
			std::map<std::string, osgEarth::Annotation::AnnotationNode*>	m_FeatureNodeMap_draw_3d;
			std::map<std::string, vector<osg::Vec3d>>						m_FeatrueNodePosMap_draw_3d;
			osg::Group*														m_FeatureNodeParent_draw_3d = nullptr;

			osgEarth::Annotation::AnnotationEditor*							m_Editor_drawAble_3d = nullptr;
			std::string														m_edited_drawAble_3d_name = "";
			osgEarth::Annotation::AnnotationNode*							m_edited_featureNode = nullptr;
			osg::Group*														m_edited_FeatureNodeParent = nullptr;

			double															m_dHeightScale = 1;
			double															m_dWidthScale = 1;
			int																m_nDiaphaneity = -1;

			std::vector<int>												m_vLableColorVec;

			CAirTrafficLineMap												m_AirTrafficLineMap;

		private:
			void InitDataBase();
			void LoadZoneBaseFormDB();
			void LoadZoneBasePointFormDB(CZoneBase& stData);
			bool AddZoneBaseToDB(const CZoneBase& stData);
			bool EditZoneBaseToDB(const CZoneBase& stData);
			bool RemoveZoneBaseToDB(const QString& strID);
			bool RemoveZoneBasePointFormDB(const QString& strID);
			bool JudegeZoneIDIsExist(const QString& strID);
			bool TransformToDBData(const CZoneBase& stData, Zone_point_s_Vec& TmpVec);
			int GetZonePointCount(const CZoneBase& stData);

			void DrawOneQuYu(QPainter *pDC, CZoneBase &zone, bool bModify = false);
			void DrawKongYuCircle(QPainter *pDC, double x, double y, float radius, float startangle, float endangle, int type, int filltype);
			void DrawKongYuPie(QPainter *pDC, double x, double y, float radius, float startangle, float endangle, int type, int filltype, QPainterPath &path);
			void DrawQYEllipse(QPainter *pDC, double x, double y, float fLRadius, float fSRadius, float fRotateAngle, int filltype);
			void DrawCircleRing(QPainter *pDC, double x, double y, CZoneBase &zone);
			void DrawArcRing(QPainter *pDC, float x, float y, CZoneBase &zone);
			void DrawKongYuRect(QPainter *pDC, CZoneBase *pZone, double x1, double y1, bool flag = 0);
			void calcPoint(double x, double y, double l, double h, double angle, double type, double &centerx, double &centery, QVector<QPointF> &pts);
			void DrawFG(QPainter *pDC, int x1, int y1, int x2, int y2);

			void Init3DZones();
			void Add3DZone(const CZoneBase &zone);
			void Add3DZoneSHPObject(const CZoneBase &zone);
			void Remove3DZone(const QString &strID);
			void Remove3DZoneUserMesh(const QString &strID);
			void Draw3DGlobe(IUserMesh2 *pUM,double r, int precision = 10);

			void LoadZoneBaseAdditionFormDB();
			bool AddZoneBaseAdditionToDB(const CZoneBaseAddition& stData);
			bool RemoveZoneBaseAdditionToDB(const QString& strID,const int& nActionID);

			void UpdateLocalSHPObject(const QString &strID, SHPObject* pSHPObject);
			void RemoveLocalSHPObject(const QString &strID);

			bool GetLocalSHPObject(const QString &strID, SHPObject& stSHPObject);

			IUserMesh2 *CreateRectMesh(IGraphics *pGrp, int x, int y, int z);

			IUserMesh2 *SetCubeColor(IGraphics *pGrp, int x, int y, int z, unsigned int color);

			bool JudgeAltitudeintercept(const QString &strZoneID1, const QString &strZoneID2,double& dZ,double& dM);

			CircumCircle GetZoneCircumCircle(CZoneBase stZoneBase);

			ZonePointVec GetNewAirwayBetween2Point(ZonePoint p1, ZonePoint p2, CircumCircle ccircle);

			std::vector<std::vector<shared_ptr<AStar::PlanNode>>> createGridsNode(const std::vector<double>& gridMapParams, const QGeoCoordinate& gridMapLeftup, const CZoneBase& obstacleZone);

			void pathPlanning(casic_vector3d start, casic_vector3d goal, std::vector<double>& fly_param, std::vector<casic_vector3d>& searchPointList);

			void sparsePath(std::vector<casic_vector3d> &SearchPointVec);

			bool isPointInZone(const QGeoCoordinate& point, const CZoneBase& obstacleZone);

			void LoadZoneRuleFromDB();
			bool EditZoneRuleToDB(const CZoneBaseRule& stData);

			void LoadRelationFromDB();
			bool AddRelationFromDB(const int& nAirLineID, const QStringList& ZoneIDList);
			bool RemoveRelationFromDB(const int& nAirLineID);

			void LoadLableColorDB();
			bool AddLableColorDB(std::vector<int> Color);
			bool ClearLableColorDB();

			void LoadZoneScaleIni();

			void LoadLineDB();
			bool AddlineDB(const CAirTrafficLine data);
			bool EditLineDB(const CAirTrafficLine data);
			bool RemovelineDB(const int nID);
			bool IsLineExistedDB(const int nID);
			bool SetLineShowStatesDB(const int nLineID, const bool bShow);

			bool AddLinePointDB(const CAirTrafficLinePoint data);
			bool AddLinePointsDB(const CAirTrafficLinePointVec data);
			bool EditLinePointDB(const CAirTrafficLinePoint data);
			bool InsertLinePointDB(const CAirTrafficLinePoint data);
			bool RemoveLinePointDB(const int nLineID, const int nPointID);
			bool RemoveLinePointDB(const int nLineID);
			bool IsLinePointExistedDB(const int nLineID, const int nPointID);
			int GetLinePointMaxID();
			void updatePointsIndexFormNow(const int nowIndex, const int nLineID, const int disNum);

		public:

			void OnEarthMessage_Pick(int wParam, void * lParam);

		public:

			void DrawQYZoneData(QPainter *painter);

		public:

			void SetActivatedAciton(const int& nActionID);

			CZoneBaseVec GetAllZoneBase();

			CZoneBaseVec GetAllZoneBase(const int& nType);

			bool GetZoneBase(const QString& strID, CZoneBase& stData);

			bool AddZoneBase(const CZoneBase& stData, const long long& llStartTime = 0, const long long& llEndTime = 0);

			bool EditZoneBase(const CZoneBase& stData, const long long& llStartTime = 0, const long long& llEndTime = 0);

			bool EditInTimeZoneBase(const CZoneBase& stData, const long long& llStartTime = 0, const long long& llEndTime = 0);

			bool RemoveZoneBase(const QString &strID);

			bool RemoveZoneBase();

			bool IsZoneBaseIDExisted(const QString &strID);

			bool SetZoneBaseShowStates(const QString &strID, const bool bShowFlag);

			bool SetZoneBaseColor(const QString &strID, const unsigned int nColor);

			bool GetZoneBaseValidTime(const QString &strID, TimeVec&  StartTimeVec, TimeVec&  EndTimeVec);

			bool SetZoneBaseValidTime(const QString &strID, const TimeVec  StartTimeVec, const TimeVec  EndTimeVec);

			bool JudgeZoneIntersect(const QString &strZoneID1, const QString &strZoneID2, SHPObject * pIntersectZone = nullptr);

			bool JudgeZoneIntersect(const QString &strZoneID1, QStringList& strIDList);

			void ClearZoneIntersect();

			bool FindLineByZones(const QString &strZoneID1, const QString &strZoneID2, QStringList& strLineIDList);

			bool TransformRectangleToPoints(const QString &strZoneID, ZonePointVec& PointVec);

			bool GriddingZone(const QString &strZoneID);

			bool ReplanAirway_1(const QString& strAirwayZoneID, const QString& strZoneID2);
			bool ReplanAirway_2(const QString& strAirwayZoneID, const QString& strZoneID2);

			bool AddCoverZoneBase(const CZoneBase& stData);

			bool RemoveCoverZoneBase(const QString& strZoneID);

			bool setCoverisShow(const QString& strZoneID, bool isShow);

			bool RemoveCoverZoneBase();

			bool setCoverisShow(bool isShow);

			void GetZoneRule(CZoneBaseRule& stData);

			bool SetZoneRule(const CZoneBaseRule& stData);

			QStringList GetZoneFileNames();

			bool OpenZoneFile(const QString& strName,CZoneBaseVec& ZoneFileVec);

			bool SaveZoneFile(const QString& strName);

			bool RemoveZoneFile(const QString& strName);

			bool IsExistZoneFile(const QString& strName);

			bool GetZoneListByAirLineID(const int& nAirLineID, QStringList& ZoneIDList);

			bool RemoveRelationByLineID(const int& nAirLineID);

			bool SetRelationByLineID(const int& nAirLineID, const QStringList& ZoneIDList);
			void createADrawAble_3d(std::string polyID, int polyType, unsigned int _color = 255, float _color_A = 0.5);
			void createADrawAble_3d(std::string polyID, int polyType, unsigned int _color, float _color_A, double _width, double _height, float _angle);
			void endAndSaveCreateADrawAble_3d(std::string polyID);
			void cancleADrawAble(std::string polyID);

			void delADrawAble_3d(std::string polyID);

			void addEditor_forDrawAble_3d(std::string polyID);
			void addEditor_forDrawAble_3d(const CZoneBase& stData);
			void endEditing_forDrawAble_3d();
			void delEditor_forDrawAble_3d(std::string polyID);

			bool SetZoneShowScale(const int& nZoneType,const double& dHeightScale,const double& dWidthScale,const int&nDiaphaneity);

			bool SetZoneShowScaleDefault(const int& nZoneType);

			std::vector<int> GetLableColor();

			bool addLableColor(std::vector<int> Color);

			//º½ÏßÊý¾Ý
			//---------------------------------------------------------------------------------
			void LoadAirTrafficLine();

			CAirTrafficLineMap GetAllAirTrafficLine();

			CAirTrafficLine GetAllAirTrafficLine(const int &nLineID);

			int GetAirTrafficLineMaxID();

			int GetAirTrafficLinePointMaxID();

			int GetAirTrafficLinePointMaxID(int nLineID);

			CAirTrafficLinePointVec getAirTrafficLinePointsFornLineID(const int nLineID);

			bool AddAirTrafficLine(const CAirTrafficLine stLine);

			bool EditAirTrafficLine(const CAirTrafficLine stLine);

			bool RemoveAirTrafficLine(const int nID);

			bool IsAirTrafficLineExited(const int nID);

			CAirTrafficLinePointVec GetAirTrafficLinePoints(const int nID);
			CAirTrafficLinePoint GetAirTrafficLinePoint(const int nID, const int nPoint);

			bool AddAirTrafficLinePoint(const CAirTrafficLinePoint stPoint);

			bool RemoveAirTrafficLinePoints(const int nLineID);
			bool RemoveAirTrafficLinePoint(const int nLineID, const int nPoint);

			bool IsAirTrafficLinePointExited(const int nLineID, const int nPoint);

			bool SetAirTrafficLiseShowStates(const int nLineID, const bool bShow);
			//---------------------------------------------------------------------------------
		};
	}
}

#endif
