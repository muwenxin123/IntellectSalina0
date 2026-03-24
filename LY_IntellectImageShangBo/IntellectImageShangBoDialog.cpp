#include "IntellectImageShangBoDialog.h"
#include "Protocol/XygsZC/Bag_XygsZCDefines.h"
#include "ui_IntellectImageShangBoDialog.h"

#include <QDateTime>
#include <QDebug>
#include <QSettings>
#include <QTextCodec>

#include "QRegExpExample.h"

#include "IIntellectImageShangBoActivator.h"

#define GROUPADDRESS_UDP_MULTICAST "226.0.1.101"

IntellectImageShangBoDialog::IntellectImageShangBoDialog(QWidget *parent) :
      QWidget(parent),
      ui(new Ui::IntellectImageShangBoDialog),
      m_StrSensorId(""),
      m_StrComponentID(""),
      m_ComponentID(0),
      m_Timer(this),
      m_ShangBoPodAutoConnect(false),
      m_ServerAddress("192.168.1.166"),
      m_ServerPort(2000),
      m_LocalPort(2000),
      m_bindPort(false)
{
    ui->setupUi(this);

    ui->groupBox_Control_Track->setVisible(false);
    ui->groupBox_Control_Track->hide();

    ui->lineEdit_Control_ConnectAddress->setValidator(new QRegExpValidator(regIP));
    ui->lineEdit_Control_ConnectPort->setValidator(new QRegExpValidator(regInt));

    ReadShangBoPodConfig();

    ui->lineEdit_Control_ConnectAddress->setText(m_ServerAddress.toString());
    ui->lineEdit_Control_ConnectPort->setText(QString::number(m_ServerPort));
    ui->checkBox_Control_AutoConnect->setChecked(m_ShangBoPodAutoConnect);

    if(m_ShangBoPodAutoConnect){
        ui->pushButton_Control_Connect->setEnabled(false);
        ui->pushButton_Control_DisConnect->setEnabled(true);
        InitWeaponInfoList();
    }

    connect(&m_socket, &QTcpSocket::readyRead, this, [this](){
        while (m_socket.bytesAvailable()) {
            const QByteArray& data = m_socket.readAll();

            if(data.size() <= 0){
                continue ;
            }

            if((unsigned char)(data.at(0)) != SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD ||
                (unsigned char)(data.at(1)) != SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD){
                continue ;
            }

            const unsigned char length = (unsigned char)(data.at(2));
            if(data.size() != (length + 4)){
                continue ;
            }

            const unsigned char verifySum = ShangBoIntellectMsg::CRCCheck_ShangBo((unsigned char*)(data.constData() + 3), length);
            if((unsigned char)(data.at(data.size() - 1)) != verifySum){
                continue ;
            }

            if(length == SHANGBOINTELLECTMSG_SHANGBOCONTROLSTATUSMSG_LENGTH)
            {

                if(length != sizeof(ShangBoIntellectMsg::ShangBoControlStatusMsg)){
                    continue ;
                }

                const ShangBoIntellectMsg::ShangBoControlStatusMsg* const shangBoControlStatusMsg = (ShangBoIntellectMsg::ShangBoControlStatusMsg*)(data.constData() + 3);

                if(shangBoControlStatusMsg->synchro1 != SHANGBOINTELLECTMSG_SHANGBOCONTROLSTATUSMSG_FIRST_HEAD ||
                    shangBoControlStatusMsg->synchro2 != SHANGBOINTELLECTMSG_SHANGBOCONTROLSTATUSMSG_SECOND_HEAD){
                    continue ;
                }

                const unsigned char verifySum2 = ShangBoIntellectMsg::CRCCheck_ShangBo((unsigned char*)(shangBoControlStatusMsg), length - 1);
                if(shangBoControlStatusMsg->verifySum != verifySum2){
                    continue ;
                }

                if(shangBoControlStatusMsg->laserDistance != 0 && (m_ComponentID != 0)){

                    if(IIntellectImageShangBoActivator::Get().getIRegistryImpl() != nullptr)
                    {
                        qnzkna::framework::MessageReferenceProps props;
                        props.put("TargetType",   std::to_string(NODE_TYPE_SENSOR));
                        props.put("TargetID",     m_StrSensorId);
                        props.put("WeaponID",     m_StrComponentID);

                        XygsZC_02 xygsZC_02;

                        xygsZC_02.unNodeType   = NODE_TYPE_SENSOR;
                        xygsZC_02.unNodeID     = m_ComponentID;
                        xygsZC_02.pitch        = shangBoControlStatusMsg->pitch;
                        xygsZC_02.roll         = shangBoControlStatusMsg->roll;
                        xygsZC_02.yaw          = shangBoControlStatusMsg->yaw;
                        xygsZC_02.distance     = shangBoControlStatusMsg->laserDistance;
                        xygsZC_02.fieldWidth   = 0;
                        xygsZC_02.useful       = true;
                        xygsZC_02.newBatch     = false;

                        IIntellectImageShangBoActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgGimbalAttitude", (char*)&xygsZC_02, sizeof(xygsZC_02), props));

                    }

                }
            }
        }
    });
}

