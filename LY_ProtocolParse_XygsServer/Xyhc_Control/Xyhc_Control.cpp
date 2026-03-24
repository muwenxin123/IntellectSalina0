#include "Xyhc_Control.h"

#include "DataManage/Common/TypeDefine_Weapon.h"
#include "DataManage/NebulaUAVState/NebulaStatePointInfo.h"
#include "DataManage/Common/TypeDefine_WeaponSystem.h"
#include "DataManage/KeyEvent/SimuPosition.h"

#include "Protocol/XygsSeverClientControl/XyhcControl_SeverClient.h"

#include <QFile>
#include <QSettings>
#include <QDir>
#include <QDebug>

using namespace qnzkna::NetSeverClient;

#define UAV_PARAMETER_RECVMSG_NAME    "MsgUAVParameterState"

#define XYGSJL_LOG_DIR        "../data/Record/Record_UAVFlightLog/"
#define XYGSJL_LOG_CMD_DIR    "../data/Record/Record_UAVFlightLog/"

#define XYGSJL_LOG_HEADER     "时间,无人机,温度,功耗,电压,电流,飞控输出通道1,飞控输出通道2,飞控输出通道3,飞控输出通道4,飞控输出通道5,飞控输出通道6,\
飞控输出通道7,飞控输出通道8,飞控输出通道9,飞控输出通道10,差分状态,纬度,经度,GPS高度,相对原点高度,实时距地高度,\
目标速度,地速,组合导航天向速度,轨迹角,俯仰角,横滚角,副翼,升降,油门,方向,锁定状态,机头指向角,飞行控制模式,状态,\
飞行阶段,GPS丢星时间,链路中断时间,飞行区域范围,已飞圈数,未飞圈数,飞行距离,飞行时间,目标航点,目标高度,目标航向,\
悬停时间,飞控电压,动力电压,GPS星数,水平精度HDOP,垂直精度VDOP,速度精度SDOP,年,月,日,时,分,秒,飞控温度,侧偏距,\
输入通道1,输入通道2,输入通道3,输入通道4,输入通道5,输入通道6,输入通道7,预留,指令返回值1,指令返回值2"

#define XYGSJL_LOG_CMD_HEADER "时间,无人机,帧头,指令"

CLogRecordCVS    IProtocolParseXygsServerImpl_Xyhc_Control::s_LogCVS(XYGSJL_LOG_HEADER, XYGSJL_LOG_DIR, "JLServer");
CLogRecordCVSCMD IProtocolParseXygsServerImpl_Xyhc_Control::s_LogCVSCMDUp(XYGSJL_LOG_CMD_HEADER, XYGSJL_LOG_CMD_DIR, "JLServerCmdUp");
CLogRecordCVSCMD IProtocolParseXygsServerImpl_Xyhc_Control::s_LogCVSCMDDown(XYGSJL_LOG_CMD_HEADER, XYGSJL_LOG_CMD_DIR, "JLServerCmdDown");

