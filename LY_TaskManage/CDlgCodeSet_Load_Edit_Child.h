#ifndef CDLGCODESET_LOAD_EDIT_CHILD_H
#define CDLGCODESET_LOAD_EDIT_CHILD_H

#include <QWidget>
#include "LyDialog.h"
#include "DataManage/INebulaTaskManageService.h"

namespace Ui
{
class CDlgCodeSet_Load_Edit_Child;
}

class CDlgCodeSet_Load_Edit_Child : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgCodeSet_Load_Edit_Child(QWidget *parent = nullptr);
    ~CDlgCodeSet_Load_Edit_Child();

private:
    Ui::CDlgCodeSet_Load_Edit_Child *ui;
private:
    qnzkna::TaskManage::INebulaTaskManageService			            *m_pITaskManageService;

    CCodeSet_Load_DB													m_CodeSet_Load_DB;

public:
    void SetEditData(const CCodeSet_Load_DB stData);
    CCodeSet_Load_DB GetEditData();

    void InitEditCtrl();
    void UpdateEditCtrl();

    void UpdateLoadCombobox();
    void InitCodeCombobox();
    void UpdateCodeCombobox();

private:
    void showEvent(QShowEvent *event);

signals:
    void PushOkButton();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_currentIndexChanged(int index);
};

#endif
