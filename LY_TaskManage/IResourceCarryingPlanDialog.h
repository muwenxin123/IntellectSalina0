#ifndef IRESOURCECARRYINGPLANDIALOG_H
#define IRESOURCECARRYINGPLANDIALOG_H

#include <QWidget>
#include "LyDialog.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "QMenu"
#include "lY_TaskManage.h"
#include "ISAddCarryingPlanResDialog.h"

namespace Ui
{
class IResourceCarryingPlanDialog;
}
using namespace std;
class IResourceCarryingPlanDialog : public LyDialog
{
    Q_OBJECT

public:
    explicit IResourceCarryingPlanDialog(QWidget *parent = nullptr);
    ~IResourceCarryingPlanDialog();

    void Init();

    void CreateCarryingTable(map<int, CResourceCarryingVec> carryingInfo);

private slots:
    void on_pbtnSave_clicked();
    void on_pbtnClose_clicked();

    void on_AllPlanTable_customContextMenuRequested(const QPoint &pos);

    void on_Menu_Add();
    void on_Menu_Edit();
    void on_Menu_Remove();
public:

    map<int, CResourceCarryingVec> m_CarryingPlatInfoVec;

private:
    QMenu m_menu;
    QTableWidget  *m_currentWidget;

    ISAddCarryingPlanResDialog *m_pAddCarryingPlanResDialog;
private:
    Ui::IResourceCarryingPlanDialog *ui;
};

#endif
