#ifndef CDLGMISSIONLINELIST_H
#define CDLGMISSIONLINELIST_H

#include <QWidget>
#include "DataManage/NebulaTask/NebulaMissionLine.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IWeaponManageService.h"
#include "QMenu"
#include "QAction"
#include "QMutex"
#include "map3d/IGraphics.h"

namespace Ui
{
class CDlgMissionLineList;
}

class CDlgMissionLineList : public QWidget, public qnzkna::TaskManage::INebulaTaskManageObserver
{
    Q_OBJECT

public:
    explicit CDlgMissionLineList(QWidget *parent = nullptr);
    ~CDlgMissionLineList();

public:

    void TabWidgetConnet();
    void CreateTableMenu();

    void UpdateTrackInfoToListHeading();

    void UpdataWindowData();
    void UpdateShowData();
    void UpdateListData();

    bool GetShowInfoByLineID(const int nLineID, QString &strTaskID, QString &strPlatformID);

    QString GetLineTypeStr(int nLineType);
    QString GetLineProtocolStr(int nLinePro);
    CNeubulaMissionLineVec GetShowData();

    void AddLinePoint(const int nLineID, const QString strName, const CNeubulaMissionLinePoint stPoint, const QString MissionID);
    void insertLinePointN(const int nLineID, const QString strName, const CNeubulaMissionLinePoint stPoint, const int insertId, const QString MissionID);
    void RemoveLinePoint(const int nLineID, const int nPoint);

    void updateLineName(const int nLineID, const QString strName);

    void ClearLinePoints(const int nLineID);

    bool UpdateLinePointMouseMove(int nLineID, int nPointID, double dLon, double dLat);

    virtual void Update(qnzkna::TaskManage::INebulaTaskManageService *sub);

    CNeubulaMissionLineVec getMissionShowLines()
    {
        return m_MissionLineShowVec;
    }

    std::string LineMatchPlatform(const std::string &strMissionID, const int &nLineID, const int &nNo);

    void localWeaponNameBindToId();

    QString	GetPlatNameByID(const std::string strPlatID);

    void updateMenu();

	void Draw3DLinesWeaponID();
	void Clear3DlinesWeaponID();
private:

    void showEvent(QShowEvent *event);

public slots:

    void selectBox(int r, int c);

private slots:

    void on_Menu_Add();

    void on_Menu_Edit();

    void on_Menu_Remove();

    void on_Menu_Update();

    void on_Menu_HideSelectMissionAll();

    void on_Menu_ShowSelectMissionAll();

    void on_Menu_HideAll();

    void on_Menu_ShowAll();

    void customContextMenuRequested(const QPoint &pos);

    void doubleClicked_line();

    void tablewidget_cellClicked();

private:
    Ui::CDlgMissionLineList *ui;

    QMenu						*m_TableMenu;
    QAction						*m_MenuAdd;
    QAction						*m_MenuEdit;
    QAction						*m_MenuRemove;
    QAction						*m_MenuUpdate;
    QAction						*m_MenuHideSelectMissionAll;
    QAction						*m_MenuShowSelectMissionAll;
    QAction						*m_MenuHideAll;
    QAction						*m_MenuShowAll;

    CNeubulaMissionLineVec		m_MissionLineVec;
    CNeubulaMissionLineVec		m_MissionLineShowVec;

	std::vector<IUserMesh1*> Path3DLines;
	std::vector<IRenderableObject*> Path3DLines_plat;
    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::WeaponManage::IWeaponManageService				*m_pIWeaponManageService = nullptr;

    QMutex						m_Mutex;

    bool                                                     m_bPressEvect = true;
    int                                                      currActionID;
    int                                                      m_nActionID;

    std::map<std::string, std::string>						 m_SystemSysNameAndID;
};

#endif
