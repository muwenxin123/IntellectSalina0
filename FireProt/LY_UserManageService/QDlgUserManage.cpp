#include "QDlgUserManage.h"
#include "ui_QDlgUserManage.h"
#include "TinyXML/tinyxml.h"
#include <QDateTime>
#include <QTime>
#include <QFile>
#include "Common/CommonString.h"
#include <QCString.h>
#include <QDebug>
#include <LyMessageBox.h>

QDlgUserManage::QDlgUserManage(QWidget *parent) :
    LyDialog("", parent),
    ui(new Ui::QDlgUserManage)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->tabWidget);

    this->setResizable(false);
    this->setDragable(true);

    m_pUserModify = nullptr;
    IsClosed = false;
    m_pUserModify = new QDlgUserCreateModify;

    connect(m_pUserModify, &QDlgUserCreateModify::UpdateUserService, this, &QDlgUserManage::UpdateUI);

    m_strCurrSelectUserName = "";
    m_strCurUserName = "";
    m_strError = "";
    m_mapUserInfoList.clear();
    m_mapUserTypeList.clear();
    pUserModel = new QStandardItemModel;
    pTypeModel = new QStandardItemModel;
    ui->lvUserList->setModel(pUserModel);
    ui->lvMemberList->setModel(pTypeModel);
    InitUI();

}

QDlgUserManage::~QDlgUserManage()
{

    delete ui;
}

void QDlgUserManage::InitUI()
{

    ui->ledtUserName->setEnabled(false);
    ui->ledtUserType->setEnabled(false);
    ui->ledtTypeName->setEnabled(false);
    for (auto itor : m_pUserModify->m_puserService->m_mapUserInfoList)
    {
        if (itor.second.GetUserName() == "Root")
        {
            continue;
        }
        QStandardItem *item = new QStandardItem(itor.second.GetUserName().c_str());
        pUserModel->appendRow(item);
    }
    for (auto itor : m_pUserModify->m_puserService->m_mapUserTypeList)
    {
        QStandardItem *item = new QStandardItem(itor.second.GetUserType().c_str());
        pTypeModel->appendRow(item);
    }

}

void QDlgUserManage::UpdateUI()
{

    pUserModel->clear();
    for (auto itor : m_pUserModify->m_puserService->m_mapUserInfoList)
    {
        qDebug() << itor.second.GetUserName().c_str();
        if (itor.second.GetUserName() == "Root")
        {
            continue;
        }
        QStandardItem *item = new QStandardItem(itor.second.GetUserName().c_str());
        pUserModel->appendRow(item);
    }
    pTypeModel->clear();
    for (auto itor : m_pUserModify->m_puserService->m_mapUserTypeList)
    {
        QStandardItem *item = new QStandardItem(itor.second.GetUserType().c_str());
        pTypeModel->appendRow(item);
    }
}

void QDlgUserManage::on_pbtnAddMenber_clicked()
{
    ui->ledtTypeName->setEnabled(true);
    ui->cbxCMD->setEnabled(true);
    ui->cbxCombat->setEnabled(true);
    ui->cbxCommunicate->setEnabled(true);
    ui->cbxCooperate->setEnabled(true);
    ui->cbxIntelligence->setEnabled(true);
    ui->cbxSupport->setEnabled(true);
    ui->ledtUserType->setText("");
}

void QDlgUserManage::on_pbtnAdd_clicked()
{

    if (!m_pUserModify || m_pUserModify->isVisible())
    {

        m_pUserModify->hide();
    }
    m_pUserModify->UpdateUser("");
    m_pUserModify->exec();

}

void QDlgUserManage::on_pbtnClose_clicked()
{
    IsClosed = true;
    emit closeDlg();
    this->close();

}

void QDlgUserManage::on_pbtnCancel2_clicked()
{
    IsClosed = true;
    emit closeDlg();
    this->close();
}

void QDlgUserManage::on_pbtClose_clicked()
{
    IsClosed = true;
    emit closeDlg();
    this->close();
}