IProtocolParseXygsServerImpl_Xyhc_Control::IProtocolParseXygsServerImpl_Xyhc_Control(void)
    : IProtocolParseXygsServerImpl_Control("MsgUAVFlyControlSend"
                                           , NETHEADER_UCTYPE_XYHC_CONTROL
                                           , NODE_TYPE_WEAPONCOMPONENT
                                           )
    , m_bRecordHeartbeats(false)
    , m_bControlCommandRecord(true)
    , m_bTelemetryLogRecord(true)
{

    QSettings settings("../data/conf/FlightLogConfig.ini", QSettings::IniFormat);

    settings.beginGroup("FlightLogRecord");
    if(settings.contains("HeartbeatsRecordFlag")){
        m_bRecordHeartbeats = settings.value("HeartbeatsRecordFlag", false).toBool();
    }
    settings.endGroup();

    settings.beginGroup("ControlCommand");
    if(settings.contains("ControlCommandRecordFlag")){
        m_bControlCommandRecord = settings.value("ControlCommandRecordFlag", false).toBool();
    }
    settings.endGroup();

    settings.beginGroup("TelemetryLog");
    if(settings.contains("TelemetryLogRecordFlag")){
        m_bTelemetryLogRecord = settings.value("TelemetryLogRecordFlag", false).toBool();
    }
    settings.endGroup();

    const QString strFlightLogDir = XYGSJL_LOG_DIR;
    QDir dir(strFlightLogDir);
    if (!dir.exists()){
        dir.makeAbsolute();
        dir.mkdir(dir.path());
    }

    qDebug() << __LINE__ <<sizeof(qnzkna::NetSeverClient::XygsXyhcControlInfo::XyhcControlInfo);

    qDebug() << __LINE__ <<sizeof(NetHeader);

    qDebug() << __LINE__ <<sizeof(xygsJL_01);

    qDebug() << __LINE__ <<sizeof(xygsJL_02);

    qDebug() << __LINE__ <<sizeof(xygsJL_03);

    qDebug() << __LINE__ <<sizeof(xygsJL_04);

    qDebug() << __LINE__ <<sizeof(xygsJL_05);

    qDebug() << __LINE__ <<sizeof(xygsJL_06);

    qDebug() << __LINE__ <<sizeof(xygsJL_07);

    qDebug() << __LINE__ <<sizeof(xygsJL_08);

    qDebug() << __LINE__ <<sizeof(xygsJL_V7_08);

    qDebug() << __LINE__ <<sizeof(xygsJL_09);

    qDebug() << __LINE__ <<sizeof(xygsJL_V7_09);

    qDebug() << __LINE__ <<sizeof(xygsJL_10);

    qDebug() << __LINE__ <<sizeof(xygsJL_11);

    qDebug() << __LINE__ <<sizeof( xygsJL_12);

    qDebug() << __LINE__ <<sizeof(xygsJL_13);

    qDebug() << __LINE__ <<sizeof(xygsJL_14);

    qDebug() << __LINE__ <<sizeof(xygsJL_15);

    qDebug() << __LINE__ <<sizeof(xygsJL_16);

    qDebug() << __LINE__ <<sizeof(xygsJL_17);

    qDebug() << __LINE__ <<sizeof(xygsJL_18);

    qDebug() << __LINE__ <<sizeof(xygsJL_19);

    qDebug() << __LINE__ <<sizeof(xygsJL_21);

    qDebug() << __LINE__ <<sizeof(xygsJL_22);

    qDebug() << __LINE__ <<sizeof(xygsJL_23);

    qDebug() << __LINE__ <<sizeof(xygsJL_25);

    qDebug() << __LINE__ <<sizeof(xygsJL_26);

    qDebug() << __LINE__ <<sizeof(xygsJL_A3A3);
}

IProtocolParseXygsServerImpl_Xyhc_Control::~IProtocolParseXygsServerImpl_Xyhc_Control(void)
{
}

