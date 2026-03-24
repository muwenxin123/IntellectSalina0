#ifndef ZONEMANAGEWIDGET_H
#define ZONEMANAGEWIDGET_H

#include <QWidget>
#include "zoneadditiondrawwidget.h"
#include "cxqyzonetreewidget.h"
#include "CDlgZoneParameterEdit.h"
#include "IZoneManageService.h"
#include "INebulaTaskManageService.h"
#include "IProj.h"
#include "QCString.h"
namespace Ui {
class ZoneManageWidget;
}
class QTableWidget;

class ZoneManageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ZoneManageWidget(QWidget *parent = nullptr);
    ~ZoneManageWidget();

	enum ZoneType { CIRCLE, PIE, ELLIPSE, POLY, RECTANGLE, CIRCLERING, PIERING, POINT, LINE };

	void DrawQYZoneData(QPainter *painter);
	void insertJWDpt(double ll, double bb, bool bEndflg = false);
	void EditMouseMoveJWD(double ll, double bb);
	void WriteKyList(QUYU_Zone &zone, bool InitFlg = true);
	void On2d3dChanged(int wParam);
	void DingWeiqy(const QUYU_Zone &zone);

	void isZoneSelected(bool isSelected);

	void InitZoneData();
	void UpdateWindowData();

	void SetEditStates(bool bEdited);

	void CreateTree();
	void UpdateTreeData();
	void AddZoneTreeItem(const int& nType, QTreeWidgetItem* pRootItem);
	void UpdateParentItem(QTreeWidgetItem* item);
	void UpdateShowStates();

	void TableWidgetAddCheckBox(QTableWidget *tableWidget, int x, int y, QString text, Qt::CheckState CheckState);

	void SetScreenCenter(double &dLon, double &dLat);

	qnzkna::ZoneManage::CZoneBase& getCurZoneBase();
	void setCurZoneBase(qnzkna::ZoneManage::CZoneBase& _curZoneBase);

	void TransmitShowData();
	void DrawTextFunctionTwo(QPainter& painter, IProj* pProj);
	void DrawTextFunctionThree();
	void ClearTextFunctionThree();
	void CalculateLocation(qnzkna::ZoneManage::ZonePointVec ZoneVec, double& dlon, double& dlat);
	//绘制三维文字
	void createUserMesh2Text(IGraphics *pGraphics, std::vector<IText*>& wndVec, ARGB ar, const char* lpszText, double dLng, double dLat, double dAlt);

	void Draw3DTakeOffAndLandImg();
	void Clear3DTakeOffAndLandImg();

	void UpdateLableColorShow();

	void GetLableColor();
public:

	bool b_IsShowTextAll = false;
	std::vector<IText*>m_3DLocPositionMesh2Vec;
	std::vector<ILabel*>m_3DTakeOffMeshVec;

	QCString  m_strDataDir;
	std::map<QUuid, ILabel *>					m_mapLabel;
	std::map<QUuid, std::string>				m_mapLabelPath;
	QMap<std::string, Qt::CheckState> m_CurAreaShowItemMap;
	unsigned int										m_nColor[25];
	std::map<std::vector<std::vector<int>>,bool>        m_SetColorMap;
	std::vector<std::vector<int>> m_vDefaultTRGB;
private:

	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);
	void paintEvent(QPaintEvent *event);

public:
	ZoneAdditionDrawWidget* m_pZoneAdditionDrawWidget = nullptr;

	int m_iCurSelDotIdx;
	bool m_bModifyFlag = false;
	bool m_bCreateFlag = false;

	CXQYZoneTreeWidget *m_QYZoneTree;

public:

private slots:

	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

    void on_pb_add_clicked();

	void on_pb_edit_clicked();

	void on_pb_save_clicked();

	void on_pushButton_4_clicked();

	void on_pb_delete_clicked();

	void on_pb_ghhl_clicked();

	void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

	void on_pbtnAreaName_clicked();

	void on_pushButton_3_clicked();

	void on_checkBox_stateChanged(int nStates);

	void on_checkBox_2_stateChanged(int nStates);

	void on_btnlablecolor_clicked();

	//土地图标
	void on_btnlandIcon_clicked();
public:
	bool isDrowIcon = false;



private:
    Ui::ZoneManageWidget *ui;

	bool													m_bMoveInsert;
	bool													m_bQYZoneSmooth;
	char													m_DwFlashFlag;
	int														m_iQYZoneType;
	int														m_iQYZoneTopHeight;
	int														m_iQYZoneBotHeight;

	CDlgZoneParameterEdit*									m_pChildDlg = nullptr;

	qnzkna::ZoneManage::IZoneManageService*					m_pIZoneManageService = nullptr;

	qnzkna::TaskManage::INebulaTaskManageService*			m_pINebulaTaskManageService = nullptr;

	qnzkna::ZoneManage::CZoneBaseVec						m_ZoneBaseVec;

	qnzkna::ZoneManage::CZoneBase							m_curZoneBase;

	int														m_curRow = -1;

	QStringList												m_TreeHeader;
};

#endif