void QDlgUserManage::on_pbtnApply_clicked()
{
    if (m_strCurrSelectUserName == "" || m_strCurrSelectUserName.isEmpty())
    {
        return;
    }
    auto itor = m_pUserModify->m_puserService->m_mapUserInfoList.find(m_strCurrSelectUserName.toStdString());
    if (itor != m_pUserModify->m_puserService->m_mapUserInfoList.end())
    {

        itor->second.SetUserDescribe(ui->tedtUserDescribe->placeholderText().toStdString());
        if (ui->ckbCMD->checkState())
        {
            itor->second.SetProject(ui->ckbCMD->text().toStdString());
        }
        if (ui->cbxIntelligence->checkState())
        {
            itor->second.SetProject(ui->cbxIntelligence->text().toStdString());
        }
        if (ui->ckbCooperate->checkState())
        {
            itor->second.SetProject(ui->ckbCooperate->text().toStdString());
        }
        if (ui->ckbCombat->checkState())
        {
            itor->second.SetProject(ui->ckbCombat->text().toStdString());
        }
        if (ui->ckbCommunicate->checkState())
        {
            itor->second.SetProject(ui->ckbCommunicate->text().toStdString());
        }
        if (ui->ckbSupport->checkState())
        {
            itor->second.SetProject(ui->ckbSupport->text().toStdString());
        }
        m_pUserModify->m_puserService->WriteXML("../data/UserInfoFile.dat");

    }

    ui->ledtUserName->setEnabled(false);
    ui->ledtUserType->setEnabled(false);

}

bool QDlgUserManage::GetIsClosed()
{
    return IsClosed;
}

void QDlgUserManage::showEvent(QShowEvent *event)
{

}

void QDlgUserManage::closeEvent(QCloseEvent *event)
{

}

void QDlgUserManage::on_pbtnModify_clicked()
{
    if (m_strCurrSelectUserName == "")
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select a user"));
        return;
    }
    if (!m_pUserModify || m_pUserModify->isVisible())
    {

        m_pUserModify->hide();
    }
    m_pUserModify->UpdateUser(m_strCurrSelectUserName);
    m_pUserModify->exec();
}

void QDlgUserManage::on_lvUserList_clicked(const QModelIndex &index)
{
    m_strCurrSelectUserName = index.data().toString();
    auto userinfo = m_pUserModify->m_puserService->GetUserInfo(m_strCurrSelectUserName.toStdString());
    if (!userinfo)
    {
        return;
    }
    ui->ledtUserName->setText(QString::fromStdString(userinfo->GetUserName().c_str()));
    auto strType = QString::fromStdString(userinfo->GetUserType().c_str());
    qDebug() << "123" << strType;
    ui->ledtUserType->setText(tr2("admin"));

}

void QDlgUserManage::on_pbtDelete_clicked()
{
    if (m_strCurrSelectUserName == "")
    {
        return;
    }
    auto userinfo = m_pUserModify->m_puserService->GetUserInfo(m_strCurrSelectUserName.toStdString());
    m_pUserModify->m_puserService->Remove(m_strCurrSelectUserName.toStdString());
    UpdateUI();
}

