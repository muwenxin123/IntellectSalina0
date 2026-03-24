#ifndef CDLGMARKLIST_H
#define CDLGMARKLIST_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QMutex>
#include <QGeoCoordinate>
#include "DataManage/Common/TypeDefine_Node.h"
#include "DataManage/Mark/Mark.h"
#include "Protocol/XygsZC/Bag_XygsZCDefines.h"
#include "QTableWidget"
#include "DataManage/Mark/ZoneInfo.h"
#include "DataManage/INebulaTaskManageService.h"
#include "Protocol/XygsTelegraph/TelegrpahHead.h"
#include "DataManage/IWeaponManageService.h"

#define ThreatenByNone		            1
#define ThreatenByAreaCenterPoint		2
#define ThreatenByTakeOffPoint			3

typedef std::map<double, SystemMarkInfo> MarkInfoMapByThreaten;
namespace Ui
{
class CDlgMarkList;
}

class LyWidget;
class CombatZoneEditDialog;

class CDlgMarkList : public QWidget, public qnzkna::TaskManage::INebulaTaskManageObserver
{
    Q_OBJECT

public:
    explicit CDlgMarkList(QWidget *parent = nullptr);
    ~CDlgMarkList();

private:
    Ui::CDlgMarkList *ui;

    QMenu													*m_TableMenu;
    QAction													*m_MenuAdd;
    QAction													*m_MenuEdit;
    QAction													*m_MenuRemove;
    QAction													*m_MenuUpdate;
    QAction													*m_MenuImg;
    QAction													*m_MenuVideo;
    QAction													*m_MenuMerge;
    QAction                                                 *m_MenuMarkState;

    QMenu                                                   *m_MarkState;
    QAction                                                 *m_MenuDestroy;
    QAction                                                 *m_MenuPressing;
    QAction                                                 *m_MenuAttack;
    QAction                                                 *m_MenuUndestructed;
    QAction                                                 *m_MenuUnsuppressed;
    QAction                                                 *m_MenuDamage;
    QAction                                                 *m_MenuDisapper;

    QStringList												m_headerlabels;

    CSystemMarkInfoVec									m_MarkInfoVec;

    MarkInfoMapByThreaten									m_ThreatenMarkMap;

    bool													m_bUpdateFlg;

    QStringList												m_DBMarkType;

    int														m_nActionID;
    int														currActionID;
    bool													m_bPressEvect = true;

    std::map<std::string, std::string>						m_SystemSysNameAndID;

    qnzkna::WeaponManage::IWeaponManageService				*m_pIWeaponManageService = nullptr;

	QGeoCoordinate											m_SearchCenterPoint;

	QGeoCoordinate											m_TakeOffPoint;

public:

    void TabWidgetConnet();
    void CreateTableMenu();

    void UpdateTrackInfoToListHeading();

    void UpdataWindowData();
    void UpdataWindowDataByAction();
    void UpdateListData();
    void UpdataListData(QTableWidget *pQTable, SystemMarkInfo& stMarkInfo, int nNo,double dThreaten = 0);

    QString GetTypeString(ESystemMarkType eType);
    QString GetAttributeString(ESystemMarkAttribute eType);
    QString GetMaterialString(ESystemMarkMaterial eType);
    QString GetMissionTypeString(EMissionType eType);
    QString GetThreatenDegreeString(EThreatenDegree eType);
    QString GetZoneAttributeString(EZoneAttribute eType);
    QString GetZoneShapeString(EZoneShape eType);
    QString GetMarkState(EMarkState eState);

    void updateTableNo(QTableWidget *pTable);

    virtual void Update(qnzkna::TaskManage::INebulaTaskManageService *sub);

    void AddDBMark(const TeleMsg_FB_04 &dbData);

    void localWeaponNameBindToId();

    void TableWidgetAddCheckBox(QTableWidget *tableWidget, int x, int y, QString text, Qt::CheckState CheckState);

    void UpdateDataByDB();

    void updateMenu();

    void UpdateMarkState(int Num);

    void SetSearchZoneCenterPoint(const double& dLon, const double& dLat, const double& dAlt);
	void SetTakeOffPoint(const double& dLon, const double& dLat, const double& dAlt);
	void UpdateListDataByThreaten();
private:

    void showEvent(QShowEvent *event);

public slots:

    void on_Menu_Update();

private slots:

    void on_Menu_Add();

    void on_Menu_Edit();

    void on_Menu_Remove();

    void on_Menu_Image();

    void on_Menu_Video();

    void on_Menu_Merge();

    void on_Menu_Destroy();

    void on_Menu_Pressing();

    void on_Menu_Attack();

    void on_Menu_Undestructed();

    void on_Menu_Unsuppressed();

    void on_Menu_Damage();

    void on_Menu_Disapper();

    void customContextMenuRequested(const QPoint &pos);

    void tableWidget_cellDoubleClicked(int row, int column);

    void tableWidget_cellClicked();

    void execMergeAction(int mergeIndex, QString mergeName, int mergedIndex, QString mergedName);

    void on_comboBox_currentIndexChanged(int index);
};

#endif
