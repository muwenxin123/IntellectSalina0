#ifndef CDLGEDITLINE_H
#define CDLGEDITLINE_H

#include <QWidget>
#include "LyDialog.h"
#include "LineEditHead.h"
#include "CommonStruct.h"
#include "DataManage/NebulaTask/NebulaRouteLine.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/Formation/Formation.h"
#include "DataManage/IFormationManageService.h"
#include "DataManage/IWeaponManageService.h"

#include "CoordinateTranfer.h"
#include "Protocol/XygsJL/Bag_XygsJLDefines.h"
#include "DataManage/XYWeaponManage/XYWeaponManage.h"
#include "DataManage/IMarkManageService.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "CDlgMissionList.h"

namespace Ui
{
class CDlgEditLine;
}

class CDlgEditGroup;
class CDlgEditLine : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgEditLine(QWidget *parent = nullptr);
    ~CDlgEditLine();

private:
    Ui::CDlgEditLine *ui;

private:
    int											m_Ctrl_Table_Tabnum;
    std::string									m_strProtocolType;

    unsigned int								m_nColor[25];
    int											m_nColorNumber;

    STWndRouteLineInfoList						m_stWndRouteLineInfoList;

    int											m_Item;
    int											m_SubItem;
    int											rowCount;
    int											m_ComItem;
    int											m_ComSubItem;
    bool										needSave;
    bool										ComneedSave;
    bool										ComneedSave1;
    bool										ComneedSave2;
    QString										m_StrEditForList;

    int											m_nCourseNum;
    int											m_nHeight;
    int											m_Last_Tab_No;

    int											TABITEMNum;

    int											m_Tab_Num;

    unsigned int								m_TaskId;

    unsigned int								m_GroupID;

    unsigned int								m_LineID;

    unsigned int								m_PointID;

    int											m_Hitrow;
    int											m_Hitcol;

    std::list<WndUpdatePointInfo>				m_UpdatePointInfoList;

    CNebulaRoutePoint							m_NebulaRoutePoint;

    CNebulaRouteLine::RouteLineIDList			m_Routelinelist;
    CNebulaRoutePoint::RoutePointIDList			m_RoutePointIDlist;

    int											m_Upload_States;
    int											m_Upload_Point_No;
    int											m_Upload_Send_No;
    int											m_Upload_PointNum;

    xygsJL_09									s_xygsJL_09;
    xygsJL_V7_09								s_xygsJL_09_54;
    xygsJL_08									s_xygsJL_08;
    xygsJL_V7_08								s_xygsJL_08_54;
    xygsJL_17									s_xygsJL_17;

    xygsJL_14									s_xygsJL_14;

    volatile int								m_update_Flag = 0;
    char										*m_strUAVParamInfo[MAX_BUFFER_LEN + 1];
    int											m_nParamLen;

    EeditStates									m_bLiseEditStates;

    NebulaRoutePointVec							m_NebulaRoutePointVecTemp;
    NebulaRoutePointVec							m_NebulaRoutePointVecFinal;
    double										m_dAutoLineHeight;
    double										m_dAutoLineSpeed;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::WeaponManage::IWeaponManageService				*m_pIWeaponManageService = nullptr;

    qnzkna::SystemConfig::IConfigProjectControlService		*m_pIConfigProjectControlService = nullptr;
    qnzkna::MarkManage::IMarkManageService			*m_pIMarkManageService = nullptr;

    QMenu										*m_TableMenu;
    QAction										*Startuavroute;
    QAction										*Stopuavroute;
    QAction										*Removepoint;
    QAction										*Insetpoint;
    QAction										*Autoqeneration;
    QAction										*Upload;
    QAction										*Saveplanepoint;

    QAction										*Updateorigin;
    QAction										*Removeallpoint;
    QAction										*Adjustarea;
    QAction										*SpeedEconomy;
    QAction										*SpeedMin;
    QAction										*SpeedMax;
    QAction										*AddHeight;
    QAction										*LessenHeight;
    QAction										*HoverSet;
    QAction										*HoverSelect;
    QAction										*HoverFinish;
    QAction										*HoverClear;
    QAction										*SetFlayGoal;
    QAction										*landCurrentVehiclePosition;
    QAction										*GenerateLine;
    QAction										*EditCodeSet;

    QTimer										*m_UpLoadTimer;
    QTimer										*m_DownLoadTimer;
    QTimer										*m_UpLoadTimer54;
    QTimer										*m_DownLoadTimer54;

    bool										m_RecvXYUAVParameterInfo;
    int											m_DownLoadTimeCount;
    int											m_DownLoadMaxCount;
    int											m_DownLoadTryTimeCount;
    int											m_DownLoadTimeCount54;
    int											m_DownLoadMaxCount54;
    int											m_DownLoadTryTimeCount54;

    CXYWeaponManage								m_CXYWeaponManage;
    CNeubulaMissionVec							m_MissionVec;
    std::map<std::string, std::string>			m_SystemSysNameAndID;

signals:
    void UpdateUAVParameterInfoSig(const QString strWeaponId, const QByteArray &message);

protected:

    virtual void changeEvent(QEvent *event);

