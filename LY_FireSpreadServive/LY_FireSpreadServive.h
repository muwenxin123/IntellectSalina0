#ifndef LY_FIRESPREADSERVIVE_H
#define LY_FIRESPREADSERVIVE_H

#include "LY_FireSpreadServive_global.h"
#include <QCString.h>
#include "QWidget"
#include "QMetaType"
#include "IFireSpreadImpl.h"
#include "IProj.h"
#include "ServiceFramework\IRegistry.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

class LY_FIRESPREADSERVIVESHARED_EXPORT LY_FireSpreadServive
{
public:
    LY_FireSpreadServive();
    inline static LY_FireSpreadServive&Get();

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
	static qnzkna::FireSpread::IFireSpreadImpl			s_IFireSpreadImpl;

};

inline LY_FireSpreadServive &LY_FireSpreadServive::Get()
{
    static LY_FireSpreadServive sLY_FireSpreadServive;
    return sLY_FireSpreadServive;
}

#endif
