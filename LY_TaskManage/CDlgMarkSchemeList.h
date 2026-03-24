#ifndef CDLGMARKSCHEMELIST_H
#define CDLGMARKSCHEMELIST_H
#include <QWidget>
#include "LyDialog.h"
#include "ui_CDlgMarkSchemeList.h"
#include "ui_CDlgMarkSchemeList.h"
#include "ITaskManagerActivator.h"

class CDlgMarkSchemeList : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMarkSchemeList(QWidget *parent = nullptr);
    ~CDlgMarkSchemeList();

    void Init();

    void GetMarkSchemeData();

    void CreateTableWidget();
public slots:
    void on_tbwMarkScheme_itemDoubleClicked(QTableWidgetItem *item);

    void on_btnOk_Clicked();

    void on_btnCancel_Clicked();

private:
    Ui::CDlgMarkSchemeList *ui;
    qnzkna::TaskManage::INebulaTaskManageService							*m_pITaskManageService = nullptr;
    CNebulaSchemeVec                                                        m_cNebulaSchemeVec;
    std::string                                                             m_sMarkID;
};
#endif
