#ifndef CDLGEDITGROUPP_H
#define CDLGEDITGROUPP_H

#include <QWidget>
#include "LyDialog.h"
#include "CDlgEditLine.h"
#include "DataManage/Common/TypeDefine_Node.h"
#include "DataManage/Formation/Formation.h"
#include "DataManage/ICommandUnitManageService.h"
#include "DataManage/IFormationManageService.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IWeaponManageService.h"
#include "SystemConfig/IConfigProjectControlService.h"

const float PIXELS_PER_UNIT = 40.0f;

#define EDGE_SPACE_WIDTH	8
#define UAV_NUM		15

enum E_VIEW_MODE
{
    VIEW_TOP,
    VIEW_BACK,
    VIEW_TOP_SELECT,
    VIEW_NUM,
};

enum E_BTN_STATES
{
    BTN_NORMAL,
    BTN_ADD,
    BTN_EDIT,
    BTN_DELETE,
    BTN_OK,
    BTN_CANCEL,
};

namespace Ui
{
class CDlgEditGroup;
}
class QTreeWidgetItem;

typedef struct  _VehicleShowInfo
{
    QRect rect;
    bool Flag;
    bool Flag_Selected;
    bool Flag_In_Group;
    int eOperType;
    int	SelectVehicleID;
    int CountNo;
    QString name;
    QString VehicleID;
    _VehicleShowInfo()
    {
        name = "";
        VehicleID = "";
        Flag = false;
        Flag_Selected = false;
        Flag_In_Group = false;
        eOperType = 0;
        CountNo = 0;
    }
} VehicleShowInfo;

typedef std::vector<VehicleShowInfo>	VehicleShowInfoVec;
typedef VehicleShowInfoVec::iterator	VehicleShowInfoVecItr;

typedef struct _WndGroupInfo
{
    int			NebulaRouteGroup;
    int			FormMode;
    int			FormationID;
    _WndGroupInfo()
    {
        NebulaRouteGroup = -1;
        FormMode = -1;
        FormationID = -1;
    }

} WndGroupInfo, *LPWndGroupInfo;

typedef std::vector<WndGroupInfo>	WndGroupInfoList, *LPWndGroupInfoList;
typedef WndGroupInfoList::iterator WndGroupInfoListItr;

inline std::string getFormConfigPath()
{
    char filePath[1024] = { 0 };
    sprintf(filePath, "%s", "../data");

    return filePath;
}

class CDlgEditLine;
class CDlgEditGroup : public LyDialog, public qnzkna::TaskManage::INebulaTaskManageObserver
{
    Q_OBJECT

public:
    explicit CDlgEditGroup(QWidget *parent = nullptr);
    ~CDlgEditGroup();

private:
    Ui::CDlgEditGroup *ui;

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

protected:

    virtual void changeEvent(QEvent *event);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_comboBox_3_currentIndexChanged(const QString &arg1);

    void on_comboBox_4_currentIndexChanged(const QString &arg1);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_comboBox_5_currentIndexChanged(const QString &arg1);

    void on_comboBox_8_currentIndexChanged(const QString &arg1);

    void on_comboBox_9_currentIndexChanged(const QString &arg1);

    void on_comboBox_10_currentIndexChanged(const QString &arg1);

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_7_clicked();

    void on_radioButton_8_clicked();

signals:
    void UpdateLineDlgData(unsigned int nTaskID, unsigned int nGroupID);
    void UpdateRouteLineInfoList(unsigned int nTaskID, unsigned int nGroupID, qnzkna::NodeManage::CNodeRelationInfoPtrVector	&vecNodeInfo);
    void ClearLineGroupCombobox();
    void UpdateLineGroupComboboxItem(int nNo, QString strItem);
    void SetLineGroupComboboxCul(int nNo);
    void SetFormLabelTest(QString strName);

public:
    void InitData();

    void DrawWindows();

    inline int getIconDescSize() const
    {
        return 32;
    }

    void drawVehicleIcon(QPainter &mPainter, int nX, int nY, int nW, int nH, int nID);

    void drawVehicleInfoIcon(QPainter &mPainter, int nX, int nY, int nW, int nH, int nID, int Color);;

    QPixmap		m_VehicleIcon[VIEW_NUM];

    E_VIEW_MODE	m_eViewMode;

    QPoint convertWorldToView2D(float fX, float fY);
    QPoint convertWorldToView3D(float fX, float fY, float fZ);
    QPixmap *LoadFromFile(const char *lpszFile);

    void getViewRange(int nW, int nH, float &fRangeX, float &fRangeY);

    QPoint  convertViewToWorld(float fX, float fY);

    void onWindowSizeChanged();

    void getDefaultFormInfo(int num);

    SForm *getFormByIdx(int nIdx);

    bool readFormsFromFile();

    bool UpdateFormsFromFile();

    int FormsListNo;
    bool writeFormsToFile();

    bool readTaskManageInfo();

    void InitUAVListInfo();

    void drawSelectedVehicles(QPainter &mPainter);

    void drawVehicles(QPainter &mPainter);

