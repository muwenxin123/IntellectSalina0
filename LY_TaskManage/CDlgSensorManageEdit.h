#ifndef CDLGSENSORMANAGEEDIT_H
#define CDLGSENSORMANAGEEDIT_H

#include <QWidget>
#include "LyDialog.h"
#include "QMap"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/NebulaTask/NebulaMission.h"

namespace Ui
{
class CDlgSensorManageEdit;
}

class CDlgSensorManageEdit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgSensorManageEdit(QWidget *parent = nullptr);
    ~CDlgSensorManageEdit();

private slots:

    void on_checkBox_Load_stateChanged(int arg1);

    void on_comboBox_currentIndexChanged(int index);

    void on_comboBox_3_currentIndexChanged(int index);

    void on_comboBox_4_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

public:
    void InitLoadCheckBox();
    void ConnectCheckBox();
    void DisConnectCheckBox();

    void UpdateLinePointCombobox();
    void InitLoadCombobox();

    void ConnectControl();

    void SetCodeSetData(const CCodeSet_Load_DB_Vec CodeSetVec);
    void SetTaskID(const std::string strID);
    void SetPlatformID(const std::string strID);
    void SetLoadID(const std::string strID);
    void SetLineID(const int nLineID);
    void SetPointID(const int nID);
    void SetMarkIDList(const QStringList strList);

    void UpdateListData();
    void UpdateCheckBoxData();
    void CalculateMark();

    void SavePodData();
private:
    Ui::CDlgSensorManageEdit *ui;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;

    QMap<QCheckBox *, int>									m_pQCheckBoxMap;
    QMap<QCheckBox *, QString>								m_pQCheckBoxMarkMap;

    CCodeSet_Load_DB_Vec									m_CodeSet_Load_DB_Vec;
    CCodeSet_Load_DB_Vec									m_CodeSet_Load_DB_Wnd_Vec;
    QStringList												m_MarkList;
    std::string												m_strTaskID;
    std::string												m_strPlatformID;
    std::string												m_strLoadID;
    int														m_LineID;
    int														m_PointID;

};

#endif