void QDlgUserManage::on_pbtnApply2_clicked()
{
    if (ui->ledtTypeName->isEnabled())
    {
        CUserType m_usertype;
        m_usertype.SetUserType(ui->ledtTypeName->text().toStdString());
        qDebug() << "ui->ledtTypeName->text().toStdString()" << ui->ledtTypeName->text();
        if (ui->ckbCMD->checkState())
        {
            m_usertype.SetProject(ui->ckbCMD->text().toStdString());
        }
        if (ui->cbxIntelligence->checkState())
        {
            m_usertype.SetProject(ui->cbxIntelligence->text().toStdString());
        }
        if (ui->ckbCooperate->checkState())
        {
            m_usertype.SetProject(ui->ckbCooperate->text().toStdString());
        }
        if (ui->ckbCombat->checkState())
        {
            m_usertype.SetProject(ui->ckbCombat->text().toStdString());
        }
        if (ui->ckbCommunicate->checkState())
        {
            m_usertype.SetProject(ui->ckbCommunicate->text().toStdString());
        }
        if (ui->ckbSupport->checkState())
        {
            m_usertype.SetProject(ui->ckbSupport->text().toStdString());
        }

        m_pUserModify->m_puserService->m_mapUserTypeList.insert(std::make_pair(m_usertype.GetUserType(), m_usertype));
        qDebug() << "m_mapUserTypeList" << m_pUserModify->m_puserService->m_mapUserTypeList.size();
        ui->ledtTypeName->setEnabled(false);
        m_pUserModify->m_puserService->WriteUserTypeXML("../data/UserType.xml");
        UpdateUI();
    }
    else
    {
        if (m_strCurrSelectType == "" || m_strCurrSelectType.isEmpty())
        {
            return;
        }
        auto itor = m_pUserModify->m_puserService->m_mapUserTypeList.find(m_strCurrSelectType.toStdString());
        if (itor != m_pUserModify->m_puserService->m_mapUserTypeList.end())
        {
            itor->second.GetProjectVec()->clear();
            qDebug() << "ui->ckbCMD->text().toStdString()" << ui->ckbCMD->text();
            if (ui->ckbCMD->checkState())
            {
                itor->second.SetProject(ui->ckbCMD->text().toStdString());
            }
            if (ui->cbxIntelligence->checkState())
            {
                itor->second.SetProject(ui->cbxIntelligence->text().toStdString());
            }
            if (ui->ckbCooperate->checkState())
            {
                itor->second.SetProject(ui->ckbCooperate->text().toStdString());
            }
            if (ui->ckbCombat->checkState())
            {
                itor->second.SetProject(ui->ckbCombat->text().toStdString());
            }
            if (ui->ckbCommunicate->checkState())
            {
                itor->second.SetProject(ui->ckbCommunicate->text().toStdString());
            }
            if (ui->ckbSupport->checkState())
            {
                itor->second.SetProject(ui->ckbSupport->text().toStdString());
            }
            qDebug() << "itor->second" << itor->second.GetProjectVec()->size();
            m_pUserModify->m_puserService->WriteUserTypeXML("../data/UserType.xml");
            UpdateUI();
        }
    }

}

void QDlgUserManage::on_lvMemberList_clicked(const QModelIndex &index)
{

    m_strCurrSelectType = index.data().toString();
    ui->ledtTypeName->setText(index.data().toString());
    ui->cbxCMD->setCheckState(Qt::Unchecked);
    ui->cbxIntelligence->setChecked(false);
    ui->cbxCooperate->setChecked(false);
    ui->cbxCombat->setChecked(false);
    ui->cbxCommunicate->setChecked(false);
    ui->cbxSupport->setChecked(false);

    qDebug() << "m_strCurrSelectType" << index.data().toString() << m_pUserModify->m_puserService->m_mapUserTypeList.size();
    auto itor = m_pUserModify->m_puserService->m_mapUserTypeList.find(m_strCurrSelectType.toStdString());
    if (itor != m_pUserModify->m_puserService->m_mapUserTypeList.end())
    {
        StringVector *typeVec = itor->second.GetProjectVec();
        for (auto mitm : *typeVec)
        {
            if (QString::fromStdString(mitm.c_str()) == tr2("通信分系统"))
            {
                ui->cbxCommunicate->setChecked(true);
            }
            if (QString::fromStdString(mitm.c_str()) == tr2("保障分系统"))
            {
                ui->cbxSupport->setChecked(true);
            }
        }
    }
}

void QDlgUserManage::on_pbtnDeleteMember_clicked()
{
    if (m_strCurrSelectType == "" || m_strCurrSelectType.isEmpty())
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select a user type"));
        return;
    }
    auto itor = m_pUserModify->m_puserService->m_mapUserTypeList.find(m_strCurrSelectType.toStdString());
    if (itor != m_pUserModify->m_puserService->m_mapUserTypeList.end())
    {
        m_pUserModify->m_puserService->m_mapUserTypeList.erase(itor);
        m_pUserModify->m_puserService->WriteUserTypeXML("../data/UserType.xml");
        UpdateUI();
    }
}
