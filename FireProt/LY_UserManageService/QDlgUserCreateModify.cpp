#include "QDlgUserCreateModify.h"
#include "ui_QDlgUserCreateModify.h"
#include <QCString.h>

#include <QCString.h>
#include "LyMessageBox.h"

QDlgUserCreateModify::QDlgUserCreateModify(QWidget *parent) :
    LyDialog("", parent),
    ui(new Ui::QDlgUserCreateModify)
{

    this->setResizable(false);
    this->setDragable(true);
    ui->setupUi(new QDialog(centralWidget()));
    QMetaObject::connectSlotsByName(this);
    m_puserService = new UserManageService;
    m_strModifyType = "";
    InitUI();
}

QDlgUserCreateModify::~QDlgUserCreateModify()
{
    delete ui;
}

void QDlgUserCreateModify::InitUI()
{
    int index = 0;
    for (auto userType : m_puserService->m_mapUserTypeList)
    {
        ui->cbxUserType->insertItem(index, userType.second.GetUserType().c_str());
        index++;
    }

}

void QDlgUserCreateModify::UpdateUser(QString strType)
{
    ui->ledtConfirmPassword->setText("");
    ui->ledtNewPassword->setText("");
    ui->ledtOldPassword->setText("");
    if ("" == strType)
    {

        ui->ledtUserName->setText("");
        ui->ledtUserName->setToolTip(tr("New User"));
        ui->ledtOldPassword->setEnabled(false);
        m_strModifyType = "";
    }
    else
    {

        ui->ledtUserName->setText(strType);
        auto userinfo = m_puserService->GetUserInfo(strType.toStdString());
        if (userinfo != nullptr)
        {
            ui->lineEditUserID->setText(userinfo->GetUserID().c_str());
        }

        m_strModifyType = strType;
        ui->ledtOldPassword->setEnabled(true);

    }
}

void QDlgUserCreateModify::on_pbtnCancel_clicked()
{
    close();
}

void QDlgUserCreateModify::on_pbtnOK_clicked()
{
    if (m_strModifyType == "")
    {

        if (ui->lbUserName->text() == "")
        {
            LyMessageBox::information(NULL, tr("tip"), tr("The user name cannot be empty"));
            return;
        }

        if (ui->ledtNewPassword->text() == "" || ui->ledtConfirmPassword->text() == "")
        {
            LyMessageBox::information(NULL, tr("tip"), tr("The password cannot be empty"));
            return;
        }

        if (ui->ledtNewPassword->text() != ui->ledtConfirmPassword->text())
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Confirm that the password are inconsistent"));
            return;
        }

        CUserInfo userInfo(ui->ledtUserName->text().toStdString(), ui->ledtConfirmPassword->text().toStdString(), ui->lineEditUserID->text().toStdString());
        userInfo.SetUserName(ui->ledtUserName->text().toStdString());
        userInfo.SetUserPassword(ui->ledtConfirmPassword->text().toStdString());

        userInfo.SetUserManagement("admin");
        userInfo.SetUserType(ui->cbxUserType->currentText().toStdString());

        m_puserService->Register(userInfo);
    }
    else
    {
        auto userCurr = m_puserService->m_mapUserInfoList.find(m_strModifyType.toStdString());
        if (userCurr != m_puserService->m_mapUserInfoList.end())
        {
            userCurr->second.SetUserType(ui->cbxUserType->currentText().toStdString());
            userCurr->second.SetUserID(ui->lineEditUserID->text().toStdString());
            userCurr->second.SetUserManagement("admin");
            userCurr->second.SetUserPassword(ui->ledtConfirmPassword->text().toStdString());
        }
        m_puserService->WriteXML("../data/UserInfoFile.dat");
    }

    emit UpdateUserService();
    this->close();
}
