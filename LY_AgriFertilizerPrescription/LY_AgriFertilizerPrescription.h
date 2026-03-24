
#ifndef LY_AgriFertilizerPrescription_H
#define LY_AgriFertilizerPrescription_H
#include <QWidget>
#include "LY_AgriFertilizerPrescription.h"
#include <QCString.h>
#include <QMouseEvent>
#include "LY_AgriFertilizerPrescription_global.h"
#include "ly_agrifertilizerpreswidget.h"
#include "ly_agrisprayprescommandwidget.h"
#include "ly_agrifertilizepescviewmodel.h"
#include "AgriFertilizerPrescriptionImpl.h"
class QPainter;
class IProj;

class LY_DomainWidget : public QWidget
{
	Q_OBJECT
public:
	LY_DomainWidget() {};
	~LY_DomainWidget() {};
};


class LY_AGRIFERTILIZERPRESCRIPTION_EXPORT LY_AgriFertilizerPrescription
{ 

public:
    LY_AgriFertilizerPrescription();
    ~LY_AgriFertilizerPrescription();
    inline static LY_AgriFertilizerPrescription&Get();

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
	bool OnNetRecv(int nLength, char *lpData);
	inline const QCString GetDataDir() const { return m_strDataDir; };
	inline void SetDataDir(const char *lpszDir)
	{
		m_strDataDir = lpszDir;
	}
	inline void SetConfDir(const char *lpszDir)
	{
		m_strConfDir = lpszDir;
	}
	inline void SetCoordinateTransformation(IProj *pProj)
	{
		m_pProj = pProj;
	}
	inline LY_DomainWidget* GetDomainWidget()
	{
		return p_DomainWidget;
	}

	LY_AgriFertilizerPresWidget * GetFertilizerPresWidget();
	LY_AgriSprayPresCommandWidget * GetSprayPresCommandWidget();
	LY_AgriFertilizePescViewModel *GetFertiliVIewModel();
private:
	static qnzkna::framework::IRegistry             *s_IRegistryImpl;
	static qnzkna::AgriFertilizerPrescription::IAgriFertilizerPrescriptionImpl   s_AgriFertilizerPrescriptionImpl;

	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;

	LY_AgriFertilizerPresWidget *m_fertilizerPresWidget;
	LY_AgriSprayPresCommandWidget *m_sprayPresWidget;
	LY_DomainWidget *p_DomainWidget = nullptr;
	LY_AgriFertilizePescViewModel *m_viewModel;
};

inline LY_AgriFertilizerPrescription &LY_AgriFertilizerPrescription::Get()
{
    static LY_AgriFertilizerPrescription sLY_MonitorMoudle;
    return sLY_MonitorMoudle;
}


#endif
