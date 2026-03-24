#ifndef LY_AGRIMULTISPECTRALSENSOR_H_
#define LY_AGRIMULTISPECTRALSENSOR_H_
 
#include <QMetaType>
#include <QCString.h>

#include "AgriMultispectralSensorWidget.h"
#include "LY_AgriMultispectralSensor_global.h"
#include "AgriMultispectralAnalysis.h"
#include "AgriMultispectralSensorDBHandler.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_AGRIMULTISPECTRALSENSORSHARED_EXPORT LY_AgriMultispectralSensor
{
public:
	LY_AgriMultispectralSensor();
	inline static LY_AgriMultispectralSensor &Get();

	void Startup();
	void Shutdown();
	void Reset();

public:
	inline AgriMultispectralSensorDBHandler* GetpDbHandler()
	{
		return m_pAgriMultispectralSensorDBHandler;
	}
	inline AgriMultispectralAnalysis* GetpAgriMultispectralAnalysis()
	{
		return m_pAgriMultispectralAnalysis;
	}

public:
	bool DrawGraph(QPainter &painter, IProj *pProj);
	bool DrawDynamic(QPainter &painter, IProj *pProj);
	bool OnMouseButtonDown(QMouseEvent *e);
	bool OnMouseDoubleClick(QMouseEvent *e);
	bool OnCommand(int nID, void *lParam = nullptr);
	bool OnCreate(int wParam, QWidget *pWgt);
	bool OnTimer(int wParam);
	bool OnNetRecv(int nLength, char *lpData);
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

	void SetScreenCenter(const double dLon, const double dLat);

private:
	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;

private:
	AgriMultispectralSensorWidget				*m_pAgriMultispectralSensorWidget;
	AgriMultispectralAnalysis					*m_pAgriMultispectralAnalysis = nullptr;
	AgriMultispectralSensorDBHandler			*m_pAgriMultispectralSensorDBHandler = nullptr;
};

inline LY_AgriMultispectralSensor &LY_AgriMultispectralSensor::Get()
{
	static LY_AgriMultispectralSensor sLY_AgriMultispectralSensor;
	return sLY_AgriMultispectralSensor;
}
#endif  // LY_AgriMultispectralSensor_H