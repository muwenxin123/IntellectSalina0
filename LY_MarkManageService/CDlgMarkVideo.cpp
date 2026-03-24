#include "CDlgMarkVideo.h"
#include "ui_CDlgMarkVideo.h"
#include "QCString.h"
#include "IMarkManageImpl.h"

CDlgMarkVideo::CDlgMarkVideo(QWidget *parent) :
    LyDialog(tr("MarkVideo"), parent),
    ui(new Ui::CDlgMarkVideo)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 600);
}

CDlgMarkVideo::~CDlgMarkVideo()
{
    delete ui;
}

void CDlgMarkVideo::SetPicNameVec(const PictureNameVec &NameVec)
{
    m_PictureNameVec = NameVec;
    m_nIndex = 0;
}

void CDlgMarkVideo::showEvent(QShowEvent *event)
{

}
