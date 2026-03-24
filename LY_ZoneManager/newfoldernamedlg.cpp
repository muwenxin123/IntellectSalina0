#include "newfoldernamedlg.h"

#include "LY_ZoneManager.h"
#include "QCString.h"

NewFolderNameDlg::NewFolderNameDlg(const QString &WndName, const QString &FileName, int maxname, QWidget *pParent, bool isModal): LyDialog(tr2("新建文件夹"), pParent), ui(new Ui::NewFolderNameDlg())
{
    ui->setupUi((QDialog *)centralWidget());
    m_WndName = WndName;
    m_FileName = FileName;
    m_maxnamelen = maxname;

    setWindowTitle(m_WndName);

    ui->lineEdit->setText(m_FileName);
    connect(ui->toolButton, &QToolButton::clicked, this, &NewFolderNameDlg::OnOK);

    ui->toolButton->setIcon(QIcon(":/data/images/1k/black/icon/Tool/14/ensure_normal.png"));
    ui->toolButton->setText(tr2("确定"));
    ui->toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

NewFolderNameDlg::~NewFolderNameDlg()
{
    delete ui;
}

void NewFolderNameDlg::OnOK()
{
    m_FileName = ui->lineEdit->text().trimmed();

    accept();
}

