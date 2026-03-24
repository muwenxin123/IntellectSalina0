#ifndef DRAWVEHICLEICON_H
#define DRAWVEHICLEICON_H

#include <QWidget>
#include "CDlgEditGroupp.h"

namespace Ui
{
	class DrawVehicleIcon;
}

class DrawVehicleIcon : public QWidget
{
    Q_OBJECT

public:
    explicit DrawVehicleIcon(QWidget *parent = nullptr);
    ~DrawVehicleIcon();

    void setText(QString ) {}
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    Ui::DrawVehicleIcon *ui;

	void mousePressEvent(QMouseEvent *event);

	void mouseReleaseEvent(QMouseEvent *event);

	void mouseMoveEvent(QMouseEvent *event);

public:

	void InitData();

	void drawVehicleInfoIcon(QPainter& mPainter, int nX, int nY, int nW, int nH, int nID, int Color);;

	void drawSelectedVehicles(QPainter& mPainter);

	QPoint convertWorldToView2D(float fX, float fY);
	QPoint convertWorldToView3D(float fX, float fY, float fZ);

	void getViewRange(int nW, int nH, float& fRangeX, float& fRangeY);

	QPoint  convertViewToWorld(float fX, float fY);

	inline int getIconDescSize() const
	{
		return 32;
	}

	void drawVehicleBorder(QPainter& mPainter, bool bSel);

	void drawVehicleInfoShow(QPainter& mPainter);

	void initVehicleInfoShow();

	void initVehicleInfoPosition();

	void initVehicleInfoPositionSelected();

	void SetVehicleShowInfo(VehicleShowInfoVec& DataVec);

	VehicleShowInfoVec GetVehicleShowInfo();

	void SetUAVNumber(int nNumber);

	void SetScale(float fScale);

	bool JudgeSelected(int nID);

	void SetUAVForm(SForm& sform);

	void SetLineKind(int nKind);

	void SetWndStates(E_BTN_STATES btnStates);

	int getMouseHoverVehicleID(QPoint point);

	bool getMouseHoverVehicleIDLegend(QPoint point);
public:
	QRect				m_rectView;
	QRect				m_rectInfoView;
	QRect				m_updateView;

	int					m_nW, m_nH;
	int					m_nCX, m_nCY;
	int					m_nCXOri, m_nCYOri;
	float				m_fScale;
	int					m_UAVnumber;
	int					Last_UAVnumber;
	int					SelectVehicleID;
	int					m_line_kind;
	int					m_Btn_Flag;
	int					LastSelectVehicleID;

	QPixmap				m_VehicleIcon[VIEW_NUM];

	VehicleShowInfo		s_VehicleShowInfo;
	VehicleShowInfoVec	m_VehicleShowInfoVec;

	E_VIEW_MODE			m_eViewMode;

	SForm				m_sForm;

	std::vector<CNebulaTask*>	m_vecWndTaskInfo;

	unsigned int m_TaskId;

	unsigned int m_TaskId_Last;

	unsigned int m_GroupID;

	unsigned int m_GroupID_last;

	E_BTN_STATES		m_EBtnStates;

	QPoint	m_LastMousePt;

};

#endif
