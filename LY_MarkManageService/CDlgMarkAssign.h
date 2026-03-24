#ifndef CDLGMARKASSIGN_H
#define CDLGMARKASSIGN_H

#include <QWidget>
#include "IMarkManageServiceActivator.h"
#include "QMutex"

typedef std::map<std::string, std::string> UAVMarkRelationMap;

namespace Ui
{
class CDlgMarkAssign;
}

class CDlgMarkAssign : public QWidget
{
    Q_OBJECT

public:
    explicit CDlgMarkAssign(QWidget *parent = nullptr);
    ~CDlgMarkAssign();

private:
    Ui::CDlgMarkAssign *ui;

    mutable QMutex  								sMutex;

    qnzkna::WeaponManage::MultiWeaponComponentMap	m_UAVMap;

    UAVMarkRelationMap							m_UAVMarkRelationMap;
private:
    virtual void paintEvent(QPaintEvent *event);
signals:
    void RecUpdateWindow(QString strUAVID, QString strMarkID);
public:
    void InitUAVList();
    void SetUAVMarkRelation(std::string &strUAVID, std::string &strMarkID);
    QString GetMarkID(std::string strUAVID);
    void UpdateWindow(QString strUAVID, QString strMarkID);

};

#endif