bool IProtocolParseXygsServerImpl_Xyhc_Control::Pack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext)
{
    if(msg == nullptr || pContext == nullptr){
        return false;
    }

    const unsigned int nInBufLen = msg->getMessageLength();
    if (0 >= nInBufLen){
        return false;
    }

    const qnzkna::framework::MessageReferenceProps& props = msg->getMessageProperties();
    const std::string& strWeaponID = props.find("WeaponID");

    if(strWeaponID.empty()){
        return false;
    }

    const char* const pInBuf = msg->getMessage();

    unsigned int nOutBufLen = 0;
    const long nFlags = *(const long* const)(pInBuf);
    if(0xA3A3 == nFlags){
        nOutBufLen = sizeof (xygsJL_A3A3);
    }
    else if(540708 == nFlags){
        nOutBufLen = sizeof (xygsJL_V7_08);
    }
    else if(540709 == nFlags){
        nOutBufLen = sizeof (xygsJL_V7_09);
    }
    else if (0x06 == nFlags) {
        nOutBufLen = sizeof (xygsJL_06);
    }
    else if(0x0B == nFlags){
        nOutBufLen = sizeof (xygsJL_11);
    }
    else if(17 == nFlags) {
        nOutBufLen = sizeof (xygsJL_17);
    }
    else if (1 == nFlags) {
        nOutBufLen = sizeof (xygsJL_01);
    }
    else if(2 == nFlags) {
        nOutBufLen = sizeof (xygsJL_02);
    }
    else if(3 == nFlags) {
        nOutBufLen = sizeof (xygsJL_03);
    }
    else if(4 == nFlags) {
        nOutBufLen = sizeof (xygsJL_04);
    }
    else if(5 == nFlags) {
        nOutBufLen = sizeof (xygsJL_05);
    }
    else if(7 == nFlags) {
        nOutBufLen = sizeof (xygsJL_07);
    }
    else if(8 == nFlags) {
        nOutBufLen = sizeof (xygsJL_08);
    }
    else if(0x09 == nFlags) {
        nOutBufLen = sizeof (xygsJL_09);
    }
    else if(10 == nFlags) {
        nOutBufLen = sizeof (xygsJL_10);
    }
    else if(12 == nFlags) {
        nOutBufLen = sizeof (xygsJL_12);
    }
    else if(13 == nFlags) {
        nOutBufLen = sizeof (xygsJL_13);
    }
    else if(14 == nFlags) {
        nOutBufLen = sizeof (xygsJL_14);
    }
    else if(15 == nFlags) {
        nOutBufLen = sizeof (xygsJL_15);
    }
    else if(16 == nFlags) {
        nOutBufLen = sizeof (xygsJL_16);
    }
    else if(18 == nFlags) {
        nOutBufLen = sizeof (xygsJL_18);
    }
    else if(19 == nFlags) {
        nOutBufLen = sizeof (xygsJL_19);
    }
    else if(20 == nFlags) {
        nOutBufLen = sizeof (xygsJL_20);
    }
    else if(21 == nFlags) {
        nOutBufLen = sizeof (xygsJL_21);
    }
    else if(22 == nFlags) {
        nOutBufLen = sizeof (xygsJL_22);
    }
    else if(23 == nFlags) {
        nOutBufLen = sizeof (xygsJL_23);
    }
    else if(24 == nFlags) {
        return false;
    }
    else if(25 == nFlags) {
        nOutBufLen = sizeof (xygsJL_25);
    }else{
        return false;
    }

    if (nOutBufLen <= 0){
        return false;
    }

    if (nOutBufLen != nInBufLen){
        return false;
    }

    bool ok = false;
    const unsigned long long unNodeID= QString::fromStdString(strWeaponID).toULongLong(&ok);
    if(!ok || (unNodeID == 0) ){
        return false;
    }

    qnzkna::NetSeverClient::XygsXyhcControlInfo::XyhcControlInfo netMsg;

    netMsg.header.ucType       = IProtocolParseXygsServerImpl_Control::m_ucType;
    netMsg.header.ucSubType    = NETHEADER_UCSUBTYPE_XYHC_CONTROL_UP_TO_SERVER;
    netMsg.header.unLength     = 0;
    netMsg.header.unMask       = NETHEADER_UNMASK_SERVER_CLIENT;

    netMsg.header.unNodeType   = IProtocolParseXygsServerImpl_Control::m_unNodeType;
    netMsg.header.unNodeID     = unNodeID;
    netMsg.header.unSeatType   = NETHEADER_UNSEATTYPE_CLIENT;
    netMsg.header.unSeatID     = IProtocolParseXygsServerImpl_Control::getSeatID();

    memcpy(&netMsg.XyhcControl, pInBuf, (std::min)((unsigned int)sizeof(netMsg.XyhcControl), nOutBufLen));
    netMsg.header.unLength = nOutBufLen + sizeof (NetHeader);

    IProtocolParseXygsServerImpl_Control::NetSend_XygsServer( netMsg.header.unLength,  (const char* const)(&netMsg));

    {

        if (m_bControlCommandRecord && ((0xA3A3 != nFlags) || m_bRecordHeartbeats) ) {
            if (!s_LogCVSCMDUp.isopen()) {
                s_LogCVSCMDUp.open();
            }
            if (s_LogCVSCMDUp.isopen()) {

#ifdef Q_OS_WIN
                const unsigned int messageFormatStringLength = 20 + MAX_WEAPONMODEL_ID_LENGTH + qnzkna::framework::MessageReferenceLength * 4;
                char formatString[20 + MAX_WEAPONMODEL_ID_LENGTH + 3 + messageFormatStringLength] = { 0 };
                const std::string& targetID = props.find("TargetID");
                SYSTEMTIME systime;
                GetLocalTime(&systime);
                int len = snprintf(formatString, sizeof(formatString), "\n%02d%02d_%02d:%02d:%02d:%03d,%s,up,"
                                    , systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds
                                    , targetID.c_str()
                                    );

                int messageFormatStringPrintLength = 0;
                if(len > 0 && len < messageFormatStringLength){
                    int messageFormatStringPos         = len;
                    for (int i = 0; i < nOutBufLen && (messageFormatStringPos + 3) <= messageFormatStringLength; i ++) {
                        unsigned char nChar = (unsigned char)(pInBuf[i]);
                        messageFormatStringPrintLength += snprintf((formatString + messageFormatStringPos), 4, "%02X ", nChar);
                        messageFormatStringPos += 3;
                    }
                }

#else
                const unsigned int messageFormatStringLength = 20 + MAX_WEAPONMODEL_ID_LENGTH + qnzkna::framework::MessageReferenceLength * 4;
                char formatString[20 + MAX_WEAPONMODEL_ID_LENGTH + 3 + messageFormatStringLength] = { 0 };
                QDateTime dateTime = QDateTime::currentDateTime();
                const std::string& targetID = props.find("TargetID");
                int len = sprintf(formatString, sizeof(formatString), "\n%02d%02d_%02d:%02d:%02d:%03d,%s,up,"
                                    , dateTime.date().month(), dateTime.date().day(), dateTime.time().hour(), dateTime.time().minute(), dateTime.time().second(), dateTime.time().msec()
                                    , targetID.c_str()
                                    );

                int messageFormatStringPrintLength = 0;
                if(len > 0 && len < messageFormatStringLength){
                    int messageFormatStringPos         = len;
                    for (int i = 0; i < nOutBufLen && (messageFormatStringPos + 3) <= messageFormatStringLength; i ++) {
                        unsigned char nChar = (unsigned char)(pInBuf[i]);
                        messageFormatStringPrintLength += sprintf((formatString + messageFormatStringPos), 5, "%02X ", nChar);
                        messageFormatStringPos += 3;
                    }
                }
#endif
                s_LogCVSCMDUp.write(formatString, (len + messageFormatStringPrintLength));
            }
        }

    }
    return true;
}

