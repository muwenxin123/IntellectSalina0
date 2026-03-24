#ifndef QDLGUSERMANAGE_H
#define QDLGUSERMANAGE_H

#include <LyDialog.h>
#include <QDialog>
#include "SystemConfig/UserInfo.h"
#include "SystemConfig/IUserManageService.h"
#include <QMutex>
#include <QStandardItemModel>
#include "QDlgUserCreateModify.h"
#include "UserManageService.h"

namespace Ui
{
class QDlgUserManage;
}

class QDlgUserManage : public LyDialog
{
    Q_OBJECT

public:
    explicit QDlgUserManage(QWidget *parent = nullptr);
    ~QDlgUserManage();

    void InitUI();
signals:
    void closeDlg();

public:

    QDlgUserCreateModify   *m_pUserModify;

private slots:
    void UpdateUI();

private:

    QStandardItemModel     *pUserModel;
    QStandardItemModel     *pTypeModel;
    QMutex                  m_qmutex;
    CUserInfoMap			m_mapUserInfoList;
    CUserTypeMap            m_mapUserTypeList;
    std::string				m_strError;
    std::string				m_strCurUserName;

private slots:

    void on_pbtnAddMenber_clicked();

    void on_pbtnAdd_clicked();

    void on_pbtnClose_clicked();

    void on_pbtnCancel2_clicked();

    void on_pbtClose_clicked();

    void on_pbtnApply_clicked();

    void on_pbtnModify_clicked();

    void on_lvUserList_clicked(const QModelIndex &index);

    void on_pbtDelete_clicked();

    void on_pbtnApply2_clicked();

    void on_lvMemberList_clicked(const QModelIndex &index);

    void on_pbtnDeleteMember_clicked();

public:
    bool GetIsClosed();
private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::QDlgUserManage  *ui;
    bool                IsClosed;
    QString             m_strCurrSelectUserName;
    QString             m_strCurrSelectType;
};

#endif
