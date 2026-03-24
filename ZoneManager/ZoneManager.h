#ifndef ZoneManager_H
#define ZoneManager_H

#include "IInterface.h"
#include <QPointF>
#include <QPainter>
#include <QCString.h>

class QMouseEvent;
class IProj;
class IUserMesh1;
class IUserMesh2;
class QPainter;
class QWidget;
class CQYZoneDlg;
typedef struct QUYU_Zone_ QUYU_Zone;

class ZoneManager
{
public:
    ZoneManager();
    ~ZoneManager();
    static ZoneManager &Get();

	bool CheckNameStr(const QString &str, int len = 100000);
	QString GetKTimeformat(int dqdkj);
	double StrToJWD(const QString &str);

	double GetAngle(int x1, int y1, int x2, int y2);

	double GetDistance(int x1, int y1, int x2, int y2);
	int GetMapScale();
	QString GetSaveFileNameEx(const QString &filename, int type = 0);

	void X_S(double x, double y, float *l, float *b);
	void S_X(float x, float y, double *xx, double *yy);
	void L_X(double l, double b, double *x, double *y);
	void X_L(double x, double y, double *l, double *b);
	void L_S(double x, double y, int *sx, int *sy);
	void S_L(int sx, int sy, double *x, double *y);

    bool On2d3dChanged(int wParam);
    void OnEarthMessage(int wParam, void *lParam);
	bool DrawGraph(QPainter &painter, IProj *pProj);
	bool DrawDynamic(QPainter &painter, IProj *pProj);
	void OnMouseButtonDown(QMouseEvent *e);
	void OnMouseButtonRelease(QMouseEvent *e);
	void OnMouseMove(QMouseEvent *e);
	bool OnCommand(int nID);
	bool OnCreate(int wParam, QWidget *pTab);
	bool OnTimer(int wParam);

	inline void SetCoordinateTransformation(IProj *pProj)
	{
		m_pProj = pProj;
	};

	void DrawAirLine(QPainter *painter);

	QString  JudgeLandMarkName(double *jd, double *wd);
	QString GetMarkNamebyPoint(double sx, double sy, double *l, double *b);
	QString GetDotNamebyPoint(int sx, int sy, double *l, double *b);
	QString CheckMapObjectByLayerName(int sx, int sy, const QString &layer, float *jd, float *wd);
	QString RtnDir_and_Dis_str(double jd1, double wd1, double jd2, double wd2, int kind = 0);
	double GetCircleK(double x1, double y1, double x2, double y2);

	int IfInPels3(double sx, double sy, double zx, double zy);

	int IfOnLine(double x, double y, double x1, double y1, double x2, double y2, int nPixel = 2, bool bScreen = true);
	double GetLineDir(double x, double y);

	int IfOnQYPot(double jd, double wd, const QUYU_Zone &zone);
	int IfOnQYLine(double jd, double wd, const QUYU_Zone &zone);

	void ConversionPoint(double &x, double &y, double x1, double y1, double x2, double y2);
	bool IsbetweenPoint(double s, double e, double x, double &Angle);

	void InitDbPara();
	void GetPlaneOilInfo();
	void RefreshTrackLayer();
	void DrawDotLine(QPainter *p, int px0, int py0, int px, int py, int px2, int py2, int nCLR);
	inline const QString GetDataDir() const
	{
		return m_strDataDir;
	};
	inline const QString GetConfDir() const
	{
		return m_strConfDir;
	};
	inline void SetDataDir(const char *lpszDir)
	{
		m_strDataDir = tr2(lpszDir);
	};
	inline void SetConfDir(const char *lpszDir)
	{
		m_strConfDir = tr2(lpszDir);
	};

private:
	void DrawArc(QPainter *p, int cx, int cy, int sx, int sy, int ex, int ey, long r);
	void DrawArrow(QPainter *p, int x, int y, float k, int color, int style, int blc);
	void ShowXinXi(double jd, double wd);
	void RButtonDownOnHX(double jd, double wd, double x, double y);

public:
    IProj           *m_pProj;
    QWidget		    *m_pWnd;
    CQYZoneDlg		*m_pQYZoneDlg;
    char            m_nModifyZone;
    bool            m_bRoatedZone;
    bool            m_bRoatingZone;
    bool            m_bJudgelandmark;
    int             m_nAirlineSn;
    int             m_arrowType;
    int             m_mkind;
    int             m_bkind;
    int             m_oriX, m_oriY, m_pyx, m_pyy;
    int             b_oriX, b_oriY, b_pyx, b_pyy;
    double          m_OriLong, m_OriLat;
    double          m_ZOriLong, m_ZOriLat;

    QCString	m_strDbName;
    QCString	m_strDbIP;
    QCString	m_strDbUser;
    QCString	m_strDbPass;
    QCString	m_strDbDriver;

private:
    int         m_yrx;
    QString     m_strDataDir;
    QString     m_strConfDir;
};

#endif
