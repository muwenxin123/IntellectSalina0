#ifndef LY_ZONEMANAGERSERVIVE_H
#define LY_ZONEMANAGERSERVIVE_H

#include "LY_ZoneManagerServive_global.h"
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include <QCString.h>
#include "QWidget"
#include "QMetaType"
#include "IZoneManagerImpl.h"
#include "IProj.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

class LY_ZONEMANAGERSERVIVESHARED_EXPORT LY_ZoneManagerServive
{
public:
    LY_ZoneManagerServive();
    inline static LY_ZoneManagerServive&Get();

    void Startup();
    void Shutdown();
    void Reset();
    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
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
	inline IProj *GetProj() const
	{
		return m_pProj;
	}

	void OnEarthMessage(int wParam, void * lParam);

	void X_S(double x, double y, float *l, float *b);
	void S_X(float x, float y, double *xx, double *yy);
	void L_X(double l, double b, double *x, double *y);
	void X_L(double x, double y, double *l, double *b);
	void L_S(double x, double y, int *sx, int *sy);
	void S_L(int sx, int sy, double *x, double *y);

public:
	IProj       *m_pProj;
private:
    QCString	m_strDataDir;
    QCString	m_strConfDir;

private:
	static qnzkna::framework::IRegistry*				s_IRegistryImpl;
	static qnzkna::ZoneManage::IZoneManagerImpl			s_IZoneManagerImpl;

};

inline LY_ZoneManagerServive &LY_ZoneManagerServive::Get()
{
    static LY_ZoneManagerServive sLY_ZoneManagerServive;
    return sLY_ZoneManagerServive;
}

#endif
