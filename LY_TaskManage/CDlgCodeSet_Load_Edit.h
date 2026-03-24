#ifndef CDLGCODESET_LOAD_EDIT_H
#define CDLGCODESET_LOAD_EDIT_H

#include <QWidget>
#include "LyDialog.h"
#include "DataManage/INebulaTaskManageService.h"
#include "CDlgCodeSet_Load_Edit_Child.h"

namespace Ui
{
class CDlgCodeSet_Load_Edit;
}
class QMenu;

class CDlgCodeSet_Load_Edit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgCodeSet_Load_Edit(QWidget *parent = nullptr);
    ~CDlgCodeSet_Load_Edit();

private:
    Ui::CDlgCodeSet_Load_Edit *ui;

private:
    qnzkna::TaskManage::INebulaTaskManageService			            *m_pITaskManageService;

    CCodeSet_Load_DB_Vec												m_CodeSet_Load_DB_Vec;
    CCodeSet_Load_DB													m_CodeSet_Load_DB;
    CodeSetLoad															m_CodeSetLoad;

    QMenu																*m_TableMenu;
    QAction																*m_MenuAdd;
    QAction																*m_MenuEdit;
    QAction																*m_MenuRemove;
    CDlgCodeSet_Load_Edit_Child											*pChild = nullptr ;

public:
    void InitData();

    void TabWidgetConnet();
    void CreateTableMenu();

    void CreateListHeading();
    void UpdataWindowData();
    void UpdateListData();
    void UpdateSelectedList();

    void UpdateTaskCombobox();
    void UpdatePlatformCombobox();
    void UpdateLineCombobox();

    void SetShowData(const CCodeSet_Load_DB stCodeSetDB);

    int CreateCodeSetNo();

    void UpdateCodeSetWidgets();
private:
    void showEvent(QShowEvent *event);

private slots:

    void on_Menu_Add();

    void on_Menu_Edit();

    void on_Menu_Remove();

    void customContextMenuRequested(const QPoint &pos);

    void on_comboBox_3_currentIndexChanged(int index);

    void on_comboBox_currentIndexChanged(int index);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void EditFinish();

};

#endif
