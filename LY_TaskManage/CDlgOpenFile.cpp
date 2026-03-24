#include "CDlgOpenFile.h"
#include "ui_CDlgOpenFile.h"
#include "ITaskManagerActivator.h"
#include "LyMessageBox.h"
#include "QCString.h"
#include "lY_TaskManage.h"

CDlgOpenFile::CDlgOpenFile(QWidget *parent) :
    LyDialog(tr("OpenFile"), parent),
    ui(new Ui::CDlgOpenFile)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(320, 500);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));

    ui->checkBox->setChecked(false);
    ui->checkBox->setVisible(false);
    ui->checkBox->hide();
}

CDlgOpenFile::~CDlgOpenFile()
{
    delete ui;
}

void CDlgOpenFile::changeEvent(QEvent *event)
{
    LyDialog::changeEvent(event);
    switch (event->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CDlgOpenFile::on_pushButton_clicked()
{
    QListWidgetItem *Item = ui->listWidget->currentItem();
    if (Item == nullptr)
    {
        LyMessageBox::information(NULL, tr("Tips"), tr("Please select one to open!"));

        return;
    }
    m_strSelFileName = Item->text();
    if (ui->checkBox->checkState())
    {
        LY_TaskManage::Get().GetDlgPlay()->SetPlayStates(true);
    }
    else
    {
        LY_TaskManage::Get().GetDlgPlay()->SetPlayStates(false);
    }
    this->hide();

    emit openpushButton();
}

void CDlgOpenFile::on_pushButton_2_clicked()
{
    QListWidgetItem *Item = ui->listWidget->currentItem();
    if (Item == nullptr)
    {
        LyMessageBox::information(NULL, tr("Tips"), tr("Please select one to remove!"));

        return;
    }

    QString strDeleteName = Item->text();

    qnzkna::TaskManage::INebulaTaskManageService *m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    int result = m_pITaskManageService->RemoveNebulaTaskFile(strDeleteName.toStdString().c_str());
    switch (result)
    {
    case qnzkna::TaskManage::INebulaTaskManageService::EOperatorNebulaTask_Succeed:
        LyMessageBox::information(NULL, tr("Tips"), tr("Remove success!"));

        break;
    case qnzkna::TaskManage::INebulaTaskManageService::EOperatorNebulaTask_ErrNoFile:
        LyMessageBox::information(NULL, tr("Tips"), tr("File do not exist,remove failed"));

        break;
    case qnzkna::TaskManage::INebulaTaskManageService::EOperatorNebulaTask_ErrDelFile:
        LyMessageBox::information(NULL, tr("Tips"), tr("Remove failed!"));

        break;
    default:
        break;
    }

    UpdateListData();
}

void CDlgOpenFile::on_pushButton_3_clicked()
{
    this->hide();
}

void CDlgOpenFile::showEvent(QShowEvent *event)
{
    UpdateListData();
}

void CDlgOpenFile::UpdateListData()
{
    m_strSelFileName = "";
    ui->listWidget->clear();
    qnzkna::TaskManage::INebulaTaskManageService *m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    nameList = m_pITaskManageService->GetAllNebulaTaskFileName();
    qnzkna::TaskManage::INebulaTaskManageService::NebulaTaskFileNameList::iterator itrStrSet = nameList.begin();
    for (; itrStrSet != nameList.end(); ++itrStrSet)
    {
        std::string strFileName = *itrStrSet;

        long nLength = strFileName.length();
        std::string strName = strFileName.substr(0, nLength - 4);

        ui->listWidget->addItem(QString::fromStdString(strName));
    }
}

QString CDlgOpenFile::GetOpenFileName()
{
    return m_strSelFileName;
}

bool CDlgOpenFile::OpenFile(QString strFileName)
{
    return true;
}