IntellectImageShangBoDialog::~IntellectImageShangBoDialog()
{

    delete ui;
}
void IntellectImageShangBoDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void IntellectImageShangBoDialog::ReadShangBoPodConfig()
{
    QSettings settings("../data/conf/ShangBoPodConfig.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.beginGroup("ShangBoPodConfig");

    if(settings.contains("SensorId") && settings.value("SensorId").canConvert(QVariant::Type::String)){
        const QString& strSensorId(settings.value("SensorId", "").toString());
        if(!strSensorId.isNull()){
            m_StrSensorId = strSensorId.toStdString();
        }
    }

    if(settings.contains("ComponentID") && settings.value("ComponentID").canConvert(QVariant::Type::String)){
        const QString& strComponentID(settings.value("ComponentID", "").toString());
        if(!strComponentID.isNull()){
            m_StrComponentID = strComponentID.toStdString();
            bool ok = false;
            unsigned int tempComponentID = strComponentID.toULongLong(&ok);
            if(ok){
                m_ComponentID = tempComponentID;
            }else{
                m_StrComponentID = "";
                m_ComponentID    = 0;
            }
        }
    }

    if(settings.contains("ShangBoPodAutoConnect") && settings.value("ShangBoPodAutoConnect").canConvert(QVariant::Type::Bool)){
        m_ShangBoPodAutoConnect = settings.value("ShangBoPodAutoConnect", "false").toBool();
    }

    if(settings.contains("ShangBoPodConnectAddress") && settings.value("ShangBoPodConnectAddress").canConvert(QVariant::Type::String)){
        QHostAddress tempShangBoPodConnectAddress(settings.value("ShangBoPodConnectAddress", "").toString());
        if(!tempShangBoPodConnectAddress.isNull()){
            m_ServerAddress = tempShangBoPodConnectAddress;
        }
    }

    if(settings.contains("ShangBoPodConnectPort") && settings.value("ShangBoPodConnectPort").canConvert(QVariant::Type::UInt)){
        bool ok = false;
        unsigned int tempShangBoPodConnectPort = settings.value("ShangBoPodConnectPort", "2000").toUInt(&ok);
        if(ok){
            m_ServerPort = tempShangBoPodConnectPort;
        }
    }

    if(settings.contains("ShangBoPodLocalPort") && settings.value("ShangBoPodLocalPort").canConvert(QVariant::Type::UInt)){
        bool ok = false;
        unsigned int tempShangBoPodLocalPort = settings.value("ShangBoPodLocalPort", "2000").toUInt(&ok);
        if(ok){
            m_LocalPort = tempShangBoPodLocalPort;
        }
    }

    settings.endGroup();
}

void IntellectImageShangBoDialog::SaveShangBoPodConfig()
{
    QSettings settings("../data/conf/ShangBoPodConfig.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.beginGroup("ShangBoPodConfig");
    settings.setValue("ShangBoPodAutoConnect", ui->checkBox_Control_AutoConnect->isChecked());
    QHostAddress tempShangBoPodConnectAddress(ui->lineEdit_Control_ConnectAddress->text());
    if(!tempShangBoPodConnectAddress.isNull()){
        settings.setValue("ShangBoPodConnectAddress", tempShangBoPodConnectAddress.toString());
    }

    bool ok = false;
    unsigned int tempShangBoPodLocalPort = ui->lineEdit_Control_ConnectPort->text().toUInt(&ok);
    if(ok){
        settings.setValue("ShangBoPodLocalPort", tempShangBoPodLocalPort);
    }

    settings.setValue("tempShangBoPodLocalPort", "2000");
    settings.endGroup();
}

void IntellectImageShangBoDialog::InitSocketInfoConnect()
{
    qDebug() << "InitSocketInfoConnect bind:" << m_bindPort;
    if(!m_bindPort){
       m_bindPort = m_socket.bind(m_LocalPort, QUdpSocket::ShareAddress);
    }

    m_socket.connectToHost(m_ServerAddress, m_ServerPort);
}

void IntellectImageShangBoDialog::InitSocketInfoDisConnect()
{
    m_socket.disconnectFromHost();
}

void IntellectImageShangBoDialog::InitWeaponInfoList()
{

    m_ServerAddress = "192.168.1.166";

    if(!m_bindPort){
        m_bindPort = m_socket.bind(m_LocalPort, QUdpSocket::ShareAddress);
    }
    m_socket.connectToHost(m_ServerAddress, m_ServerPort);
    qDebug() << "InitWeaponInfoList bind:" << m_bindPort;

}

void IntellectImageShangBoDialog::sendShangBoControlMsg(const ShangBoIntellectMsg::ShangBoControlMsg &shangBoControlMsg)
{
    unsigned char msgBuffer[sizeof(ShangBoIntellectMsg::ShangBoControlMsg) + SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_EXT_LENGTH] = {0};
    msgBuffer[0] = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    msgBuffer[1] = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    msgBuffer[2] = sizeof(shangBoControlMsg);
    memcpy_s((msgBuffer + 3), sizeof(msgBuffer) - SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_EXT_LENGTH, &shangBoControlMsg, sizeof(shangBoControlMsg));

    msgBuffer[sizeof(msgBuffer) - 1 - SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_CRC_LENGTH] = ShangBoIntellectMsg::CRCCheck_ShangBo((const unsigned char*)(&shangBoControlMsg), sizeof(shangBoControlMsg) - 1);
    msgBuffer[sizeof(msgBuffer) - 1] = ShangBoIntellectMsg::CRCCheck_ShangBo((msgBuffer + 3), sizeof(shangBoControlMsg));

    qDebug() << QByteArray((const char *)msgBuffer, sizeof(msgBuffer)).toHex(' ');
    QByteArray byteData = QByteArray((const char *)msgBuffer, sizeof(msgBuffer));

    m_socket.write((const char *)msgBuffer, sizeof(msgBuffer));

}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_Up_clicked()
{

    if(ui == nullptr){
        return ;
    }

    const double controlSpeed = ui->doubleSpinBox_Control_Common_ControlSpeed->value();

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_HOLDERSERRCH;
    shangBoControlMsg.generalCrl.controlGeneral.paramX      = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY      = controlSpeed * 10;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed   = 0;

    sendShangBoControlMsg(shangBoControlMsg);

}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_Down_clicked()
{

    if(ui == nullptr){
        return ;
    }

    const double controlSpeed = ui->doubleSpinBox_Control_Common_ControlSpeed->value();
    if(controlSpeed < 0 || controlSpeed > 100){
        return ;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_HOLDERSERRCH;
    shangBoControlMsg.generalCrl.controlGeneral.paramX      = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY      = -controlSpeed * 10;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed   = 0;

    sendShangBoControlMsg(shangBoControlMsg);

}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_Left_clicked()
{

    if(ui == nullptr){
        return ;
    }

    const double controlSpeed = ui->doubleSpinBox_Control_Common_ControlSpeed->value();
    if(controlSpeed < 0 || controlSpeed > 100){
        return ;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_HOLDERSERRCH;
    shangBoControlMsg.generalCrl.controlGeneral.paramX      = -controlSpeed * 10;
    shangBoControlMsg.generalCrl.controlGeneral.paramY      = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed   = 0;

    sendShangBoControlMsg(shangBoControlMsg);

}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_Right_clicked()
{

    if(ui == nullptr){
        return ;
    }

    const double controlSpeed = ui->doubleSpinBox_Control_Common_ControlSpeed->value();
    if(controlSpeed < 0 || controlSpeed > 100){
        return ;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_HOLDERSERRCH;
    shangBoControlMsg.generalCrl.controlGeneral.paramX      = controlSpeed * 10;
    shangBoControlMsg.generalCrl.controlGeneral.paramY      = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed   = 0;

    sendShangBoControlMsg(shangBoControlMsg);

}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_Stop_clicked()
{

    if(ui == nullptr){
        return ;
    }

    const double controlSpeed = ui->doubleSpinBox_Control_Common_ControlSpeed->value();
    if(controlSpeed < 0 || controlSpeed > 100){
        return ;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2    = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_HOLDERSERRCH;
    shangBoControlMsg.generalCrl.controlGeneral.paramX      = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY      = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed   = 0;

    sendShangBoControlMsg(shangBoControlMsg);

}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_Center_clicked()
{

    if (ui == nullptr) {
        return;
    }

    const double controlSpeed = ui->doubleSpinBox_Control_Common_ControlSpeed->value();
    if (controlSpeed < 0 || controlSpeed > 100) {
        return;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_REGRESSIONCONTRE;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);

}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_BearFollow_clicked()
{

    if (ui == nullptr) {
        return;
    }

    b_BearFollow = !b_BearFollow;
    unsigned char cBearFollow = 0;
    if (b_BearFollow)
    {
        ui->pushButton_Control_Common_BearFollow->setText(QString::fromLocal8Bit("关方位跟随"));
        cBearFollow = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_FOLLOW_POSITION;
    }
    else
    {
        ui->pushButton_Control_Common_BearFollow->setText(QString::fromLocal8Bit("开方位跟随"));
        cBearFollow = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_FOLLOW_ON;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = cBearFollow;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_OffMotor_clicked()
{

    if (ui == nullptr) {
        return;
    }
    b_Motor = !b_Motor;
    unsigned char cMotor = 0;
    if (b_Motor)
    {
        ui->pushButton_Control_Common_OffMotor->setText(QString::fromLocal8Bit("关电机"));
        cMotor = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_DYNAME_ON;
    }
    else
    {
        ui->pushButton_Control_Common_OffMotor->setText(QString::fromLocal8Bit("开电机"));
        cMotor = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_DYNAME_OFF;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = cMotor;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_StartDistance_clicked()
{

    if (ui == nullptr) {
        return;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_LASERRANGING_ON;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_StopDistance_clicked()
{

    if (ui == nullptr) {
        return;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_LASERRANGING_OFF;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_pushButton_Trace_Common_Recognition1_clicked()
{
}

void IntellectImageShangBoDialog::on_pushButton_Trace_Common_Recognition2_clicked()
{

}

void IntellectImageShangBoDialog::on_pushButton_Image_Common_Photograph_clicked()
{

    if (ui == nullptr) {
        return;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_PHOTOGRAGH;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_pushButton_Image_Common_ChangeMultiple_Add_clicked()
{

    if (ui == nullptr) {
        return;
    }
    const double multiple = ui->doubleSpinBox_Image_Common_ChangeMulitiple->value();
    if (multiple < 0 || multiple > 10)
        return;

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_ZOOMINSTRUCT;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = multiple * 10;

    sendShangBoControlMsg(shangBoControlMsg);

    m_Timer.singleShot(100, this, [this](){
        ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
        memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
        shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
        shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
        shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_ZOOMINSTRUCT;
        shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
        shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
        shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
        shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;
        sendShangBoControlMsg(shangBoControlMsg);
    });
}

void IntellectImageShangBoDialog::on_pushButton_Image_Common_ChangeMulitiple_Sub_clicked()
{

    if (ui == nullptr) {
        return;
    }
    const double multiple = ui->doubleSpinBox_Image_Common_ChangeMulitiple->value();
    if (multiple < 0)
        return;

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_ZOOMINSTRUCT;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = -multiple * 10;

    sendShangBoControlMsg(shangBoControlMsg);

    m_Timer.singleShot(100, this, [this](){
        ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
        memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
        shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
        shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
        shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_ZOOMINSTRUCT;
        shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
        shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
        shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
        shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;
        sendShangBoControlMsg(shangBoControlMsg);
    });
}

void IntellectImageShangBoDialog::on_pushButton_Image_Common_Zoom_Add_clicked()
{

    if (ui == nullptr) {
        return;
    }
    const double ImageZoom = ui->doubleSpinBox_Image_Common_ChangleZoom->value();
    if (ImageZoom < 0)
        return;

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_ASSIGNZOOM;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = ImageZoom * 10;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);

    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;
    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_pushButton_Image_Common_StartVideo_clicked()
{

    if (ui == nullptr) {
        return;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_STORAGE_ON;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_pushButton_Image_Common_StopVideo_clicked()
{

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_STORAGE_OFF;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_checkBox_Trace_Common_OpenRecognition_stateChanged(int arg1)
{

    if (ui == nullptr) {
        return;
    }

    unsigned char recognition = 0;
    if (arg1)
        recognition = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_TARGETDISTINGGUISH_ON;
    else
        recognition = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_TARGETDISTINGGUISH_OFF;
    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = recognition;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = 0;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_pushButton_Control_Connect_clicked()
{
    SaveShangBoPodConfig();
    ReadShangBoPodConfig();
    ui->pushButton_Control_Connect->setEnabled(false);
    ui->pushButton_Control_DisConnect->setEnabled(true);
    InitSocketInfoConnect();
}

void IntellectImageShangBoDialog::on_pushButton_Control_DisConnect_clicked()
{
    ui->pushButton_Control_Connect->setEnabled(true);
    ui->pushButton_Control_DisConnect->setEnabled(false);

    InitSocketInfoDisConnect();
}

void IntellectImageShangBoDialog::on_checkBox_Control_AutoConnect_stateChanged(int arg1)
{
    SaveShangBoPodConfig();
    ReadShangBoPodConfig();
}

void IntellectImageShangBoDialog::on_lineEdit_Control_ConnectAddress_editingFinished()
{
    SaveShangBoPodConfig();
    ReadShangBoPodConfig();
}

void IntellectImageShangBoDialog::on_lineEdit_Control_ConnectPort_editingFinished()
{
    SaveShangBoPodConfig();
    ReadShangBoPodConfig();
}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_HDown_clicked()
{

    if (ui == nullptr) {
        return;
    }

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_FRAMEANGLE;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = 0;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = -90 * 100;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}

void IntellectImageShangBoDialog::on_pushButton_Control_Common_SetAngle_clicked()
{

    if (ui == nullptr) {
        return;
    }

    const double angle_Yaw = ui->doubleSpinBox_Control_Common_SetAngle_Yaw->value();
    if (angle_Yaw < -180 || angle_Yaw > 180)
        return;

    const double angle_Pitch = ui->doubleSpinBox_Control_Common_SetAngle_Pitch->value();
    if (angle_Pitch < -120 || angle_Pitch > 120)
        return;

    ShangBoIntellectMsg::ShangBoControlMsg shangBoControlMsg;
    memset(&shangBoControlMsg, 0, sizeof(shangBoControlMsg));
    shangBoControlMsg.synchro1 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD;
    shangBoControlMsg.synchro2 = SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD;
    shangBoControlMsg.controlInfo = ShangBoIntellectMsg::ControlInfo::SHANGBOCONTROLMSG_FRAMEANGLE;
    shangBoControlMsg.generalCrl.controlGeneral.paramX = angle_Yaw * 100;
    shangBoControlMsg.generalCrl.controlGeneral.paramY = angle_Pitch * 100;
    shangBoControlMsg.generalCrl.controlGeneral.framesParam = 0;
    shangBoControlMsg.generalCrl.controlGeneral.zoomSpeed = 0;

    sendShangBoControlMsg(shangBoControlMsg);
}
