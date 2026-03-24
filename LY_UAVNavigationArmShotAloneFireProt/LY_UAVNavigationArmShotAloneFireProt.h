#ifndef LY_UAVNAVIGATIONARMSHOTALONEFIREPROT_H
#define LY_UAVNAVIGATIONARMSHOTALONEFIREPROT_H

#include <QMetaType>

#include "LY_UAVNavigationArmShotAloneFireProt_global.h"
#include "IUAVNavigationArmShotAloneFireProtDialog.h"

#include <QCString.h>

#include "Protocol/XygsZC/Bag_XygsZCDefines.h"

class QPainter;
class QMouseEvent;
class QWidget;
class IProj;

class LY_UAVNAVIGATIONARMSHOTALONEFIREPROTSHARED_EXPORT LY_UAVNavigationArmShotAloneFireProt
{
public:
    LY_UAVNavigationArmShotAloneFireProt();
    inline static LY_UAVNavigationArmShotAloneFireProt &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int wParam, void *lParam);
    bool OnCreate(int wParam, QWidget *pTab);
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

    const std::list<IUAVNavigationArmShotAloneFireProtDialog*> *getPIUAVNavigationArmShotAloneFireProtDialog() const;

    QWidget *getPWnd() const
    {
        return m_pWnd;
    }

private:
    IProj       *m_pProj;
    QCString	m_strDataDir;
    QCString	m_strConfDir;

private:
    std::list<IUAVNavigationArmShotAloneFireProtDialog*>   m_pIUAVNavigationArmShotAloneFireProtDialogList;
    QWidget                                                *m_pWnd = nullptr;
};

inline LY_UAVNavigationArmShotAloneFireProt &LY_UAVNavigationArmShotAloneFireProt::Get()
{
    static LY_UAVNavigationArmShotAloneFireProt sLY_UAVNavigationArmShotAloneFireProt;
    return sLY_UAVNavigationArmShotAloneFireProt;
}

#endif