bool IProtocolParseXygsServerImpl_Xyhc_Control::UnPack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext)
{
    if(msg == nullptr || pContext == nullptr){
        return false;
    }

    return false;
}

bool IProtocolParseXygsServerImpl_Xyhc_Control::NetHeader_XygsServer(const int nLength, const char * const pRecvBuf)
{
    if(nLength <= sizeof (NetHeader))
    {
        return false;
    }

    const auto* const pNetHeader = (const NetHeader* const)(pRecvBuf);
    if(pNetHeader->unLength != nLength){
        return false;
    }

    if((pNetHeader->ucType == IProtocolParseXygsServerImpl_Control::m_ucType)
            && (pNetHeader->ucSubType == NETHEADER_UCSUBTYPE_XYHC_CONTROL_DOWN_TO_CLIENT)
            && (pNetHeader->unNodeType == IProtocolParseXygsServerImpl_Control::m_unNodeType)
            && (pNetHeader->unSeatType == NETHEADER_UNSEATTYPE_SERVER)
            )
    {
        return true;
    }

    return false;
}

bool IProtocolParseXygsServerImpl_Xyhc_Control::NetRecv_XygsServer(const int nLength, const char * const pRecvBuf)
{
    if(!NetHeader_XygsServer(nLength, pRecvBuf)){
        return false;
    }

    const auto* const pNetMsg = (const qnzkna::NetSeverClient::XygsXyhcControlInfo::XyhcControlInfoMsgStatus* const)(pRecvBuf);

    const int msgLength = pNetMsg->header.unLength - sizeof (NetHeader);

    if(msgLength <= 8 || msgLength >= 512){
        return false;
    }

    const QString& strWeaponId = QString::number(pNetMsg->header.unNodeID);
    if (strWeaponId.isEmpty()) {
        return false;
    }

    const long nBagFlags = pNetMsg->XyhcStatus.xygsJL_A3A3_MsgStatus_info.flags;
    if (0 == nBagFlags){
        return false;
    }

    qnzkna::framework::IRegistry* pContext = IProtocolParseXygsServerActivator::Get().getIRegistryImpl();
    if(pContext == nullptr) {
        return false;
    }

    qnzkna::framework::MessageReferenceProps props;
    props.put("TargetType",   std::to_string(NODE_TYPE_WEAPONCOMPONENT));
    props.put("TargetID",     strWeaponId.toStdString());
    props.put("WeaponID",     strWeaponId.toStdString());
    props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54);

    {

        if (m_bControlCommandRecord && 20 != nBagFlags && 0 != nBagFlags )
        {
            if (!s_LogCVSCMDDown.isopen()) {
                s_LogCVSCMDDown.open();
            }
            if (s_LogCVSCMDDown.isopen()) {

#ifdef Q_OS_WIN
                const unsigned int messageFormatStringLength = 20 + MAX_WEAPONMODEL_ID_LENGTH + qnzkna::framework::MessageReferenceLength * 4;
                char formatString[20 + MAX_WEAPONMODEL_ID_LENGTH + 5 + messageFormatStringLength] = { 0 };
                const std::string& targetID = props.find("TargetID");
                SYSTEMTIME systime;
                GetLocalTime(&systime);

                int len = snprintf(formatString, sizeof(formatString), "\n%02d%02d_%02d:%02d:%02d:%03d,%s,dw,"
                                    , systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds
                                    , targetID.c_str()
                                    );

                int messageFormatStringPrintLength = 0;
                if(len > 0 && len < messageFormatStringLength){
                    int messageFormatStringPos         = len;
                    const unsigned char* const pInBuf = (const unsigned char* const)(&(pNetMsg->XyhcStatus));
                    for (int i = 0; i < msgLength && (messageFormatStringPos + 3) <= messageFormatStringLength; i ++) {
                        unsigned char nChar = (unsigned char)(pInBuf[i]);
                        messageFormatStringPrintLength += snprintf((formatString + messageFormatStringPos), 4, "%02X ", nChar);
                        messageFormatStringPos += 3;
                    }
                }

#else
                const unsigned int messageFormatStringLength = 20 + MAX_WEAPONMODEL_ID_LENGTH + qnzkna::framework::MessageReferenceLength * 4;
                char formatString[20 + MAX_WEAPONMODEL_ID_LENGTH + 5 + messageFormatStringLength] = { 0 };
                QDateTime dateTime = QDateTime::currentDateTime();
                const std::string& targetID = props.find("TargetID");

                int len = sprintf(formatString, sizeof(formatString), "\n%02d%02d_%02d:%02d:%02d:%03d,%s,dw,"
                                    , dateTime.date().month(), dateTime.date().day(), dateTime.time().hour(), dateTime.time().minute(), dateTime.time().second(), dateTime.time().msec()
                                    , targetID.c_str()
                                    );

                int messageFormatStringPrintLength = 0;
                if(len > 0 && len < messageFormatStringLength){
                    int messageFormatStringPos         = len;
                    const unsigned char* const pInBuf = (const unsigned char* const)(&(pNetMsg->XyhcStatus));
                    for (int i = 0; i < msgLength && (messageFormatStringPos + 3) <= messageFormatStringLength; i ++) {
                        unsigned char nChar = (unsigned char)(pInBuf[i]);
                        messageFormatStringPrintLength += sprintf((formatString + messageFormatStringPos), 4, "%02X ", nChar);
                        messageFormatStringPos += 3;
                    }
                }
#endif
                s_LogCVSCMDDown.write(formatString, (len + messageFormatStringPrintLength));
            }
        }
    }
    if(540708 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_V7_08))
        {
            return false;
        }
        const xygsJL_V7_08& bag = pNetMsg->XyhcStatus.xygsJL_V7_08_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(540709 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_V7_09))
        {
            return false;
        }
        const xygsJL_V7_09& bag = pNetMsg->XyhcStatus.xygsJL_V7_09_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if (1 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_01))
        {
            return false;
        }
        const xygsJL_01& bag = pNetMsg->XyhcStatus.xygsJL_01_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(2 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_02))
        {
            return false;
        }
        const xygsJL_02& bag = pNetMsg->XyhcStatus.xygsJL_02_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(3 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_03))
        {
            return false;
        }
        const xygsJL_03& bag = pNetMsg->XyhcStatus.xygsJL_03_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(4 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_04))
        {
            return false;
        }
        const xygsJL_04& bag = pNetMsg->XyhcStatus.xygsJL_04_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(5 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_05))
        {
            return false;
        }
        const xygsJL_05& bag = pNetMsg->XyhcStatus.xygsJL_05_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(6 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_06))
        {
            return false;
        }
        const xygsJL_06& bag = pNetMsg->XyhcStatus.xygsJL_06_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(7 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_07))
        {
            return false;
        }
        const xygsJL_07& bag = pNetMsg->XyhcStatus.xygsJL_07_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(8 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_08))
        {
            return false;
        }
        const xygsJL_08& bag = pNetMsg->XyhcStatus.xygsJL_08_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(9 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_09))
        {
            return false;
        }
        const xygsJL_09& bag = pNetMsg->XyhcStatus.xygsJL_09_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(10 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_10))
        {
            return false;
        }
        const xygsJL_10& bag = pNetMsg->XyhcStatus.xygsJL_10_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(11 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_11))
        {
            return false;
        }
        const xygsJL_11& bag = pNetMsg->XyhcStatus.xygsJL_11_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(12 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_12))
        {
            return false;
        }
        const xygsJL_12& bag = pNetMsg->XyhcStatus.xygsJL_12_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(13 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_13))
        {
            return false;
        }
        const xygsJL_13& bag = pNetMsg->XyhcStatus.xygsJL_13_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(14 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_14))
        {
            return false;
        }
        const xygsJL_14& bag = pNetMsg->XyhcStatus.xygsJL_14_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(15 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_15))
        {
            return false;
        }
        const xygsJL_15& bag = pNetMsg->XyhcStatus.xygsJL_15_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(16 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_16))
        {
            return false;
        }
        const xygsJL_16& bag = pNetMsg->XyhcStatus.xygsJL_16_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(17 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_17))
        {
            return false;
        }
        const xygsJL_17& bag = pNetMsg->XyhcStatus.xygsJL_17_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(18 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_18))
        {
            return false;
        }
        const xygsJL_18& bag = pNetMsg->XyhcStatus.xygsJL_18_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(19 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_19))
        {
            return false;
        }
        const xygsJL_19& bag = pNetMsg->XyhcStatus.xygsJL_19_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    else if(20 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_20))
        {
            return false;
        }

        const xygsJL_20& bag = pNetMsg->XyhcStatus.xygsJL_20_MsgStatus_info;

        pContext->messageNotify(new qnzkna::framework::MessageReference("MsgUAVRealTimeState", (const char*)&bag, sizeof(bag), props));

        if(m_bTelemetryLogRecord)
        {
            if (!s_LogCVS.isopen()) {
                s_LogCVS.open();
            }

            if (s_LogCVS.isopen()) {

#ifdef Q_OS_WIN

                const telemetryInformation& telemetryInfo = bag.param;
                char formatString[20 + MAX_WEAPONMODEL_ID_LENGTH + qnzkna::framework::MessageReferenceLength * 6] = { 0 };
                SYSTEMTIME systime;
                GetLocalTime(&systime);
                const std::string& targetID = props.find("TargetID");
                int len = snprintf(formatString, sizeof(formatString), "\n%02d%02d_%02d:%02d:%02d:%03d,%s,%d,%d,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%d,%f,%f,%d,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
                                    , systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds
                                    , targetID.c_str()
                                    , telemetryInfo._temperature
                                    , telemetryInfo._powerconsumption
                                    , telemetryInfo._voltage
                                    , telemetryInfo._current
                                    , telemetryInfo._fkOutputChannal01
                                    , telemetryInfo._fkOutputChannal02
                                    , telemetryInfo._fkOutputChannal03
                                    , telemetryInfo._fkOutputChannal04
                                    , telemetryInfo._fkOutputChannal05
                                    , telemetryInfo._fkOutputChannal06
                                    , telemetryInfo._fkOutputChannal07
                                    , telemetryInfo._fkOutputChannal08
                                    , telemetryInfo._fkOutputChannal09
                                    , telemetryInfo._fkOutputChannal10
                                    , telemetryInfo.differenceStatus
                                    , telemetryInfo.position.lng
                                    , telemetryInfo.position.lat
                                    , telemetryInfo.gpsHeigth
                                    , telemetryInfo.relativeOriginHeigth
                                    , telemetryInfo.relativeGroundHeigth
                                    , telemetryInfo.targetVelocity
                                    , telemetryInfo.groundVelocity
                                    , telemetryInfo.verticalVelocity
                                    , telemetryInfo.angleofRudder
                                    , telemetryInfo.angleofPitch
                                    , telemetryInfo.angleofRoll
                                    , telemetryInfo._aileron
                                    , telemetryInfo._elevator
                                    , telemetryInfo._throttle
                                    , telemetryInfo._rudder
                                    , telemetryInfo.lockStatus
                                    , telemetryInfo.angleofNose
                                    , telemetryInfo.controlMode
                                    , telemetryInfo._status
                                    , telemetryInfo.flyPeriod
                                    , telemetryInfo.gpsLostTime
                                    , telemetryInfo.communicationLostTime
                                    , telemetryInfo.flyZoo
                                    , telemetryInfo.finishCircleNum
                                    , telemetryInfo.unFinishCircleNum
                                    , telemetryInfo.flyDistance
                                    , telemetryInfo.flyTime
                                    , telemetryInfo.targetPoint
                                    , telemetryInfo.targetHeigth
                                    , telemetryInfo.targetDirection
                                    , telemetryInfo.hoverTime
                                    , telemetryInfo.fkvoltage
                                    , telemetryInfo.powervoltage
                                    , telemetryInfo.gpsNumber
                                    , telemetryInfo.levelPrecision
                                    , telemetryInfo.verticalPrecision
                                    , telemetryInfo.velocityPrecision
                                    , telemetryInfo._year
                                    , telemetryInfo._month
                                    , telemetryInfo._day
                                    , telemetryInfo._hour
                                    , telemetryInfo._min
                                    , telemetryInfo._sec
                                    , telemetryInfo.fkTemperature
                                    , telemetryInfo._offset
                                    , telemetryInfo.inputChannel01
                                    , telemetryInfo.inputChannel02
                                    , telemetryInfo.inputChannel03
                                    , telemetryInfo.inputChannel04
                                    , telemetryInfo.inputChannel05
                                    , telemetryInfo.inputChannel06
                                    , telemetryInfo.inputChannel07
                                    , telemetryInfo.temp
                                    , telemetryInfo.commandReturnValue01
                                    , telemetryInfo.commandReturnValue02
                                    );

#else
                const telemetryInformation& telemetryInfo = bag.param;
                char formatString[20 + MAX_WEAPONMODEL_ID_LENGTH + qnzkna::framework::MessageReferenceLength * 6] = { 0 };
                QDateTime dateTime = QDateTime::currentDateTime();
                const std::string& targetID = props.find("TargetID");
                int len = sprintf(formatString, sizeof(formatString),"\n%02d%02d_%02d:%02d:%02d:%03d,%s,%d,%d,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%d,%f,%f,%d,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
                                    , dateTime.date().month(), dateTime.date().day(), dateTime.time().hour(), dateTime.time().minute(), dateTime.time().second(), dateTime.time().msec()
                                    , targetID.c_str()
                                    , telemetryInfo._temperature
                                    , telemetryInfo._powerconsumption
                                    , telemetryInfo._voltage
                                    , telemetryInfo._current
                                    , telemetryInfo._fkOutputChannal01
                                    , telemetryInfo._fkOutputChannal02
                                    , telemetryInfo._fkOutputChannal03
                                    , telemetryInfo._fkOutputChannal04
                                    , telemetryInfo._fkOutputChannal05
                                    , telemetryInfo._fkOutputChannal06
                                    , telemetryInfo._fkOutputChannal07
                                    , telemetryInfo._fkOutputChannal08
                                    , telemetryInfo._fkOutputChannal09
                                    , telemetryInfo._fkOutputChannal10
                                    , telemetryInfo.differenceStatus
                                    , telemetryInfo.position.lng
                                    , telemetryInfo.position.lat
                                    , telemetryInfo.gpsHeigth
                                    , telemetryInfo.relativeOriginHeigth
                                    , telemetryInfo.relativeGroundHeigth
                                    , telemetryInfo.targetVelocity
                                    , telemetryInfo.groundVelocity
                                    , telemetryInfo.verticalVelocity
                                    , telemetryInfo.angleofRudder
                                    , telemetryInfo.angleofPitch
                                    , telemetryInfo.angleofRoll
                                    , telemetryInfo._aileron
                                    , telemetryInfo._elevator
                                    , telemetryInfo._throttle
                                    , telemetryInfo._rudder
                                    , telemetryInfo.lockStatus
                                    , telemetryInfo.angleofNose
                                    , telemetryInfo.controlMode
                                    , telemetryInfo._status
                                    , telemetryInfo.flyPeriod
                                    , telemetryInfo.gpsLostTime
                                    , telemetryInfo.communicationLostTime
                                    , telemetryInfo.flyZoo
                                    , telemetryInfo.finishCircleNum
                                    , telemetryInfo.unFinishCircleNum
                                    , telemetryInfo.flyDistance
                                    , telemetryInfo.flyTime
                                    , telemetryInfo.targetPoint
                                    , telemetryInfo.targetHeigth
                                    , telemetryInfo.targetDirection
                                    , telemetryInfo.hoverTime
                                    , telemetryInfo.fkvoltage
                                    , telemetryInfo.powervoltage
                                    , telemetryInfo.gpsNumber
                                    , telemetryInfo.levelPrecision
                                    , telemetryInfo.verticalPrecision
                                    , telemetryInfo.velocityPrecision
                                    , telemetryInfo._year
                                    , telemetryInfo._month
                                    , telemetryInfo._day
                                    , telemetryInfo._hour
                                    , telemetryInfo._min
                                    , telemetryInfo._sec
                                    , telemetryInfo.fkTemperature
                                    , telemetryInfo._offset
                                    , telemetryInfo.inputChannel01
                                    , telemetryInfo.inputChannel02
                                    , telemetryInfo.inputChannel03
                                    , telemetryInfo.inputChannel04
                                    , telemetryInfo.inputChannel05
                                    , telemetryInfo.inputChannel06
                                    , telemetryInfo.inputChannel07
                                    , telemetryInfo.temp
                                    , telemetryInfo.commandReturnValue01
                                    , telemetryInfo.commandReturnValue02
                                    );
#endif

                s_LogCVS.write(formatString, len);
            }
        }

    }
    else if( 23 == nBagFlags) {
        if(msgLength != sizeof (xygsJL_23))
        {
            return false;
        }
        const xygsJL_23& bag = pNetMsg->XyhcStatus.xygsJL_23_MsgStatus_info;
        pContext->messageNotify(new qnzkna::framework::MessageReference(UAV_PARAMETER_RECVMSG_NAME, reinterpret_cast<const char*>(&bag), sizeof(bag), props));
        return true;
    }
    return false;
}

ADD_PROTOCOLPARSEXYGSSERVERIMPL_CONTROL_HANDLE(IProtocolParseXygsServerImpl_Xyhc_Control)

