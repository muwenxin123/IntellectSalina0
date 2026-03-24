#ifndef LY_AGRISOILANALYSISPREDICT_h__
#define LY_AGRISOILANALYSISPREDICT_h__

#include <QMetaType>
#include <QCString.h>

#include "AgriSoilTestingAnalysisWidget.h"
#include "LY_AgriSoilAnalysisPredict_global.h"
#include "AgriSoilAnalysisDBHandler.h"
#include "IAgriSoilTestingAnalysisServiceImpl.h"
#include "SaltFieldEventAnalysisWgt.h"
#include "ISaltDetailedInformation.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_AGRISOILTESTINGANALYSISSHARED_EXPORT LY_AgriSoilAnalysisPredict
{
public:
	LY_AgriSoilAnalysisPredict();
	inline static LY_AgriSoilAnalysisPredict &Get();

	void Startup();
	void Shutdown();
	void Reset();

	QString GetAreaID(double lon, double lat);
	bool AddEvent(SaltFieldEventInfo& info);

public:
	inline AgriSoilAnalysisDBHandler* GetpDbHandler()
	{
		return m_pAgriDataAnalysisDBHandler;
	}
	qnzkna::ZoneManage::CZoneBase GetActiveFieldZone();

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
	inline AgriSoilTestingAnalysisWidget* GetAgriPestAnalysisWidget()
	{
		return m_pAgriPestAndDiseaseAnalysisWidget;
	}
	inline ISaltDetailedInformation* GetISaltDetailedInformation() {
		return m_pISaltDetailedInformation;
	}

	void SetScreenCenter(double dLon, double dLat);

private:
	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;

private:
	SaltFieldEventAnalysisWgt*			m_pSaltFieldEventAnalysisWgt;
	AgriSoilTestingAnalysisWidget	*m_pAgriPestAndDiseaseAnalysisWidget;
	AgriSoilAnalysisDBHandler			*m_pAgriDataAnalysisDBHandler;
	ISaltDetailedInformation*			m_pISaltDetailedInformation;
private:
	static qnzkna::framework::IRegistry*           s_IRegistryImpl;
	static qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl s_IAgriSoilTestingAnalysisServiceImpl;
};

inline LY_AgriSoilAnalysisPredict &LY_AgriSoilAnalysisPredict::Get()
{
	static LY_AgriSoilAnalysisPredict sLY_AgriDataAnalysisPredict;
	return sLY_AgriDataAnalysisPredict;
}
#endif // LY_AGRISOILANALYSISPREDICT_h__
