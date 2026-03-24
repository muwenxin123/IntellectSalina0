#ifndef CDLGMISSIONPLANNINGOVERVIEW_H
#define CDLGMISSIONPLANNINGOVERVIEW_H

#include <QWidget>
#include"LyDialog.h"
#include"QCString.h"
#include <QTimer>
#include "QMenu"
#include "IProj.h"
#include "DataManage/NebulaTask/NebulaMissionPlatform.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/INebulaTaskManageService.h"
#include "ITaskManagerActivator.h"
#include <QGeoCoordinate>
#include "QtSvg/QSvgRenderer"

struct MissionPlanInfo
{
    std::string missionID;
    std::string platID;
    std::string AmmoID;
    int			nlineID;
    std::string targetID;

    MissionPlanInfo()
    {
        missionID = "";
        platID = "";
        AmmoID = "";
        nlineID = -1;
        targetID = "";
    }
};

struct VitalPostion
{
    QPointF targetPoint;
    double vitalDir;
    double vitalSize;
    double vitalDisScreen;
};

namespace Ui
{
class CDlgMissionPlanningOverview;
}

class CDlgMissionPlanningOverview : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMissionPlanningOverview(QWidget *parent = nullptr);
    ~CDlgMissionPlanningOverview();

    void Init();

    void CreateTreeWidget();

    void CreateTreeeMenu();

    void localWeaponNameBindToId();

    std::string WeaponIDToName(const std::string nID);

    void InitMarkTypeSvgNames();

    std::vector<QPolygonF> unionPolygon(std::vector<QPolygonF> &tmpQPolygonVec);
    void defUnionPolygon(std::vector<QPolygonF> &beforeVec, std::vector<QPolygonF> &unitedPolys);

    void drawMissionArea(QPainter &painter, IProj *pProj);

    void drawMarkInfoRange(QPainter &painter, IProj *pProj);

    void drawPlanLine(QPainter &painter, IProj *pProj);

    void draw3DMissionArea();

    void showEvent(QShowEvent *event);

    void hideEvent(QHideEvent *event);

public slots:

    void on_timeOut();

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

public:
    bool						                            b_UpdateAmmo = false;
    bool													b_isDraw = false;

private:
    qnzkna::WeaponManage::WeaponSystemMap					 weaponSystemMap;
    std::map<short int, std::vector<std::string>> WeaponTypeNames;
    std::map<std::string, std::string> SystemSysNameAndID;
    std::map<std::string, std::string> SystemSysIDAndName;

    std::vector<MissionPlanInfo> MissionPlanInfoVec;

    QString  m_strIconPatch = "Temp_Line/marker48X48_3D.png";
    unsigned int				m_nColor[25];
    QPixmap						m_IconImage;
    QPixmap						m_IconImageSelected;

    std::vector<QString> targetTypeSvgNames;

    std::vector<QVector<QPointF>> targetSenPolyLLs;
    std::vector<QVector<QPointF>> targetFirePolyLLs;
    std::map<std::string, QVector<QPointF>> targetKeyPolyLLs;
    std::map<std::string, QPolygonF> targetVitalDirPolys;

    std::vector<QPolygonF>tmpQPolygonSenserVec;
    std::vector<QPolygonF>tmpQPolygonFireVec;

    CNeubulaAction currAction;
    int currActionID;

    CZoneInfo Areas;

    std::vector<vector<QPointF>>						m_tmpPolygonCircle;
    std::vector<VitalPostion>							m_tmpPolyonPie;
    std::vector<vector<QPointF>>						m_tmpPolyonFireCir;

private:
    Ui::CDlgMissionPlanningOverview *ui;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::AmmunitionManage::IAmmunitionManageService     *m_pAmmunitionManageService = nullptr;
    CNeubulaMissionVec                                      m_NeubulaActionMissionVec;
    QTimer                                                  *m_timer = nullptr;
    QMenu													*m_TreeMenuRight;
};

#endif