public:
    void InitWndData();

    WndRouteLineInfoList GetWndRoutlineInfoList();

    void UpdateTrackInfoToListControl(int Culsel);

    void UpdateXYTrackInfoToListControl(int Culsel, WndRouteLineInfoList::iterator *iter_list);
    void UpdateXYTrackInfoToListControl_54(int Culsel, WndRouteLineInfoList::iterator *iter_list);

    void UpdateMavTrackInfoToListControl(WndRouteLineInfoList::iterator *iter_list);

    void UpdateTrackInfoToListHeading(int Culsel);

    void ClearWndRouteLineInfoList();

    void SendBagData(int nPointID, std::string &strWeaponName, std::string &strWeaponID, char *pBuf, int nBufLen);
    void SendBagData54(int nPointID, std::string &strWeaponName, std::string &strWeaponID, char *pBuf, int nBufLen);
    void ClearS_xygsJL_08();
    void ClearS_xygsJL_08_54();
    void ClearS_xygsJL_09();
    void ClearS_xygsJL_09_54();
    void ClearS_xygsJL_17();
    void ClearS_xygsJL_14();
    void SetS_xygsJL_09(char *pBuf, int &nlen, int nPointID);
    void SetS_xygsJL_09_54(char *pBuf, int &nlen, int nPointID);
    void SetS_xygsJL_17(char *pBuf, int &nlen, int &nXYgroupID, int &nXYID);
    void bit_set(unsigned char *pdata, unsigned char position, int flag);

    void DealUAVParameterInfo(const QString strWeaponId, const QByteArray &message);
    void UpdateUAVParameterInfo(const QString strWeaponId, const QByteArray message);
    void UpdateUAVInfoToWnd(xygsJL_09 s_xygsJL_09);
    void UpdateUAVInfoToWnd54(xygsJL_V7_09 s_xygsJL_09_54);
    bool AddNebulaRoutePoint(int nLineID, int nPointID, CNebulaRoutePoint &stpoint);

    void HandleXYUAVParameterInfo(const QString strWeaponId, const QByteArray message);
    void HandleXYUAVParameterInfo_54(const QString strWeaponId, const QByteArray message);

    typedef enum
    {
        AckNone,
        AckMissionCount,
        AckMissionItem,
        AckMissionRequest,
        AckMissionClearAll,
        AckGuidedItem,
    } AckType_t;

    typedef enum
    {
        TransactionNone,
        TransactionRead,
        TransactionWrite,
        TransactionRemoveAll
    } TransactionType_t;

    const std::string GetCurWeaponComponentProtocolType(int Culsel);

#if 0

    void UpdateTrackInfoToListControl(int Culsel);

    void UpdateXYTrackInfoToListControl(int Culsel, WndRouteLineInfoList::iterator *iter_list);
    void UpdateXYTrackInfoToListControl_54(int Culsel, WndRouteLineInfoList::iterator *iter_list);

    void UpdateMavTrackInfoToListControl(WndRouteLineInfoList::iterator *iter_list);

    void SebuildFacts();

    void UpdateTrackInfoToListHeading(int Culsel);

    void ClearWndRouteLineInfoList();

    std::string					getSelectedWeaponID();

    void CreateWinqPlaneLine(double x, double y, int nPointID, double dFirstPointX, double dFirstPointY, double *dNewPointX, double *dNewPointY, double *dLast_Angel);
    void ClearS_xygsJL_21();
    void ClearS_xygsJL_23();
    void SetS_xygsJL_08();
    void SetS_xygsJL_08_54();

#endif

    void TabWidgetConnet(int Culsel);

    void CreateTableMenu();
    EeditStates GetLineEditStates();
    bool AddLinePoint(double dLon, double dLat);
    bool AddLinePointMouseDoubleClick(double dLon, double dLat, double dHeight);
    bool InsertLinePoint(double dLon, double dLat, double dHeight);
    bool UpdateLinePoint(int nLineID, int nPointID, double dLon, double dLat);
    bool UpdateLinePointMouseMove(int nLineID, int nPointID, double dLon, double dLat);
    void SetSelectedStates(int nLineID, int nPointID);
    void ClearSelectedStates();
    void UpdateFromTableCombobox(int nRow, int nCol, int nIndex);
    void UpdateFromTableText(int nRow, int nCol, QString str);
    void UpdateTableConnect(bool bStates);
    WndRouteLineInfo *GetLineInfoByID(std::string strID);
    std::string GetLineMarkByID(std::string strID);
    void UpdateLineLength(std::vector<WndRoutePoint> &vecNebulaRoutePoint);

    void TableWidgetDisconnectCellChanged(int nCul);
    void TableWidgetConnectCellChanged(int nCul);

    void UpdataDataFormServer();

    void UpdateMissionCombobox();

    void UpdateWindowData(const std::string strMissionID);

    void UpdateTabControl();

    void localWeaponNameBindToId();

signals:
    void SaveXMl();
    void UpdateSelectGroup(int nGroupID);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

    void OnStartuavroute();

    void OnStopuavroute();

    void OnRemovepoint();

    void OnInsetpoint();

    void OnAutoqeneration();

    void OnUpload();

    void OnSaveplanepoint();

    void OnStraightpoint();

    void OnUpdateorigin();

    void OnRemoveallpoint();

    void OnSetFlayGoal();

    void OnLandCurrentVehiclePosition();

    void OnEditCodeSet();

    void on_tableWidget_cellChanged(int row, int column);

    void on_combobox_08_currentIndexChanged(int index);

    void on_combobox_10_currentIndexChanged(int index);

    void on_combobox_11_currentIndexChanged(int index);

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void StopAllTimer();
    void on_timeOut();
    void on_DowntimeOut();
    void on_timeOut54();
    void on_DowntimeOut54();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_8_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void lineShowStateAll(bool valueFromSettings);

    void on_pushButton_9_clicked();

public:
    void SetDlgMissionList(CDlgMissionList *pDlgList);
    CDlgMissionList	*m_pDlgMissionList = nullptr;
    void ShowLines(QStringList &LineList, double &dLon, double &dLat, bool bAgainFlag);
    void UpdateMarkCombobox(std::string strMarkID);
    std::string GetMarkIDString();

    void UpdateDataFromActivatingActionIDChange();

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void showEvent(QShowEvent *event);
};

#endif
