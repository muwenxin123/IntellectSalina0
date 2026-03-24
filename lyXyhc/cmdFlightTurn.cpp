#include "cmdFlightTurn.h"
#include "ui_cmdFlightTurn.h"
#include "track.h"
#include "lyXyhcInterface.h"
#include "qIIValidator.h"

CmdFlightTurn::CmdFlightTurn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CmdFlightTurn)
{
    ui->setupUi(this);
    ui->lineEditID->setValidator(new QIIValidator(0, 255));
}

CmdFlightTurn::~CmdFlightTurn()
{
    delete ui;
}

void CmdFlightTurn::on_pushButtonSend_clicked()
{
    int nZ, nP;
    sscanf(ui->lineEditID->text().toLatin1().data(), "%d.%d", &nZ, &nP);
    char szT[256] = {0};
    int nLength = sizeof(NetHeader) + sizeof(TrackCommon) + sizeof(qnzkna::xyhcStruct::flyTurnControlSet);
    TrackInfo *pTi = (TrackInfo *)szT;
    pTi->header.ucType = 1;
    pTi->header.ucSubType = 2;
    pTi->header.unLength = nLength;
    pTi->cmn.Z = nZ;
    pTi->cmn.P = nP;

    LyXyhcInterface::Get().SendData(szT, nLength);
}
