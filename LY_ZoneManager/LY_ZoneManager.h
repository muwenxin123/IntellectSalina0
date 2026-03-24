#ifndef LY_ZONEMANAGER_H
#define LY_ZONEMANAGER_H

#include "LY_ZoneManager_global.h"
#include <QCString.h>
#include "zonemanagewidget.h"
#include "IProj.h"
#include "CDlgZoneRuleManage.h"
#include "CDlgZoneFileManage.h"
#include "GetHeight.h"
#include "IZoneTypeMatchDialog.h"
#include "MeteorologicalIntelligenceWidget.h"
#include "FireSpreadWidget.h"
#include "ICDlgSet3DLableColor.h"
#include "CDlgZoneScale.h"
class QPainter;
class QMouseEvent;
class QTabWidget;
class QWidget;

class LY_ZONEMANAGERSHARED_EXPORT LY_ZoneManager
{
public:
    LY_ZoneManager();
    inline static LY_ZoneManager&Get();

	void X_S(double x, double y, float *l, float *b);
	void S_X(float x, float y, double *xx, double *yy);
	void L_X(double l, double b, double *x, double *y);
	void X_L(double x, double y, double *l, double *b);
	void L_S(double x, double y, int *sx, int *sy);
	void S_L(int sx, int sy, double *x, double *y);

    void Startup();
    void Shutdown();
    void Reset();
    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    void OnMouseButtonDown(QMouseEvent *e);
	void OnMouseButtonRelease(QMouseEvent *e);
	void OnMouseMove(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
	bool OnCommand(int nID, void *lParam);
    bool OnCreate(int wParam, QWidget *pWidget);
    bool OnTimer(int wParam);
    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    };
    inline const QCString GetDataDir() const { return m_strDataDir; };
    inline const QCString GetConfDir() const { return m_strConfDir; };
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    };
    inline void SetConfDir(const char *lpszDir)
    {
        m_strConfDir = lpszDir;
    };

	bool is_point_in_zone(const QPointF point, const std::vector<QPointF>ZonePoints);

	int IfOnQYLine(double jd, double wd, const QUYU_Zone &zone);
	int IfOnQYPot(double jd, double wd, const QUYU_Zone &zone);
	int IfOnLine(double x, double y, double x1, double y1, double x2, double y2, int nPixel = 2, bool bScreen = true);
	bool On2d3dChanged(int wParam);
	QString RtnDir_and_Dis_str(double jd1, double wd1, double jd2, double wd2, int kind = 0);
	double StrToJWD(const QString &str);

	ZoneManageWidget* GetPZoneManageWidget()const;
	CDlgZoneRuleManage* GetPDlgZoneRuleManage()const;
	CDlgZoneFileManage* GetPDlgZoneFileManage()const;
	IZoneTypeMatchDialog* GetPZoneTypeMatchDialog()const;
	ICDlgSet3DLableColor* GetCDlgSet3DLableColor() const;
public:
	char            m_nModifyZone;
	double          m_ZOriLong, m_ZOriLat;
	int             m_oriX, m_oriY, m_pyx, m_pyy;
	int             b_oriX, b_oriY, b_pyx, b_pyy;
	int             m_mkind;
	int             m_bkind;
    IProj*			m_pProj;
	static CGetHeight s_CGetHeight;

	//Õ¡µÿÕº±Í
	std::vector<QPointF> m_vlandIocn;
	QString m_strIconPath;
private:
    QCString								m_strDataDir;
    QCString								m_strConfDir;

	ZoneManageWidget*						m_pZoneManageWidget = nullptr;
	CDlgZoneRuleManage*						m_pDlgZoneRuleManage = nullptr;
	CDlgZoneFileManage*						m_pDlgZoneFileManage = nullptr;
	IZoneTypeMatchDialog*					m_pZoneTypeMatchDialog = nullptr;
	MeteorologicalIntelligenceWidget*		m_pMeteorologicalIntelligenceWidget = nullptr;
	FireSpreadWidget*						m_pFireSpreadWidget = nullptr;
	ICDlgSet3DLableColor*					m_pCDlgSet3DLableColor = nullptr;
	CDlgZoneScale*							m_pDlgZoneScale = nullptr;

	bool									m_pZoneSelected = false;
	bool									m_pDragPointSelected = false;
	int										m_pDragIndex = -1;
};

inline LY_ZoneManager &LY_ZoneManager::Get()
{
    static LY_ZoneManager sLY_ZoneManager;
    return sLY_ZoneManager;
}

#endif
