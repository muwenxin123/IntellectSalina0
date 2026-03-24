#ifndef CDLGACTIONLIST_H
#define CDLGACTIONLIST_H

#include <QWidget>
#include "LyDialog.h"
#include "QMenu"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/INebulaTaskManageService.h"
#include <QTimer>

namespace Ui
{
class CDlgActionList;
}

class CDlgActionList : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgActionList(QWidget *parent = nullptr);
    ~CDlgActionList();

private:
    Ui::CDlgActionList			*ui;
    QMenu						*m_TableMenu;
    QAction						*m_MenuAdd;
    QAction						*m_MenuEdit;
    QAction						*m_MenuRemove;
    QAction						*m_MenuUpdate;

    CNeubulaActionVec			m_ActionVec;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;

public:

    void TabWidgetConnet();
    void CreateTableMenu();

    void CreateListHeading();

    void UpdataWindowData();
    void UpdateListData();

    CNeubulaActionVec GetActionVec();

    void SetActive(int r, int c);

    void showEvent(QShowEvent *e);

public:
    CNeubulaAction activeAction;
    QTimer *m_Timer;
    bool	bclose = false;
private slots:

    void on_pbtnCancel_clicked();

    void on_Menu_Add();

    void on_Menu_Edit();

    void on_Menu_Remove();

    void on_Menu_Update();
    void on_btnEdit_clicked();
    void on_btnDele_clicked();

    void timerUpdateAction();

    void customContextMenuRequested(const QPoint &pos);

    void tbw_cellDoubleClicked(int r, int c);

    void selectBox(int r, int c);
};

#endif
