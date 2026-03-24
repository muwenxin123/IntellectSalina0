#ifndef LY_AGRIMACHINEMANAGER_H_
#define LY_AGRIMACHINEMANAGER_H_
 
#include <QMetaType>
#include <QCString.h>

#include "LY_AgriMachineManager_global.h"
#include "AgriMachineManagerDialog.h"
#include "AgriMachineDatabaseHandler.h"
#include "AgriMachineManager.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_AGRIMACHNIEMANAGERSHARED_EXPORT LY_AgriMachineManager
{
public:
	LY_AgriMachineManager();
	inline static LY_AgriMachineManager &Get();

	void Startup();
	void Shutdown();
	void Reset();

public:
	inline AgriMachineDatabaseHandler* GetpDbHandler() { return m_pAgriMachineDatabaseHandler; }
	inline AgriMachineManager* GetMachineManager() { return m_pAgriMachineManager; }

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
	AgriMachineManagerDialog	*m_pAgriMachineManagerDialog;
	AgriMachineDatabaseHandler	*m_pAgriMachineDatabaseHandler;
	AgriMachineManager			*m_pAgriMachineManager;
};

inline LY_AgriMachineManager &LY_AgriMachineManager::Get()
{
	static LY_AgriMachineManager sLY_AgriMachineManager;
	return sLY_AgriMachineManager;
}
#endif  // LY_AgriMachineManager_H