#pragma once

#include <QWidget>
#include "ui_CDlgMarkMaintain.h"
#include"LyDialog.h"
#include <QMenu>
#include "ITaskManagerActivator.h"
using namespace std;
class CDlgMarkMaintain : public LyDialog
{
    Q_OBJECT

public:
    CDlgMarkMaintain(QWidget *parent = Q_NULLPTR);
    ~CDlgMarkMaintain();

    void Init();

    void RecordInformation();
    void CreateTableData(vector<int> row);
private slots:
    void on_btnAdd_clicked();
    void on_btnEdit_clicked();
    void on_btnDele_clicked();
    void on_tbwMark_customContextMenuRequested(const QPoint &pos);

    void on_Menu_Ammo_Add();
    void on_Menu_Ammo_Edit();
    void on_Menu_Ammo_Delete();

    void on_leSearch_textChanged();
private:
    QMenu						*m_twAmmoMenu;

    CSystemMarkInfoVec		m_MarkListVec;

    vector<pair<int, QString>>	RecordDataVec;
    Ui::CDlgMarkMaintain *ui;
};
