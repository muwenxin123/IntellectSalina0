#ifndef CDLGMISSIONPLATFORMEDIT_H
#define CDLGMISSIONPLATFORMEDIT_H

#include <QWidget>
#include "QMenu"
#include "LyDialog.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/NebulaTask/NebulaMissionPlatform.h"
#include "DataManage/IMarkManageService.h"
#include "DataManage/IFormationManageService.h"

namespace Ui
{
class CDlgMissionPlatformEdit;
}

class CDlgMissionPlatformEdit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMissionPlatformEdit(QWidget *parent = nullptr);
    ~CDlgMissionPlatformEdit();

private:
    Ui::CDlgMissionPlatformEdit *ui;

    int														m_DataType;

    CNebulaMissionPlatform									m_stMissionPlatform;
    SFormList												m_vFormList_ALL;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::MarkManage::IMarkManageService			*m_pIMarkManageService = nullptr;
    qnzkna::UAVFormManage::IFormationManageService			*m_pIFormationManageService = nullptr;

public:
    void InitData();
    void InitWindowCtrl();
    void SetWindowInfo(CNebulaMissionPlatform &stInfo);
    void ShowTop();
    void SetDataType(int nType);
    void InitTimeEdit();
    void UpdateMarkCombobox();
    void UpdateMissionCombobox();

    void UpdateFormCombobox();

    bool readFormsFromFile();

private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void on_comboBox_12_currentIndexChanged(const QString &arg1);

    void on_comboBox_10_currentIndexChanged(const QString &arg1);

    void on_comboBox_3_currentIndexChanged(const QString &arg1);
};

#endif
