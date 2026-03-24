#ifndef LY_AGRIFARMMANAGER_H_
#define LY_AGRIFARMMANAGER_H_
 
#include <QMetaType>
#include <QCString.h>
#include <QMetaType>

#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "LY_AgriFarmManager_global.h"
#include "AgriFarmManagerDialog.h"
#include "AgriFarmDatabaseHandler.h"
#include "IAgriFarmManagerServiceImpl.h"
#include "AgriLegendPopup.h"
#include "LY_AgriFieldInfoWidget.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_AGRIFARMMANAGERSHARED_EXPORT LY_AgriFarmManager
{
public:
	LY_AgriFarmManager();
	inline static LY_AgriFarmManager &Get();

	void Startup();
	void Shutdown();
	void Reset();

public:
	inline AgriFarmDatabaseHandler* GetpDbHandler()
	{
		return m_pAgriFarmDatabaseHandler;
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
	AgriLegendPopup * GetLegendPopup();
	LY_AgriFieldInfoWidget *GetAgriFieldInfoWidget();

private:
	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;

private:
	AgriFarmManagerDialog	*m_pAgriFarmManagerDialog;
	AgriFarmDatabaseHandler	*m_pAgriFarmDatabaseHandler;
	AgriLegendPopup				*m_pLegendPopup;
	LY_AgriFieldInfoWidget      *m_pAgriFieldInfoWidget;
	
private:
	static qnzkna::framework::IRegistry*           s_IRegistryImpl;
	static qnzkna::AgriFarmManager::IAgriFarmManagerServiceImpl s_IAgriFarmManagerServiceImpl;
};

inline LY_AgriFarmManager &LY_AgriFarmManager::Get()
{
	static LY_AgriFarmManager sLY_AgriFarmManager;
	return sLY_AgriFarmManager;
}

#endif  // LY_AgriFarmManager_H