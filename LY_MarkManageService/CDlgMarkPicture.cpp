#include "CDlgMarkPicture.h"
#include "ui_CDlgMarkPicture.h"
#include "QCString.h"
#include "IMarkManageImpl.h"

CDlgMarkPicture::CDlgMarkPicture(QWidget *parent) :
    LyDialog(tr("MarkPicture"), parent),
    ui(new Ui::CDlgMarkPicture)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 600);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
}

CDlgMarkPicture::~CDlgMarkPicture()
{
    delete ui;
}

void CDlgMarkPicture::SetPicNameVec(const PictureNameVec &NameVec)
{
    m_PictureNameVec = NameVec;
    m_nIndex = 0;
}

void CDlgMarkPicture::ShowPicture(const int &nIndex)
{
    if (nIndex >= m_PictureNameVec.size())
    {
        return;
    }

    auto itr = m_PictureNameVec.begin();
    itr += nIndex;

    QPixmap qpic;
    bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().GetMarkImageData(*itr, &qpic);

    if (bresut == false)
    {
        return;
    }

    ui->label->clear();
    int nWidth = ui->label->width();
    int nHeight = ui->label->height();
    QPixmap fitpixmap = qpic.scaled(nWidth, nHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label->setPixmap(fitpixmap);
}

void CDlgMarkPicture::showEvent(QShowEvent *event)
{
    if (m_PictureNameVec.size() > 1)
    {
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
    }
    else
    {
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
    }

    ShowPicture(m_nIndex);
}

void CDlgMarkPicture::on_pushButton_2_clicked()
{
    m_nIndex++;
    if (m_nIndex >= m_PictureNameVec.size())
    {
        m_nIndex = 0;
    }
    ShowPicture(m_nIndex);
}

void CDlgMarkPicture::on_pushButton_clicked()
{
    m_nIndex--;
    if (m_nIndex < 0)
    {
        m_nIndex = m_PictureNameVec.size() - 1;
    }
    ShowPicture(m_nIndex);
}
