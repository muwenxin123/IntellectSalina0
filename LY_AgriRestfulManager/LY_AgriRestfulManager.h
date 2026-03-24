#ifndef LY_AGRIRESTFULMANAGER_H_
#define LY_AGRIRESTFULMANAGER_H_
 
#include <memory>
#include <QMetaType>
#include <QCString.h>

#include "LY_AgriRestfulManager_global.h"
#include "task/ITask.h"


class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_AgriRestfulManagerPrivate;
class LY_AGRIRESTFULMANAGERSHARED_EXPORT LY_AgriRestfulManager
{
public:
	LY_AgriRestfulManager();
	inline static LY_AgriRestfulManager &Get();

	void Startup();
	void Shutdown();
	void Reset();

public:
	int addTask(std::shared_ptr<ITask> taskptr, bool priority = false);
	std::shared_ptr<ITask> isExistTaskProcess();

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
	LY_AgriRestfulManagerPrivate* _p;
};

inline LY_AgriRestfulManager &LY_AgriRestfulManager::Get()
{
	static LY_AgriRestfulManager sLY_AgriRestfulManager;
	return sLY_AgriRestfulManager;
}
#endif  // LY_AgriRestfulManager_H