#ifndef CDLGMISSIONPLATFORMLOADEDIT_H
#define CDLGMISSIONPLATFORMLOADEDIT_H

#include <QWidget>
#include "LyDialog.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IMarkManageService.h"
#include "DataManage/NebulaTask/NebulaMissionPlatformLoad.h"
#include "DataManage/Common/TypeDefine_Node.h"

namespace Ui
{
class CDlgMissionPlatformLoadEdit;
}

class CDlgMissionPlatformLoadEdit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMissionPlatformLoadEdit(QWidget *parent = nullptr);
    ~CDlgMissionPlatformLoadEdit();

private:
    Ui::CDlgMissionPlatformLoadEdit *ui;

    int														m_DataType;

    CNebulaMissionPlatformLoad								m_stMissionPlatformLoad;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::MarkManage::IMarkManageService			*m_pIMarkManageService = nullptr;

    qnzkna::NodeManage::CNodeRelationInfoPtrVector						 m_vecNodeInfo;

public:
    void InitData();
    void InitWindowCtrl();
    void SetWindowInfo(CNebulaMissionPlatformLoad &stInfo);
    void ShowTop();
    void SetDataType(int nType);
    void InitTimeEdit();

    void UpdateMissionCombobox();

    void SetTypeControl(bool bStates);
private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_9_currentIndexChanged(const QString &arg1);

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void on_comboBox_12_currentIndexChanged(const QString &arg1);

    void on_comboBox_14_currentIndexChanged(const QString &arg1);

    void on_comboBox_3_currentIndexChanged(const QString &arg1);
};

#endif