    void drawVehicleBorder(QPainter &mPainter, bool bSel);

    void drawVehicleInfoShow(QPainter &mPainter);

    void initVehicleInfoShow();

    void UpdateVehicleInfoShow();

    void initVehicleInfoPosition();

    void initVehicleInfoPositionSelected();

    void updateFormsToFormCombo(int nSel = -1);

    void UpdateFormsCombobox();

    int getMouseHoverVehicleID(QPoint point);

    void InitUAVNodeInfo();
    void ClearUAVNodeInfo();

    void ClearWndTaskList();

    void UpdateTrackInfoToWnd();

    void UpdateTrackTaskControl();

    void UpdateTrackRouteGroupControl();

    void UpdateGroupFormationID();

    void SetGroupFormationID(int GroupID, int eMode, int m_FormationID);

    void UpdateGroupFormationToRadioCtrl();

    void UpdateGroupFormationToUAV();

    void UpdatePaintView();

    void ClearWndGroupInfoList();

    void OuttoTree_Vehicle(std::string &strWeaponName, std::string &strWeaponID);

    int GetTaskID();
    int GetGroupID();

    void ClearS_xygsJL_08();
    void ClearS_xygsJL_09();
    void ClearS_xygsJL_17();
    void ClearS_xygsJL_14();
    void ClearS_xygsJL_21();
    void ClearS_xygsJL_23();
    void SetS_xygsJL_08();
    void SetS_xygsJL_17(char *pBuf, int &nlen);
    void bit_set(unsigned char *pdata, unsigned char position, int flag);

    void SetBtnStates(bool bstates);
    void SetFormOrdinalNum(CNebulaRouteGroup &stNebulaRouteGroup);
    void UpdateWndData();
    void UpdateTrackRouteLineControl();
    void InitWindow();

    void InitDistanceCombobox();
    void UpdateDistanceCombobox();

    virtual void Update(qnzkna::TaskManage::INebulaTaskManageService *sub);

    void UpdateChildWndDate();

    void SetWindowStates(E_BTN_STATES eStates);

    void UpdateGroupFromLine(int nGroupID);

    void SetDlgEditLine(CDlgEditLine *pDlg);

    void UpdateSelectedGroup(int nGroupID);

    void UpdateSelectedTask(int nTaskID);

    qnzkna::NodeManage::CNodeRelationInfoPtrVector GetNodeInfoVec();
public:
    qnzkna::NodeManage::CNodeRelationInfoPtrVector	m_vecNodeInfo;

    CDlgEditLine			*m_pDlgEditLine;

    SForm	m_sForm;

    int	m_nSelVehicleIcon;

    SFormList m_vFormList;
    SFormList m_vFormList_ALL;

    CNebulaTask m_CNebulaTask;

    QRect	m_rectView;

    QRect	m_rectInfoView;

    QRect	m_updateView;

    int		m_nW, m_nH;
    int		m_nCX, m_nCY;
    int		m_nCXOri, m_nCYOri;
    float	m_fScale;

    VehicleShowInfo	s_VehicleShowInfo;

    VehicleShowInfoVec		m_VehicleShowInfoVec;

    int		SelectVehicleID;
    QPoint	m_LastMousePt;
    int		eOperType;
    int		m_UAVnumber;
    int		Last_UAVnumber;
    int		LastSelectVehicleID;
    int		m_line_kind;
    QString	m_strTaskFileName;
    int		m_Btn_Flag;
    int		m_btn_Course_OK_Flag;

    unsigned int m_TaskId;

    unsigned int m_TaskId_Last;

    unsigned int m_GroupID;

    unsigned int m_GroupID_last;

    unsigned int m_FormationID;

    WndGroupInfoList	m_vecWndGroupInfoList;

    std::vector<CNebulaTask *>	m_vecWndTaskInfo;

    CNebulaTask::NebulaTaskIDList m_TaskIdList;
    CNebulaRouteGroup::RouteGroupIDList	m_RouteGroupList;
    int	m_GroupNum;
    CNebulaRouteGroup::EFormMode eMode;

    int m_Upload_States;
    int m_Upload_Point_No;
    int m_Upload_Send_No;
    int m_Upload_PointNum;

    int m_update_Flag;
    char		*m_strUAVParamInfo[MAX_BUFFER_LEN + 1];
    int			m_nParamLen;

    bool                  m_InitScale;
    QRect                 m_RectOldClient;

    int					m_nbtnstate;
    E_BTN_STATES		m_EBtnStates;

    qnzkna::TaskManage::INebulaTaskManageService					*m_pITaskManageService = nullptr;
    qnzkna::WeaponManage::IWeaponManageService						*m_pIWeaponManageService = nullptr;
    qnzkna::UAVFormManage::IFormationManageService					*m_pIFormationManageService = nullptr;
    qnzkna::SystemConfig::IConfigProjectControlService				*m_pIConfigProjectControlService = nullptr;
    qnzkna::CCommandUnitManage::ICommandUnitManageService			*m_pICommandUnitManageService = nullptr;

};

#endif
