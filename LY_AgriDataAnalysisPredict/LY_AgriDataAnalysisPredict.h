#ifndef LY_AGRIDATAANALYSISPREDICT_H_
#define LY_AGRIDATAANALYSISPREDICT_H_
 
#include <QMetaType>
#include <QCString.h>

#include "AgriPestAndDiseaseAnalysisWidget.h"
#include "LY_AgriDataAnalysisPredict_global.h"
#include "AgriDataAnalysisDBHandler.h"
#include "IAgriDataAnalysisPredictServiceImpl.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_AGRIDATAANALYSISPREDICTSHARED_EXPORT LY_AgriDataAnalysisPredict
{
public:
	LY_AgriDataAnalysisPredict();
	inline static LY_AgriDataAnalysisPredict &Get();

	void Startup();
	void Shutdown();
	void Reset();

public:
	inline AgriDataAnalysisDBHandler* GetpDbHandler()
	{
		return m_pAgriDataAnalysisDBHandler;
	}
	qnzkna::ZoneManage::CZoneBase GetActiveFieldZone();

	inline AgriPestAndDiseaseAnalysisWidget* GetAgriPestAnalysisWidget()
	{
		return m_pAgriPestAndDiseaseAnalysisWidget;
	}
	void ReCreateDBHandler();

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
	void updatePickTargetPoint(double dLon, double dLat, float fAlt);

private:
	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;

private:
	AgriPestAndDiseaseAnalysisWidget	*m_pAgriPestAndDiseaseAnalysisWidget;
	AgriDataAnalysisDBHandler			*m_pAgriDataAnalysisDBHandler;

private:
	static qnzkna::framework::IRegistry*           s_IRegistryImpl;
	static qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl s_IAgriDataAnalysisPredictServiceImpl;
};

inline LY_AgriDataAnalysisPredict &LY_AgriDataAnalysisPredict::Get()
{
	static LY_AgriDataAnalysisPredict sLY_AgriDataAnalysisPredict;
	return sLY_AgriDataAnalysisPredict;
}
#endif  // LY_AgriDataAnalysisPredict_H