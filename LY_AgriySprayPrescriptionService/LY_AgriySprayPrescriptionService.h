#ifndef LY_AgriySprayPrescriptionService_H
#define LY_AgriySprayPrescriptionService_H

#include <QMetaType>
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ISprayPrescriptionImpl.h"

#include "LY_AgriySprayPrescriptionService_global.h"
#include <QCString.h>
#include <QWidget>
#include "LY_AgriFertiliResultWidget.h"
#include "LY_ShowVideoData.h"
#include "LY_ShowTaskDataInfo.h"
#include "LY_ShowWeatherDataInfo.h"
#include "LY_AgriSprayCommandWidget.h"


class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

class LY_DomainWidget : public QWidget
{
	Q_OBJECT
public:
	LY_DomainWidget() {};
	~LY_DomainWidget() {};
};

class LY_AGRIYSPRAYPRESCRIPTIONSERVICESHARED_EXPORT LY_AgriySprayPrescriptionService
{
public:
    LY_AgriySprayPrescriptionService();
    inline static LY_AgriySprayPrescriptionService &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QWidget *pTab);
    bool OnTimer(int wParam);
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
	inline LY_DomainWidget* GetDomainWidget()
	{
		return p_DomainWidget;
	}
	LY_ShowVideoData *GetShowVideoData() const;
	LY_ShowTaskDataInfo *GetShowTaskDataInfo() const;
	LY_ShowWeatherDataInfo *GetShowWeatherDataInfo() const;
	LY_AgriFertiliResultWidget *GetAgriFertiliResultWidget() const;
	
	LY_AgriSprayCommandWidget *GetAgriSprayCommandWidget();


private:
    IProj       *m_pProj;
    QCString	m_strDataDir;
    QCString	m_strConfDir;
public:
	LY_ShowVideoData *m_ShowVideoData;
	LY_ShowTaskDataInfo *m_ShowTaskDataInfo;
	LY_ShowWeatherDataInfo *m_ShowWeatherDataInfo;
	bool         m_IsShowReightWidget = true;
private:
    static qnzkna::framework::IRegistry             *s_IRegistryImpl;
    static qnzkna::AgriSprayPrescription::ISprayPrescriptionImpl   s_ISprayPrescriptionImpl;


	LY_DomainWidget * p_DomainWidget;

	LY_AgriFertiliResultWidget *m_AgriFertiliResultWidget = nullptr;
	LY_AgriSprayCommandWidget *m_commandWidget = nullptr;
};

inline LY_AgriySprayPrescriptionService &LY_AgriySprayPrescriptionService::Get()
{
    static LY_AgriySprayPrescriptionService sLY_AgriySprayPrescriptionService;
    return sLY_AgriySprayPrescriptionService;
}

#endif
