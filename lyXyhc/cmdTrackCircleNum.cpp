#include "cmdTrackCircleNum.h"
#include "ui_cmdTrackCircleNum.h"
#include "track.h"
#include "lyXyhcInterface.h"
#include "qIIValidator.h"

CmdTrackCircleNum::CmdTrackCircleNum(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CmdTrackCircleNum)
{
    ui->setupUi(this);
    ui->lineEditID->setValidator(new QIIValidator(0, 255));
}

CmdTrackCircleNum::~CmdTrackCircleNum()
{
    delete ui;
}

void CmdTrackCircleNum::on_pushButtonSend_clicked()
{
    int nZ = 0, nP = 0;
    sscanf(ui->lineEditID->text().toLatin1().data(), "%d.%d", &nZ, &nP);
    char szT[256] = {0};
    int nLength = sizeof(NetHeader) + sizeof(TrackCommon) + sizeof(qnzkna::xyhcStruct::cruiseCircleNumber);
    TrackInfo *pTi = (TrackInfo *)szT;
    pTi->header.ucType		= 1;
    pTi->header.ucSubType	= 21;
    pTi->header.unLength	= nLength;
    pTi->cmn.Z = nZ;
    pTi->cmn.P = nP;

    LyXyhcInterface::Get().SendData(szT, nLength);
}
