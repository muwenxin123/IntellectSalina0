#ifndef QDLGUSERLOGIN_H
#define QDLGUSERLOGIN_H

#include <LyDialog.h>
#include <QDialog>
#include "QDlgUserManage.h"

namespace Ui
{
class QDlgUserLogin;
}

class CLoginRecord
{
public:
    CLoginRecord() {};
    CLoginRecord(const CLoginRecord &data)
    {
        *this = data;
    };
    CLoginRecord(const std::string &strName, const std::string &strPwd)
    {
        m_strLoginName = strName;
        m_strLoginPwd = strPwd;
    };
    virtual ~CLoginRecord() {};
public:
    void SetLoginName(const std::string &strName)
    {
        m_strLoginName = strName;
    };
    void SetLoginPassword(const std::string &strPwd)
    {
        m_strLoginPwd = strPwd;
    };
    void SetLoginProject(const std::string &strProject)
    {
        m_strLoginProject = strProject;
    };
    std::string GetLoginName()	const
    {
        return m_strLoginName;
    };
    std::string GetLoginPassword()	const
    {
        return m_strLoginPwd;
    };
    std::string GetLoginProject()	const
    {
        return m_strLoginProject;
    };
private:
    std::string m_strLoginName;
    std::string m_strLoginPwd;
    std::string m_strLoginProject;
public:
    CLoginRecord &operator=(const CLoginRecord &data)
    {
        if (&data == this)
        {
            return *this;
        }
        m_strLoginName		= data.m_strLoginName;
        m_strLoginPwd		= data.m_strLoginPwd;
        m_strLoginProject	= data.m_strLoginProject;
        return *this;
    }
};

typedef std::vector<CLoginRecord>	CLoginRecordVector;
typedef CLoginRecordVector::iterator CLoginRecordVectorItr;

class QDlgUserLogin : public QDialog
{
    Q_OBJECT

public:
    explicit QDlgUserLogin(QWidget *parent = nullptr);
    ~QDlgUserLogin();

    void InitUI();
    bool LoadLoginFile(const std::string& strLoginFile="../data/Login.xml");
    bool SaveLoginFile(const std::string& strLoginFile="../data/Login.xml");

    void keyPressEvent(QKeyEvent *event);
private slots:
    void on_pbtnCancel_clicked();

    void on_pbtnLogin_clicked();

    void on_pbtnUser_2_clicked();

private:
    void on_cbxUserSeat_itemChanged(int index);

public:
    bool IsPwdCorrect();

    int getUserInfo() const;
    QDlgUserCreateModify   *m_pUserModify;
    int m_LoadUserID;
protected:
    CLoginRecordVector	m_vecLoginRecord;

private:
    Ui::QDlgUserLogin *ui;
    CLoginRecord m_userInfo;
    bool m_bisPWDCorrect;
    bool bEncrypt = false;
};

#endif