#include "cmdPlane.h"
#include "ui_cmdPlane.h"
#include "track.h"
#include "lyXyhcInterface.h"
#include "qIIValidator.h"

CmdPlane::CmdPlane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CmdPlane)
{
    ui->setupUi(this);
    ui->lineEditID->setValidator(new QIIValidator(0, 255));
}

CmdPlane::~CmdPlane()
{
    delete ui;
}

void CmdPlane::on_pushButtonSend_clicked()
{
    int nZ, nP;
    sscanf(ui->lineEditID->text().toLatin1().data(), "%d.%d", &nZ, &nP);

    char szT[256] = {0};

    int nLength = sizeof(NetHeader) + sizeof(TrackCommon) + sizeof(qnzkna::xyhcStruct::modelSet);
    TrackInfo *pTi = (TrackInfo *)szT;
    pTi->header.ucType = 1;
    pTi->header.ucSubType = 1;
    pTi->header.unLength = nLength;
    pTi->cmn.Z = nZ;
    pTi->cmn.P = nP;

    LyXyhcInterface::Get().SendData(szT, nLength);

}
