#ifndef LY_ConcentrationService_H_
#define LY_ConcentrationService_H_
 
#include <QMetaType>
#include <QCString.h>

#include "LY_ConcentrationService_global.h"

#include "ConcentrationMeter/CK_RUT_SL651HEX.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_CONCENTRATIONSERVICE_EXPORT LY_ConcentrationService
{
public:
	LY_ConcentrationService();
	inline static LY_ConcentrationService &Get();

	void Startup();
	void Shutdown();
	void Reset();

public:
	bool DrawGraph(QPainter &painter, IProj *pProj);
	bool DrawDynamic(QPainter &painter, IProj *pProj);
	bool OnMouseButtonDown(QMouseEvent *e);
	bool OnMouseDoubleClick(QMouseEvent *e);
	bool OnCommand(int nID, void *lParam = nullptr);
	bool OnCreate(int wParam, QWidget *pWgt);
	bool OnTimer(int wParam);
	bool OnNetRecv(int nLength, char *lpData);
	void OnEarthMessage(int wParam, void *lParam);
	inline void SetCoordinateTransformation(IProj *pProj)
	{
		m_pProj = pProj;
	}
	bool On2d3dChanged(int wParam);
	inline const QCString GetDataDir() const
	{
		return m_strDataDir;
	}
	inline const QCString GetConfDir() const
	{
		return m_strConfDir;
	}
	inline void SetDataDir(const char *lpszDir)
	{
		m_strDataDir = lpszDir;
	}
	inline void SetConfDir(const char *lpszDir)
	{
		m_strConfDir = lpszDir;
	}
	inline IProj* GetProj() const
	{
		return m_pProj;
	}

	void SetScreenCenter(double dLon, double dLat);

private:
	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;
};

inline LY_ConcentrationService &LY_ConcentrationService::Get()
{
	static LY_ConcentrationService sLY_ConcentrationService;
	return sLY_ConcentrationService;
}
#endif  // LY_ConcentrationService_H