#include "UAVProtocol.h"

UAVProtocol::UAVProtocol()
{

}

void UAVProtocol::SendCommandBytes(const uchar protoclID, const uchar protocolSubid, const uchar *msg, const uint length, const QHostAddress &hostAddress, const quint16 port, const quint16 localPort)
{
    uint frameLength = FrameLength + length;
    if (frameLength > FrameMaxLength)
    {
        return ;
    }

    unsigned char buf[FrameMaxLength] = { 0 };
    buf[FrameHeader1Order]            = FrameHeader1;
    buf[FrameHeader2Order]            = FrameHeader2;
    buf[FrameLengthOrder]             = frameLength;
    buf[ProtocolByteOrder]            = protoclID;
    buf[ProtocolSubByteOrder]         = protocolSubid;

    memcpy(&buf[FrameItemOrderStart], msg, length);

    uint cRCOrder_Start               = CRCOrder_Start + length - 3;
    uint cRCOrder_End                 = CRCOrder_End   + length - 2;

    ushort cRC16                      = Crc16Bytes(buf, frameLength);
    buf[cRCOrder_Start]               = (reinterpret_cast<unsigned char *>(&cRC16))[0];
    buf[cRCOrder_End]                 = (reinterpret_cast<unsigned char *>(&cRC16))[1];

    uint frameEndOrder                = FrameEndOrder   + length - 1;
    buf[frameEndOrder]                = FrameEnd;

    static bool connectToHost = false;
    if (!connectToHost)
    {
        udpSocket.bind(localPort);
        connectToHost = true;
    }

    udpSocket.writeDatagram(reinterpret_cast<const char *>(buf), frameLength, hostAddress, port);
}

ushort UAVProtocol::Crc16(unsigned char *buf, unsigned int length)
{
    ushort num3 = 0;
    for (unsigned int i = 0; i < length; i++)
    {
        num3 = (ushort)((num3 << 8) ^ crc16tab[((num3 >> 8) ^ buf[i]) & 0xff]);
    }
    unsigned char *bytes = reinterpret_cast<unsigned char *>(&num3);
    HighByte = bytes[1];
    LowByte = bytes[0];
    return num3;
}

ushort UAVProtocol::Crc16Bytes(unsigned char *buf, unsigned int length)
{
    unsigned int num2 = length - 5;
    ushort num3 = 0;
    for (unsigned int i = 0; i < num2; i++)
    {
        num3 = (ushort)((num3 << 8) ^ crc16tab[((num3 >> 8) ^ buf[2 + i]) & 0xff]);
    }
    unsigned char *bytes = reinterpret_cast<unsigned char *>(&num3);
    HighByte = bytes[1];
    LowByte = bytes[0];
    return num3;
}

ushort UAVProtocol::Crc16Bytes(unsigned char *buf, unsigned int length, unsigned int startIndex)
{
    unsigned int num2 = (length - startIndex) - 3;
    ushort num3 = 0;
    for (unsigned int i = 0; i < num2; i++)
    {
        num3 = (ushort)((num3 << 8) ^ crc16tab[((num3 >> 8) ^ buf[startIndex + i]) & 0xff]);
    }
    unsigned char *bytes = reinterpret_cast<unsigned char *>(&num3);
    HighByte = bytes[1];
    LowByte = bytes[0];
    return num3;
}
