#include "TrackMgr.h"
#include <float.h>
#include <QAction>
#include <QCheckBox>
#include <QDateTime>
#include <QFile>
#include <QFontDatabase>
#include <QPainter>
#include <QSettings>
#include <QTableWidget>
#include <algorithm>
#include <IProj.h>
#include <QCString.h>
#include <LyMessageBox.h>
#include <IFunctionInterface.h>
#include <CreateShape.h>
#include <map3d/IEarth.h>
#include <map3d/ITrack.h>

#include "lyXyhc.h"
#include "IXyhcActivator.h"
#include "lyXyhcInterface.h"
#include "QDb.h"
#include "track.h"
#include "trackList.h"
#include <QCString.h>

#define PI 3.14159265358979323846

#define XYGSJL_LOG_DIR        "../data/Record/Record_UAVFlightLog/"
#define XYGSJL_LOG_HEADER     "Pitch,Roll,Gaodu,Sudu,Hangxiang,Lon,Lat"
CLogRecordCVS    CTrackMgr::s_TractorCVS(XYGSJL_LOG_HEADER, XYGSJL_LOG_DIR, "TractorServer");

void SendManualCommand(int nID, int nSubType, int nCmd, bool bNo = true);
IUserMesh2 *CreateSightingLine(unsigned int l, int a, int b = 0);
IUserMesh2 *CreateParabola(float v, int h, float angle = 0);
IUserMesh2 *CreateParabola2(float v, int h, float angle = 0);

#ifdef USING_LY_CCIP_80MM_DEFINE
IUserMesh1 *UpdateUavMarkTrack_80mm(IUserMesh1 *pUM, const double dLongitudeUav, const double dLatitudeUav, const double dAltitudeUav,
                                      const double dLongitudeMark, const double dLatitudeMark, const double dAltitudeMark);

IUserMesh2 *CreateParabola_80mm(const double dLongitude, const double dLatitude, const double dAltitude,
                                const double dVehicleSpeed, const double dRelAltitude, const double dPitchAngle,
                                double &x, double &y, double &z);
IUserMesh2 *UpdateParabola_80mm(IUserMesh2 *pUM, const double dLongitude, const double dLatitude, const double dAltitude,
                                const double dVehicleSpeed, const double dRelAltitude, const double dPitchAngle,
                                double &x, double &y, double &z);

IUserMesh2 *CreateParabola2_80mm(const double x, const double y, const double z);

IUserMesh2 *UpdateParabola2_80mm(IUserMesh2 *pUM, const double x, const double y, const double z);

#endif

struct Uav3D
{
    unsigned int Uav3DMarkTrackShowTimes;
    ITrack		*pTrack;
    ILabel		*pLabel;
    IUserMesh2	*pSightingLine;
    IUserMesh2	*pSightingLine2;
    IUserMesh2	*pBombTrack;
    IUserMesh2	*pBombTrack2;
    IUserMesh1	*pUavMarkTrack;
    QColor       colorSightingLine;
    std::string	strLabel;
};

std::map<int, Uav3D> g_mapUav;

static constexpr unsigned int IFF_Number = 6;
static constexpr unsigned int Type_Number = 8;
static constexpr unsigned int Type_Font_Width = 40;

QRgb   colorIFF[IFF_Number] = { 0xFF2090FF,                  0xFFFF0000,                   0xFFFFFF00,  0xFFFFFFFF,  0xFFFFFFFF,  0xFFFFFFFF };

QImage imageType[Type_Number] = { QImage("../data/vehile.png"), QImage("../data/soldier.png"), QImage("../data/tank.png"),
                                  QImage("../data/tank.png"),   QImage("../data/unknow.png"),  QImage("../data/unknow.png"),
                                  QImage("../data/unknow.png"), QImage("../data/unknow.png")
                                };
QImage imageTypeList[IFF_Number][Type_Number];

bool     loadFont = false;
QFont    fontType;

QString  fontTypeList[Type_Number];

static bool g_MarkVehicleCalShowDefault = true;
static bool g_MarkVehicleCalLabelShowDefault = false;
static bool g_MarkVehicleCalLabelShow = false;
static bool g_SensorViewShowDefault = true;

#define USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
#include "lyXyhcInterface.h"
#include "DataManage/SystemInfo/SystemInfo.h"
void SendMsgToAll_Command(const QString &strType, const QString &strInfo, bool recvSeatFromServer = true)
{
    qnzkna::DataManageSystemInfomation::SystemInfomation::SystemInfo info;
    info.id = qnzkna::DataManageSystemInfomation::SystemInfomation::SystemInfo_ID_Enum::SystemInfo_ID_Intervention_Command;
    info.time = 0;
    if (recvSeatFromServer)
    {
        info.send = QString(QObject::tr("Server"));
        info.recv = QString(QObject::tr("Seat"));
    }
    else
    {
        info.send = QString(QObject::tr("Seat"));
        info.recv = QString(QObject::tr("Server"));
    }
    info.type = strType;
    info.info = strInfo;
    LyXyhcInterface::Get().SendMsgToAll(IInterface::MSG_Command, QNZKNA_DATAMANAGESYSTEMINFOMATION_SYSTEMINFO_CONTROL_ID, &info);
}
#endif

CTrackMgr::CTrackMgr()
    : m_XygsZC_02_Process()
{
    m_localPilot = LyXyhcInterface::Get().GetSettings().value("User/Pilot").toInt();

    QSettings settingsMarkVehicleCal("../data/conf/AutoAiming.ini", QSettings::IniFormat);
    settingsMarkVehicleCal.beginGroup("AutoAimingShowSettings");

    if (settingsMarkVehicleCal.contains("MarkVehicleCalShowDefault"))
    {
        const QVariant targetVehicleCalShowDefaultTemp = settingsMarkVehicleCal.value("MarkVehicleCalShowDefault", false);
        if (targetVehicleCalShowDefaultTemp.canConvert(QMetaType::Bool))
        {
            g_MarkVehicleCalShowDefault = targetVehicleCalShowDefaultTemp.toBool();
        }
    }

    if (settingsMarkVehicleCal.contains("MarkVehicleCalLabelShowDefault"))
    {
        const QVariant targetVehicleCalLabelShowDefaultTemp = settingsMarkVehicleCal.value("MarkVehicleCalLabelShowDefault", false);
        if (targetVehicleCalLabelShowDefaultTemp.canConvert(QMetaType::Bool))
        {
            g_MarkVehicleCalLabelShowDefault = targetVehicleCalLabelShowDefaultTemp.toBool();
        }
    }

    if (settingsMarkVehicleCal.contains("MarkVehicleCalLabelShow"))
    {
        const QVariant targetVehicleCalLabelShowTemp = settingsMarkVehicleCal.value("MarkVehicleCalLabelShow", false);
        if (targetVehicleCalLabelShowTemp.canConvert(QMetaType::Bool))
        {
            g_MarkVehicleCalLabelShow = targetVehicleCalLabelShowTemp.toBool();
        }
    }

    settingsMarkVehicleCal.endGroup();

    QSettings settingsSensorView("../data/conf/SensorView.ini", QSettings::IniFormat);
    settingsSensorView.beginGroup("SensorViewShowSettings");

    if (settingsSensorView.contains("SensorViewShowDefault"))
    {
        const QVariant sensorViewShowDefaultTemp = settingsSensorView.value("SensorViewShowDefault", false);
        if (sensorViewShowDefaultTemp.canConvert(QMetaType::Bool))
        {
            g_SensorViewShowDefault = sensorViewShowDefaultTemp.toBool();
        }
    }

    settingsSensorView.endGroup();

    m_WeiBiaoViewShowStringListMap.clear();

    m_pTelemetryWidgetShow = true;

    ReadWeiBiaoViewShowConfigureINI();

    m_pTelemetry = new QTableWidget;
    QStringList sl;
    sl << QObject::tr("time") << QObject::tr("station number") << QObject::tr("batch number") << QObject::tr("longitude") << QObject::tr("latitude") << QObject::tr("height");
    m_pTelemetry->setColumnCount(sl.size());
    m_pTelemetry->setHorizontalHeaderLabels(sl);

    if (m_pTelemetry)
    {
        m_pTelemetryWidget = new QWidget;

        QHBoxLayout *layoutH = new QHBoxLayout;

        QCheckBox   *buttonRefresh = new QCheckBox(QObject::tr("refresh"));
        QCheckBox::connect(buttonRefresh, &QCheckBox::stateChanged, [this](int state)
        {
            this->m_pTelemetryWidgetShow = (state == Qt::CheckState::Checked);
            SaveWeiBiaoViewShowConfigureINI();
        });
        buttonRefresh->setChecked(m_pTelemetryWidgetShow);

        m_pTelemetryWidgetShowDegree = false;
        QCheckBox   *buttonDegree = new QCheckBox(QObject::tr("decimal degree"));
        QCheckBox::connect(buttonDegree, &QCheckBox::stateChanged, [this](int state)
        {
            this->m_pTelemetryWidgetShowDegree = (state == Qt::CheckState::Checked);
        });
        buttonDegree->setChecked(m_pTelemetryWidgetShowDegree);

        QPushButton *buttonClear = new QPushButton(QObject::tr("empty"));
        QPushButton::connect(buttonClear, &QPushButton::clicked, [this]()
        {
            if (this->m_pTelemetry)
            {
                this->m_pTelemetry->clearContents();
            }
        });

        layoutH->addWidget(buttonRefresh);
        layoutH->addStretch();
        layoutH->addWidget(buttonDegree);
        layoutH->addStretch();
        layoutH->addWidget(buttonClear);

        layoutH->addStretch();

        QVBoxLayout *layoutV = new QVBoxLayout;
        layoutV->addLayout(layoutH);
        layoutV->addWidget(m_pTelemetry);

        m_pTelemetryWidget->setLayout(layoutV);
    }

    m_TrackList = new TrackList;

    auto imageFill = [](const QImage & imageIn, QRgb color)
    {
        QImage outImage(imageIn.width(), imageIn.height(), QImage::Format_RGBA8888);
        for (int x = 0; x < outImage.width(); x++)
        {
            for (int y = 0; y < outImage.height(); y++)
            {
                QColor colorTemp(QColor::fromRgba(color));
                colorTemp.setAlpha(imageIn.pixelColor(x, y).alpha());
                outImage.setPixelColor(x, y, colorTemp);
            }
        }
        return outImage;
    };

    for (int IFF = 1; IFF <= IFF_Number; IFF++)
    {
        for (int type = 1; type <= Type_Number; type++)
        {
            imageTypeList[IFF - 1][type - 1] = imageFill(imageType[type - 1], colorIFF[IFF - 1]);
        }
    }

    const int fontID = QFontDatabase::addApplicationFont("../data/font/Esri_icons2.ttf");
    if (fontID >= 0)
    {
        QStringList fontList = QFontDatabase::applicationFontFamilies(fontID);
        if (fontList.size() > 0)
        {
            fontType = QFont(fontList.at(0));

            fontType.setPixelSize(Type_Font_Width);
            loadFont = true;

            fontTypeList[0] = QChar(0x81);
            fontTypeList[1] = QChar(0xC3);
            fontTypeList[2] = QChar(0xCB);
            fontTypeList[3] = QChar(0xCA);
            fontTypeList[4] = QChar(0xC5);
            fontTypeList[5] = QChar(0xC6);
            fontTypeList[6] = QChar(0x09);
            fontTypeList[7] = QChar(0x09);
        }
    }

    m_TrackName.clear();

	udpSocket = new SocketUtil(SocketUtil::UDP_SOCKET, this);
	udpSocket->bindUdp(10080, QHostAddress("192.168.1.30"));

	connect(udpSocket, &SocketUtil::dataReceived, this, [this](const QByteArray &data, const QString &addr, quint16 port) {

		qDebug() << "receive data==" << m_udpAddress << m_udpPort;
		m_udpAddress = addr;
		m_udpPort = port;
	
	});


}

CTrackMgr::~CTrackMgr()
{
}

void CTrackMgr::SaveWeiBiaoViewShowConfigureINITrackOffset(QTrack *pTrack)
{

    {
        int index = m_WeiBiaoViewShowStringListMap["WeiBiaoViewOffSetPosition"].indexOf(QRegExp(QString::number(pTrack->unNodeID) + ":.*"));
        if (index >= 0)
        {
            m_WeiBiaoViewShowStringListMap["WeiBiaoViewOffSetPosition"].removeAt(index);
        }

        m_WeiBiaoViewShowStringListMap["WeiBiaoViewOffSetPosition"].append(QString::number(pTrack->unNodeID) + ":" + QString::number(pTrack->m_nOffsetX) + ":" + QString::number(pTrack->m_nOffsetY));

        SaveWeiBiaoViewShowConfigureINI();
    }
}

const QTrack *CTrackMgr::FindTrackNodeID(const unsigned long long &unNodeID)
{
    TrackMap::const_iterator it = std::find_if(m_Track.begin(), m_Track.end(), [&unNodeID](const auto & trackData)
    {
        if (trackData.second.unNodeID == unNodeID)
        {
            return true;
        }
        return false;
    });
    if (it != m_Track.end())
    {
        return &it->second;
    }
    return nullptr;
}

bool CTrackMgr::UpdateAutoAimingState(const unsigned long long unNodeID, const unsigned int autoAimingState)
{
    TrackMap::iterator it = std::find_if(m_Track.begin(), m_Track.end(), [&unNodeID](const auto & trackData)
    {
        if (trackData.second.unNodeID == unNodeID)
        {
            return true;
        }
        return false;
    });
    if (it != m_Track.end())
    {
        it->second.m_AutoAimingState = autoAimingState;
        return true;
    }
    return false;
}

QTrack *CTrackMgr::FindTrack(const unsigned char ucZH, const unsigned char ucPH)
{
    int nID = (ucZH << 8) | ucPH;
    TrackMap::iterator it = m_Track.find(nID);
    if (it != m_Track.end())
    {
        return &it->second;
    }
    return nullptr;
}

double CTrackMgr::HeadAngle(const double angle) const
{

    double headingAngle = angle;
    if (qFuzzyIsNull(std::fmod(std::abs(angle), 360)))
    {
        headingAngle = 0;
    }
    else if (angle >= 360)
    {
        headingAngle = std::fmod(std::abs(angle), 360);
    }
    else if (angle < 0)
    {
        headingAngle = 360 - std::fmod(std::abs(angle), 360);
    }
    return headingAngle;
}

inline int GetColorCode(int nIFF)
{
    switch (nIFF)
    {
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 10;
    case 4:
        return 4;
    case 5:
        return 5;
    case 6:
        return 1;
    default:
        return 0;
    }
}

QColor StringColor(const QString &color)
{
    int nR = 0;
    int nG = 0;
    int nB = 0;
    sscanf(color.toLatin1().data(), "%d %d %d", &nR, &nG, &nB);
    QColor clr(nR, nG, nB);
    return clr;
}

QColor GetColor(char colorIndex)
{
    static QColor color[16] =
    {
        QColor(255, 255, 255), QColor(128, 128, 128), QColor(32, 144, 255), QColor(255, 0, 0),
        QColor(255, 128, 64), QColor(0, 255, 0), QColor(255, 128, 128), QColor(0, 255, 255),
        QColor(255, 64, 128), QColor(255, 255, 0), QColor(255, 0, 255), QColor(128, 255, 64),
        QColor(64, 255, 128), QColor(128, 64, 255), QColor(128, 255, 128), QColor(128, 128, 255)
    };

    static bool bRead = false;
    if (!bRead)
    {
        bRead = true;
        QString strIni = LyXyhc::Get().GetConfDir();
        strIni += "ly.ini";
        QFile file(strIni);
        if (file.exists())
        {
            QSettings ini(strIni, QSettings::IniFormat);
            ini.beginGroup("Colors");
            char szT[16];
            for (int i = 0; i < 16; i++)
            {
                sprintf(szT, "Color%d", i);
                color[i] = StringColor(ini.value(szT, "0 0 0").toString());
            }
        }
    }

    return color[colorIndex % sizeof(color)];
}

QTrack *CTrackMgr::GetTrack(unsigned char ucZH, unsigned char ucPH)
{
    int nID = (ucZH << 8) | ucPH;
    return &m_Track[nID];
}

QTrack *CTrackMgr::GetTrack(unsigned char ucZH, unsigned char ucPH, unsigned char unNodeType, unsigned long long unNodeID, TrackInfo *pD)
{
    int nID = (ucZH << 8) | ucPH;
    QTrack *track = &m_Track[nID];
    if (track->unNodeType == 0 && track->unNodeID == 0)
    {
        track->unNodeType = unNodeType;
        track->unNodeID = unNodeID;
        track->unNodeNameSimple[0] = '\0';
        const auto iter = m_TrackName.find(unNodeID);
        if (iter != m_TrackName.end())
        {
            strncpy(track->unNodeNameSimple,
                    iter->second.c_str(), sizeof(track->unNodeNameSimple) - 1);

            track->unNodeNameSimple[sizeof(track->unNodeNameSimple) - 1] = '\0';
        }

        if (iter == m_TrackName.end())
        {
            const std::string &unNodeNameSimple = QString::number(track->unNodeID).toStdString();
            strncpy(track->unNodeNameSimple, unNodeNameSimple.c_str(), sizeof(track->unNodeNameSimple) - 1);

            track->unNodeNameSimple[sizeof(track->unNodeNameSimple) - 1] = '\0';
        }

        if (pD->cmn.IFF_Second)
        {
            unsigned char iff = pD->cmn.IFF_Second;
            if (track->m_status.UserColor == 0 && (iff <= IFF_Number && iff > 0))
            {
                track->m_Color = colorIFF[iff - 1];
                ChangeUav3DColor(pD->cmn.Z, pD->cmn.P, track->m_Color);
            }
        }
        else
        {
            unsigned char iff = pD->cmn.IFF_First;
            if (track->m_status.UserColor == 0 && (iff <= IFF_Number && iff > 0))
            {
                track->m_Color = colorIFF[iff - 1];
                ChangeUav3DColor(pD->cmn.Z, pD->cmn.P, track->m_Color);
            }
        }

        track->m_MarkVehicleCalShow = g_MarkVehicleCalShowDefault;
        track->m_MarkVehicleCalLabelShow = g_MarkVehicleCalLabelShowDefault;
        track->SensorViewShow = g_SensorViewShowDefault;

        const QString strTrackNodeID(QString::number(track->unNodeID));
        if (!strTrackNodeID.isEmpty())
        {
            if (m_WeiBiaoViewShowStringListMap["WeiBiaoViewShow"].contains(strTrackNodeID))
            {
                track->m_status.WeiBiao = 1;
            }

            int index = m_WeiBiaoViewShowStringListMap["WeiBiaoViewOffSetPosition"].indexOf(QRegExp(strTrackNodeID + ":.*"));
            if (index >= 0)
            {
                const QStringList strWeiBiaoViewOffSetPosition = m_WeiBiaoViewShowStringListMap["WeiBiaoViewOffSetPosition"].at(index).split(":");
                if (strWeiBiaoViewOffSetPosition.size() == 3 && strTrackNodeID == strWeiBiaoViewOffSetPosition.at(0))
                {
                    bool ok = false;
                    const int weiBiaoViewOffSetPositionX = strWeiBiaoViewOffSetPosition.at(1).toInt(&ok);
                    const int weiBiaoViewOffSetPositionY = strWeiBiaoViewOffSetPosition.at(2).toInt(&ok);
                    if (weiBiaoViewOffSetPositionX <= 127 && weiBiaoViewOffSetPositionX >= -128 &&
                            weiBiaoViewOffSetPositionY <= 127 && weiBiaoViewOffSetPositionY >= -128)
                    {
                        track->m_nOffsetX = weiBiaoViewOffSetPositionX;
                        track->m_nOffsetY = weiBiaoViewOffSetPositionY;
                    }
                }
            }

            if (m_WeiBiaoViewShowStringListMap["SensorViewHide"].contains(strTrackNodeID))
            {
                track->SensorViewShow = 0;
            }
        }
    }
    if (111000 < pD->header.unNodeID && pD->header.unNodeID < 111010)
    {
        track->unNodeType = 1;
        track->unUnuse1 = pD->cmn.Speed;
        track->unUnuse2 = pD->cmn.Course;
        track->unUnuse3 = pD->cmn.Pitch;
        track->SensorUnuse2 = pD->cmn.Roll;
    }

    return track;
}

int CTrackMgr::GetTrackID(QTrack *pTrack)
{
    for (TrackMap::iterator it = m_Track.begin(); it != m_Track.end(); it++)
    {
        if (&it->second == pTrack)
        {
            return it->first;
        }
    }
    return 0;
}

int CTrackMgr::IfZhNeedProcess(unsigned char ucZhanHao)
{
    return 1;
}

int CTrackMgr::IfZhDisplay(unsigned char ucZhanHao)
{
    return 1;
}

void CTrackMgr::ChangeUav3DColor(unsigned int Z, unsigned int P, const QColor &color)
{
    const unsigned int zp = (Z << 8 | P);
    const int zp_mapUav = static_cast<int>(zp);
    const auto iterMapUav = g_mapUav.find(zp_mapUav);
    if (iterMapUav == g_mapUav.end())
    {
        return;
    }

    const Uav3D &uav3D = iterMapUav->second;

    QColor colorBombTrack(color);
    colorBombTrack.setAlpha(255);
    QColor colorTemp;
    colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorBombTrack.alpha());

    if (uav3D.colorSightingLine != color)
    {
        iterMapUav->second.colorSightingLine = color;

        if (uav3D.pBombTrack != nullptr)
        {
            uav3D.pBombTrack->SetColor(colorTemp.rgba());
        }

        if (uav3D.pSightingLine != nullptr && uav3D.pTrack != nullptr)
        {
            QColor colorSightingLine(color);
            colorSightingLine.setAlpha(40);

            QColor colorTemp;
            colorTemp.setRgb(colorSightingLine.blue(), colorSightingLine.green(), colorSightingLine.red(), colorSightingLine.alpha());
            uav3D.pSightingLine->SetColor(colorTemp.rgba());
            colorSightingLine.setAlpha(80);
            colorTemp.setRgb(colorSightingLine.blue(), colorSightingLine.green(), colorSightingLine.red(), colorSightingLine.alpha());
            uav3D.pSightingLine2->SetColor(colorTemp.rgba());

            colorSightingLine.setAlpha(255);
            colorTemp.setRgb(colorSightingLine.blue(), colorSightingLine.green(), colorSightingLine.red(), colorSightingLine.alpha());

            uav3D.pTrack->SetColor(uav3D.pTrack->GetType(), colorTemp.rgba());
        }
    }
}

std::vector<IUserMesh2*> CTrackMgr::CreateDetectionZone(float angle_l, float angle_w, float dis_m)
{
	vector<IUserMesh2*> reUserMesh2s;
	reUserMesh2s.clear();
	IUserMesh2 *pUM = nullptr;
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh2();
	}

	int angle_w_count = (int)angle_w;
	int angle_l_count = (int)(angle_l * 2);

	if (angle_w_count == 0)
		angle_w_count = 1;
	if (angle_l_count == 0)
		angle_l_count = 1;

	float angle_w_dis = angle_w / angle_w_count;
	float angle_l_dis = angle_l / angle_l_count;

	vector<unsigned short> pIndex_up;
	vector<unsigned short> pIndex_down;
	vector<unsigned short> pIndex_left;
	vector<unsigned short> pIndex_r;
	vector<vector<unsigned short>> pIndex_cir;
	pIndex_up.clear();
	pIndex_down.clear();
	pIndex_left.clear();
	pIndex_r.clear();
	pIndex_cir.clear();

	pIndex_up.push_back(0);
	pIndex_down.push_back(0);
	pIndex_left.push_back(0);
	pIndex_r.push_back(0);

	VERTEX *pVerter = (VERTEX*)malloc(((angle_l_count + 1) * (angle_w_count + 1) + 1) * sizeof(VERTEX));
	if (pVerter != nullptr)
	{
		int pVIndex = 0;
		float angle_w_cur = -(angle_w / 2);
		pVerter[pVIndex].x = 0;
		pVerter[pVIndex].y = 0;
		pVerter[pVIndex].z = 0;
		pVIndex++;
		for (int angle_w_index = 0; angle_w_index <= angle_w_count; angle_w_index++)
		{
			double angle_w_cur_pi = angle_w_cur*PI / 180;
			angle_w_cur += angle_w_dis;
			double z = dis_m * sin(angle_w_cur_pi);
			double dis_z = dis_m * cos(angle_w_cur_pi);
			float angle_l_cur = -(angle_l / 2);

			for (int angle_l_index = 0; angle_l_index <= angle_l_count; angle_l_index++)
			{
				double angle_l_cur_pi = angle_l_cur * PI / 180;
				angle_l_cur += angle_l_dis;
				double x = dis_z * sin(angle_l_cur_pi);
				double y = dis_z * cos(angle_l_cur_pi);

				pVerter[pVIndex].x = x;
				pVerter[pVIndex].y = y;
				pVerter[pVIndex].z = z;

				if (angle_w_index == 0)
				{
					pIndex_up.push_back(pVIndex);
				}
				else if (angle_w_index == angle_w_count)
				{
					pIndex_down.push_back(pVIndex);
				}

				if (angle_l_index == 0)
				{
					pIndex_left.push_back(pVIndex);
				}
				else if (angle_l_index == angle_l_count)
				{
					pIndex_r.push_back(pVIndex);
				}

				pVIndex++;
			}
		}

		int startIndex = 1;
		int index_added = angle_l_count + 1;
		for (int wIndex = 0; wIndex < angle_w_count; wIndex++)
		{
			int startWIndex = startIndex + wIndex * index_added;
			vector<unsigned short> tempPindex_cir;
			tempPindex_cir.clear();
			tempPindex_cir.push_back(startWIndex);
			tempPindex_cir.push_back(startWIndex + index_added);
			for (int lIndex = 1; lIndex < index_added; lIndex++)
			{
				tempPindex_cir.push_back(startWIndex + lIndex);
				tempPindex_cir.push_back(startWIndex + lIndex + index_added);
			}
			pIndex_cir.push_back(tempPindex_cir);
		}

		pUM->SetPoints(pVIndex, pVerter);
		pUM->SetIndexs(pIndex_up.size(), &pIndex_up[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_down.size(), &pIndex_down[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_left.size(), &pIndex_left[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_r.size(), &pIndex_r[0], IUserMesh::USERMESH_POLYGON, false);
		for (int iindex = 0; iindex < pIndex_cir.size(); iindex++)
		{
			if (iindex != pIndex_cir.size() - 1)
			{
				pUM->SetIndexs(pIndex_cir[iindex].size(), &pIndex_cir[iindex][0], IUserMesh::USERMESH_QUAD_STRIP, false);
			}
			else
			{
				pUM->SetIndexs(pIndex_cir[iindex].size(), &pIndex_cir[iindex][0], IUserMesh::USERMESH_QUAD_STRIP);
			}
		}

		pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
		pUM->SetState(IUserMesh::LINEWIDTH, 3);
		reUserMesh2s.push_back(pUM);

		pUM = pEarth->GetGraphics()->CreateUserMesh2();
		pUM->SetPoints(pVIndex, pVerter);
		pUM->SetIndexs(pIndex_up.size(), &pIndex_up[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_down.size(), &pIndex_down[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_left.size(), &pIndex_left[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_r.size(), &pIndex_r[0], IUserMesh::USERMESH_POLYGON);
		free(pVerter);
		pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
		pUM->SetState(IUserMesh::LINEWIDTH, 2);
		pUM->SetState(IUserMesh::LIGHT, 1);
		reUserMesh2s.push_back(pUM);

	}
	return reUserMesh2s;
}

void DrawWeiBiao(QPainter &p, const QPoint &pt, const QTrack &track)
{
	QString strT;

	QRectF rectangle(pt.x(), pt.y(), 10, 10);

    {
        if (track.unNodeNameSimple[0] != '\0')
        {
            strT += QObject::tr("Name:");
            strT += QLatin1String(track.unNodeNameSimple);

        }

        if (track.getType())
        {
            strT += "\n";

			strT += tr2("类型");
            strT += QObject::tr(":");
            if ((track.getType() - 1) >= 0 && (track.getType() - 1) < LyXyhc::Get().m_vslAttributeItem[0].size())
            {
                strT += LyXyhc::Get().m_vslAttributeItem[0][track.getType() - 1];
            }

        }
        if (track.getModel())
        {
            strT += "\n";

			strT += tr2("型号");
            strT += QObject::tr(":");
            if ((track.getModel() - 1) >= 0 && (track.getModel() - 1) < LyXyhc::Get().m_vslAttributeItem[1].size())
            {
                strT += LyXyhc::Get().m_vslAttributeItem[1][track.getModel() - 1];
            }

        }

        if (track.m_nMarkID)
        {
            strT += "\n";
            strT += LyXyhc::Get().m_slAttributeTitle[9];
            strT += QObject::tr(":");

            if (((track.m_nMarkID & 0xFF) - 1) < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size()
                    && IXyhcActivator::Get().getPIMarkManageService() != nullptr)
            {
                const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
                if (pIMarkManageService != nullptr)
                {
                    const QString targetID = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][((track.m_nMarkID & 0xFF) - 1)];
                    const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                    if (!targetItem.GetID().empty())
                    {
                        strT += targetID;
                    }
                }
            }

        }

    }

    rectangle = p.QPainter::boundingRect(rectangle, Qt::AlignLeft | Qt::AlignTop, strT);

    QColor color = p.pen().color();
    color.setAlpha(30);
    p.fillRect(rectangle.x() - 2, rectangle.y() - 14, rectangle.width() + 4, rectangle.height() + 16, QBrush(color));
    p.drawText(rectangle, Qt::AlignLeft | Qt::AlignTop, strT);

}

std::string GetWeiBiao(const QTrack &track)
{
    QString strT;

    if (track.unNodeNameSimple[0] != '\0')
    {
        strT += QObject::tr("\nName:");
        strT += QLatin1String(track.unNodeNameSimple);
    }

    if (track.getType())
    {
        strT += QObject::tr("\n");

		strT += tr2("类型");
        strT += QObject::tr(":");
        if ((track.getType() - 1) >= 0 && (track.getType() - 1) < LyXyhc::Get().m_vslAttributeItem[0].size())
        {
            strT += LyXyhc::Get().m_vslAttributeItem[0][track.getType() - 1];
        }
    }

    if (track.getModel())
    {
        strT += QObject::tr("\n");

		strT += tr2("型号");
        strT += QObject::tr(":");
        if ((track.getModel() - 1) >= 0 && (track.getModel() - 1) < LyXyhc::Get().m_vslAttributeItem[1].size())
        {
            strT += LyXyhc::Get().m_vslAttributeItem[1][track.getModel() - 1];
        }
    }

    if (track.m_nMarkID)
    {
        strT += QObject::tr("\n");
        strT += LyXyhc::Get().m_slAttributeTitle[9];
        strT += QObject::tr(":");

        if (((track.m_nMarkID & 0xFF) - 1) < LyXyhc::Get().m_vslAttributeItem[9].size()
                && IXyhcActivator::Get().getPIMarkManageService() != nullptr)
        {
            const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
            if (pIMarkManageService != nullptr)
            {
                const QString targetID = LyXyhc::Get().m_vslAttributeItem[9][((track.m_nMarkID & 0xFF) - 1)];
                const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                if (!targetItem.GetID().empty())
                {
                    strT += targetID;
                }
            }
        }

    }

	strT += "\n";
    if (strT.length())
    {
        return tr3(strT);
    }
    else
    {
        return "";
    }
}

void CTrackMgr::Draw(QPainter &painter, IProj *pProj)
{
    int nRole = LyXyhcInterface::Get().GetSettings().value("User/Seat").toInt();
    for (TrackMap::iterator it = m_Track.begin(); it != m_Track.end(); it++)
    {
        const QTrack &track = it->second;
		//track.Pitch, track.Roll, track.m_nGaoDu, track.m_nSuDu, track.m_nHangXiang, track.m_Latitude, track.m_Longitude

		QJsonObject json;
		json["Pitch"] = track.Pitch;
		json["Roll"] = track.Roll;
		json["m_nGaoDu"] = track.m_nGaoDu;
		json["m_nSuDu"] = track.m_nSuDu;
		json["m_nHangXiang"] = track.m_nHangXiang;
		json["m_Latitude"] = track.m_Latitude;
		json["m_Longitude"] = track.m_Longitude;

		QByteArray data1 = QJsonDocument(json).toJson(QJsonDocument::Compact);
		udpSocket->sendData(data1, m_udpAddress, m_udpPort);

        if (!IfZhDisplay((it->first >> 8) & 0xFF))
        {
            continue;
        }

        if (track.m_status.XiaoShi & 0x01)
        {
            continue;
        }
        if (track.NoReport && nRole != 2)
        {
            continue;
        }

        QColor color = (track.m_Color);
        QBrush brush(color, Qt::SolidPattern);
        painter.setPen(color);
        painter.setBrush(brush);

        TrackPtList &tpl = it->second.m_lsPoints;
        TrackPtList::reverse_iterator it1 = tpl.rbegin();
        QPolygon poly;
        if (it1 != tpl.rend())
        {
            char szBuf[16];
            QPoint pt2;
            QPoint pt = pProj->Map2Screen(it1->dX, it1->dY);

            if (track.SensorViewShow == 1)
            {

                static int snLength2 = 0;
                static int snC = 20;
                static int snD = 20;
                if (snLength2 == 0)
                {
                    QString strIni = LyXyhc::Get().GetConfDir();
                    strIni += "ly.ini";
                    QFile file(strIni);
                    if (file.exists())
                    {
                        QSettings ini(strIni, QSettings::IniFormat);
                        ini.beginGroup("TrackMgr");
                        snLength2 = ini.value("ViewLength2", "1000").toInt();
                        snC = ini.value("ViewC", "20").toInt();
                        snD = ini.value("ViewD", "20").toInt();
                        ini.endGroup();
                    }
                }

                const double viewLength2 = pProj->Map2ScreenDistance(snLength2 / 2.0);
                const int    viewSensorYaw = (track.SensorYaw + track.m_nHangXiang - 90 + snC / 2);
                const int    startAngle = -viewSensorYaw * 16;
                const int    spanAngle = snC * 16;
                const QRectF rectangle(pt.x() - viewLength2 / 2, pt.y() - viewLength2 / 2, viewLength2, viewLength2);
                QColor viewColor(color);
                viewColor.setAlpha(20);
                painter.setBrush(QBrush(viewColor));
                painter.setPen(viewColor);
                painter.drawPie(rectangle, startAngle, spanAngle);
                painter.setBrush(Qt::BrushStyle::NoBrush);
                painter.setPen(color);

                painter.drawEllipse(pt.x() - 1, pt.y() - 1, 3, 3);
				//写入cvs
				{
					if (!s_TractorCVS.isopen()) {
						s_TractorCVS.open();
					}
					if (s_TractorCVS.isopen()) {

						const QTrack &track = it->second;
						
						char formatString[20 + MAX_WEAPONMODEL_ID_LENGTH + qnzkna::framework::MessageReferenceLength * 6] = { 0 };
						SYSTEMTIME systime;
						GetLocalTime(&systime);
//#define XYGSJL_LOG_HEADER     "Pitch,Roll,Gaodu,Sudu,Hangxiang,Lon,Lat"
						int len = snprintf(formatString, sizeof(formatString), "\n%f,%f,%f,%f,%f,%.7f,%.7f",
							track.Pitch, track.Roll, track.m_nGaoDu, track.m_nSuDu, track.m_nHangXiang, track.m_Latitude, track.m_Longitude
						);

						s_TractorCVS.write(formatString, len);
					}

				}



            }

            if ((track.getIFF() <= IFF_Number && track.getIFF() > 0) && (track.getType() < Type_Number && track.getType() > 0))
            {
                QImage image = imageTypeList[track.getIFF() - 1][track.getType() - 1];
                painter.drawImage(pt.x() - image.width(), pt.y() - image.height(), image.transformed(QMatrix().rotate(track.m_nHangXiang)));
            }
            else if (track.getIFF() == 0 && (track.getType() < Type_Number && track.getType() > 0))
            {
                QImage image = imageTypeList[IFF_Number - 1][track.getType() - 1];
                painter.drawImage(pt.x() - image.width(), pt.y() - image.height(), image.transformed(QMatrix().rotate(track.m_nHangXiang)));
            }

            if (it->second.m_nMarkID)
            {
                it->second.m_UavMarkTrackShowTimes++;
                if (it->second.m_UavMarkTrackShowTimes <= 3)
                {
                    double dMarkX = 0, dMarkY = 0;
                    pProj->jwxy(it->second.m_dMarkX, it->second.m_dMarkY, &dMarkX, &dMarkY);
                    pt2 = pProj->Map2Screen(dMarkX, dMarkY);
                    painter.drawEllipse(pt2.x() - 2, pt2.y() - 2, 5, 5);
                    painter.save();
                    painter.setPen(Qt::DashLine);
                    painter.drawLine(pt, pt2);
                    painter.restore();

                    if (it->second.m_nMarkAssign == 1)
                    {

                        if ((LyXyhc::Get().m_vslAttributeItem.size() - 2) >= 0
                                && ((LyXyhc::Get().m_vslAttributeItem.size() - 2) < LyXyhc::Get().m_vslAttributeItem.size())
                                && ((it->second.m_nMarkID & 0xFF) - 1 >= 0)
                                && ((it->second.m_nMarkID & 0xFF) - 1 < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size())
                           )
                        {
                            painter.drawText(pt2, LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][(it->second.m_nMarkID & 0xFF) - 1]);
                        }
                    }
                    else if (it->second.m_nMarkAssign == 2)
                    {
                        sprintf(szBuf, "%d.%d", (it->second.m_nMarkID >> 8) & 0xFF, it->second.m_nMarkID & 0xFF);
                        painter.drawText(pt2, QLatin1String(szBuf));
                    }
                }
                else
                {
                    it->second.m_UavMarkTrackShowTimes = 0;
                }
            }
            pt2 = pt;
            pt2.rx() += track.m_nOffsetX * OFFSET_RATE;
            pt2.ry() += track.m_nOffsetY * OFFSET_RATE;
            painter.drawLine(pt, pt2);

#ifndef USING_LY_CCIP_80MM_DEFINE

            pt2.ry() += 8;

            if (it->second.Pilot == m_localPilot)
            {
                sprintf(szBuf, " *%d", it->first & 0xFF);
            }
            else
            {
                sprintf(szBuf, " %d", it->first & 0xFF);
            }
            painter.drawText(pt2, QLatin1String(szBuf));

            if (track.m_status.WeiBiao)
            {
                DrawWeiBiao(painter, pt2, track);
            }

#elif defined USING_LY_CCIP_80MM_DEFINE

            if (track.m_MarkVehicleCalShow == 1 && (track.m_nMarkAssign == 1 || track.m_nMarkAssign == 2) && pProj != nullptr)
            {
                const double &dVehicleLongitude = track.m_Longitude;
                const double &dVehicleLatitude = track.m_Latitude;
                const double &dVehicleAltitudeTemp = track.m_nGaoDu;
                const double &dVehicleSpeedTemp = track.m_nSuDu;
                const double &dVehicleYawTemp = track.m_nHangXiang;
                const double &dVehiclePitchAngle = track.Pitch;
                const double &dMarkXTemp = track.m_dMarkX;
                const double &dMarkYTemp = track.m_dMarkY;
                const double &dMarkHTemp = track.m_dMarkH;

                double dVehicleMarkDistance = DBL_MAX, dVehicleMarkAngle = DBL_MAX;
                pProj->L_A(dVehicleLongitude, dVehicleLatitude, dMarkXTemp, dMarkYTemp, dVehicleMarkDistance, dVehicleMarkAngle);

                double dAltitude = dVehicleAltitudeTemp;
                if (dAltitude <= 0.000001 && dAltitude >= -0.000001)
                {
                    dAltitude = dAltitude + 0.0005;
                }

                const double dRelAltitude = (dAltitude - track.m_dMarkH) + 1;

                char dMarkVehicleCalTemp = 0;
                char dMarkVehicleDistShowTemp = 0;

                std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
                qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(dVehicleLongitude, dVehicleLatitude, dAltitude, dVehicleSpeedTemp, dRelAltitude, dVehiclePitchAngle, ccipdata_80mmlist);

                if (ccipdata_80mmlist.size() > 0)
                {
                    QPoint pt80MM = pt;
                    QPainterPath path;
                    path.moveTo(pt80MM);

                    bool  yawAngleSame = false;
                    double dDistance = DBL_MAX, dAngle = DBL_MAX;
                    double dMinDistance3D = DBL_MAX, dMinDistance = DBL_MAX, dMarkDistance = DBL_MAX;
                    double dHeight = DBL_MAX, dMinHeight = DBL_MAX, dMinYaw = DBL_MAX;
                    QPoint pt80MM_MIN = pt80MM;
                    double dJD = 0, dWD = 0, dX = 0, dY = 0;

                    for (const auto ccipdata : ccipdata_80mmlist)
                    {

                        pProj->A_L(dVehicleLongitude, dVehicleLatitude, ccipdata.x, dVehicleYawTemp, dJD, dWD);

                        pProj->L_A(dJD, dWD, dMarkXTemp, dMarkYTemp, dDistance, dAngle);

                        pProj->jwxy(dJD, dWD, &dX, &dY);
                        pt80MM = pProj->Map2Screen(dX, dY);
                        path.lineTo(pt80MM);

                        dHeight = (dAltitude + ccipdata.zd) - dMarkHTemp;

                        dMarkDistance = ((dHeight * dHeight) + (dDistance * dDistance));
                        if (dMarkDistance < dMinDistance3D && std::abs(dHeight) <= 0.5)
                        {

                            dMarkVehicleCalTemp = 1;
                            dMarkVehicleDistShowTemp = 1;

                            dMinDistance3D = dMarkDistance;

                            const double dVehicleMarkDifferenceYawTemp = dVehicleMarkAngle - dVehicleYawTemp;
                            yawAngleSame = (dVehicleMarkDifferenceYawTemp >= -90 && dVehicleMarkDifferenceYawTemp <= 90);
                            if (yawAngleSame && (dVehicleMarkDistance > dDistance))
                            {
                                dMinDistance = dDistance;
                            }
                            else
                            {
                                dMinDistance = -dDistance;
                            }

                            if (dVehicleMarkDifferenceYawTemp >= -180 && dVehicleMarkDifferenceYawTemp <= 180)
                            {
                                dMinYaw = dVehicleMarkDifferenceYawTemp;
                            }
                            else
                            {
                                dMinYaw = HeadAngle(dVehicleMarkDifferenceYawTemp);
                            }

                            dMinHeight = dHeight;
                            pt80MM_MIN = pt80MM;
                        }

                    }

                    {
                        const QPointF &currentPosition = path.currentPosition();
                        painter.save();

                        {

                            constexpr qreal space = 8;
                            const QVector<qreal> dashes(
                            {
                                4, space,
                                4, space,
                                4, space,
                                4, space,
                                5, space,
                                3, space,
                            });

                            QPen pen(QBrush(color), 1.5, Qt::CustomDashLine);
                            pen.setDashPattern(dashes);
                            pen.setDashOffset(4);
                            painter.setPen(pen);
                            painter.drawPath(path);
                        }

                        painter.setPen(QPen(QBrush(color), 1.5));

                        painter.drawPoint(currentPosition);
                        painter.restore();
                    }

                    {

                        it->second.m_nMarkVehicleDistShow = 0;

                        {
                            it->second.m_nMarkVehicleCal = dMarkVehicleCalTemp;
                            it->second.m_nMarkVehicleDistShow = dMarkVehicleDistShowTemp;
                            it->second.m_dMarkVehicleDist3D = std::sqrt(dMinDistance3D);
                            it->second.m_dMarkVehicleDist = dMinDistance;
                            it->second.m_dMarkVehicleHeight = dMinHeight;
                            it->second.m_dMarkVehicleYaw = dMinYaw;
                        }

                        if (yawAngleSame)
                        {

                            painter.save();
                            painter.setBrush(Qt::BrushStyle::NoBrush);
                            if ((dMinDistance < 25 && dMinDistance > -25) && (dMinHeight < 5 && dMinHeight > -5))
                            {
                                painter.setPen(QPen(QBrush(Qt::yellow), 0.5));
                            }
                            else
                            {
                                painter.setPen(QPen(QBrush(Qt::yellow), 0.5));
                            }

                            const long rx = pProj->Map2ScreenDistance(1.5);
                            painter.drawPoint(pt80MM_MIN);
                            painter.drawEllipse(pt80MM_MIN, rx, rx);
                            painter.restore();
                        }
                    }
                }

                pt2.ry() += 8;

                if (it->second.Pilot == m_localPilot)
                {
                    sprintf(szBuf, " *%d", it->first & 0xFF);
                }
                else
                {
                    sprintf(szBuf, " %d", it->first & 0xFF);
                }
                painter.drawText(pt2, QLatin1String(szBuf));

                if (track.m_status.WeiBiao)
                {
                    DrawWeiBiao(painter, pt2, track);
                }
            }
            else
            {
                pt2.ry() += 8;

                if (it->second.Pilot == m_localPilot)
                {
                    sprintf(szBuf, " *%d", it->first & 0xFF);
                }
                else
                {
                    sprintf(szBuf, " %d", it->first & 0xFF);
                }
                if (track.NoReport && nRole == 2)
                {
                    strcat(szBuf, "*");
                }
                painter.drawText(pt2, QLatin1String(szBuf));

                if (track.m_status.WeiBiao)
                {
                    DrawWeiBiao(painter, pt2, track);
                }
            }

#endif
            QPolygon poly;
            poly << pt;
            it1++;
            int nNum = 0;
            QTime tT;
            QTime tPre;
            for (; it1 != track.m_lsPoints.rend() && (track.m_status.QuanHangJi > 0 || nNum < track.m_nDisplayCount); it1++, nNum++)
            {
                pt = pProj->Map2Screen(it1->dX, it1->dY);
                painter.drawPoint(pt);
                int nA = 0;
                if (poly.size() > 0)
                {
                    nA = ((int)(atan2(pt.y() - poly.back().y(), pt.x() - poly.back().x()) * 180 / PI) + 270) % 360;
                    if (nA > 90 && nA < 270)
                    {
                        nA -= 180;
                    }
                }
                if (track.m_status.QuanHangJi == 3)
                {
                    painter.save();
                    painter.translate(pt);
                    painter.rotate(nA);
                    tT = QDateTime::fromSecsSinceEpoch(it1->nT).time();
                    if (tT.second() == 0)
                    {
                        if (tT.minute() == 0)
                        {
                            sprintf(szBuf, "%02d:00:00", tT.hour());
                        }
                        else
                        {
                            sprintf(szBuf, "%02d:00", tT.minute());
                        }
                    }
                    else
                    {
                        sprintf(szBuf, "%02d", tT.second());
                    }
                    painter.drawText(0, 0, QLatin1String(szBuf));
                    painter.restore();
                }
                else if (track.m_status.QuanHangJi == 2)
                {
                    tT = QDateTime::fromSecsSinceEpoch(it1->nT).time();
                    if (tT.minute() != tPre.minute())
                    {
                        sprintf(szBuf, "%02d", tT.minute());
                        painter.save();
                        painter.translate(pt);
                        painter.rotate(nA);
                        painter.drawText(0, 0, QLatin1String(szBuf));
                        painter.restore();
                    }
                    tPre = tT;
                }
                poly << pt;
            }
            if (track.m_status.XianHangJi)
            {
                painter.drawPolyline(poly);
            }
        }
    }
}

bool CTrackMgr::Process(int nLength, char *lpData)
{
    if (nLength <= 0)
    {
        return false;
    }

    if (nLength != sizeof(TrackInfo))
    {
        return false;
    }

    TrackPt pt;
    TrackInfo *pD = (TrackInfo *)lpData;
    QDateTime dt = QDateTime::currentDateTime();

    pt.nT = dt.toTime_t();

    QTrack *pTrk = GetTrack(pD->cmn.Z, pD->cmn.P);
    if (pD->cmn.Disapper)
    {
        pTrk->m_status.XiaoShi = 15;
    }
    else
    {
        pTrk->m_status.XiaoShi = 0;
    }

    switch (pD->header.ucType)
    {
    case NETHEADER_UCTYPE_TRACK_COMMON:
    {
        LyXyhc::Get().GetProj()->jwxy(pD->cmn.Longitude, pD->cmn.Latitude, &pt.dX, &pt.dY);
        pt.nH = pD->cmn.Altitude;
        if (m_pTelemetryWidgetShow)
        {
            if (m_pTelemetry->rowCount() > 9999)
            {
                m_pTelemetry->removeRow(9999);
            }
            m_pTelemetry->insertRow(0);
            QTableWidgetItem *pItem = new QTableWidgetItem(dt.toString("hh:mm:ss"));
            m_pTelemetry->setItem(0, 0, pItem);
            pItem = new QTableWidgetItem(QString::asprintf("%d", pD->cmn.Z));
            m_pTelemetry->setItem(0, 1, pItem);
            pItem = new QTableWidgetItem(QString::asprintf("%d", pD->cmn.P));
            m_pTelemetry->setItem(0, 2, pItem);
            if (m_pTelemetryWidgetShowDegree)
            {
                pItem = new QTableWidgetItem(QString::asprintf("%.7f", pD->cmn.Longitude));
                m_pTelemetry->setItem(0, 3, pItem);
                pItem = new QTableWidgetItem(QString::asprintf("%.7f", pD->cmn.Latitude));
                m_pTelemetry->setItem(0, 4, pItem);
            }
            else
            {
                int nT = (pD->cmn.Longitude * 360000 + 0.5);
                pItem = new QTableWidgetItem(QString::asprintf("%03d.%02d.%04d", nT / 360000, nT / 6000 % 60, nT % 6000));
                m_pTelemetry->setItem(0, 3, pItem);
                nT = (pD->cmn.Latitude * 360000 + 0.5);
                pItem = new QTableWidgetItem(QString::asprintf("%03d.%02d.%04d", nT / 360000, nT / 6000 % 60, nT % 6000));
                m_pTelemetry->setItem(0, 4, pItem);
            }

            pItem = new QTableWidgetItem(QString::asprintf("%.2f", pD->cmn.Altitude));
            m_pTelemetry->setItem(0, 5, pItem);
        }

        if (m_TrackList)
        {
            m_TrackList->Update((pD->cmn.Z << 8) | pD->cmn.P, pTrk);
            emit(sendMarkID((pD->cmn.Z << 8) | pD->cmn.P));
        }
    }
    break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL:
    {
        switch (pD->header.ucSubType)
        {
        case 1:
            pTrk->Type_Second = pD->cmn.Type_Second;
            break;
        case 2:
            pTrk->Model_Second = pD->cmn.Model_Second;
            break;
        case 3:
        {
            if (pD->cmn.IFF_Second)
            {
                unsigned char iff = pD->cmn.IFF_Second;
                if (pTrk->m_status.UserColor == 0 && (iff <= IFF_Number && iff > 0))
                {
                    pTrk->m_Color = colorIFF[iff - 1];
                    ChangeUav3DColor(pD->cmn.Z, pD->cmn.P, pTrk->m_Color);
                }
            }
            else
            {
                unsigned char iff = pD->cmn.IFF_First;
                if (pTrk->m_status.UserColor == 0 && (iff <= IFF_Number && iff > 0))
                {
                    pTrk->m_Color = colorIFF[iff - 1];
                    ChangeUav3DColor(pD->cmn.Z, pD->cmn.P, pTrk->m_Color);
                }
            }
            pTrk->IFF_Second = pD->cmn.IFF_Second;
        }
        break;
        case 4:
            pTrk->State_Second = pD->cmn.State_Second;
            break;
        case 5:
            pTrk->Task_Second = pD->cmn.Task_Second;
            break;
        case 6:
            pTrk->Formation_Second = pD->cmn.Formation_Second;
            break;
        case 7:
            pTrk->Power = pD->cmn.Power;
            break;
        case 8:
            pTrk->RemainEnergy = pD->cmn.RemainEnergy;
            break;
        case 9:
        {
            const unsigned char &pilot = pD->cmn.Pilot;
            qDebug() << "user Pilot:" << m_localPilot << ", server pilot:" << pilot;

            if (pilot == m_localPilot)
            {
                if ((LyXyhc::Get().m_vslAttributeItem.size() - 3) >= 0
                        && ((LyXyhc::Get().m_vslAttributeItem.size() - 3) < LyXyhc::Get().m_vslAttributeItem.size())
                        && (pilot - 1 >= 0)
                        && (pilot - 1 < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 3].size())
                   )
                {
                    const QStringList &sl = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 3];
                    int nID = GetTrackID(pTrk);
                    if (LyMessageBox::warning(nullptr, QObject::tr("Received command from the commander"), QObject::tr("%1 Are you sure to take over the aircraft %2. % 3 ?").arg(sl[pilot - 1]).arg(pD->cmn.Z).arg(pD->cmn.P), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
                    {
                        SendManualCommand(nID, 10, pilot, false);
                        LyXyhc::Get().ExecSql(QStringLiteral("INSERT INTO pilotUav VALUES(%1,%2)").arg(pilot).arg(nID));
                        LyXyhc::Get().ExecSql(QStringLiteral("UPDATE pilotUav SET uav=%1 WHERE pilotID=%2").arg(nID).arg(pilot));
                    }
                    else
                    {
                        SendManualCommand(nID, 10, pilot);
                    }
                }
            }
        }
        break;
        case 10:
            pTrk->Pilot = pD->cmn.Pilot;
            break;
        case 11:
        {
            if (LyXyhcInterface::Get().GetSettings().value("User/Role").toInt() == 1)
            {
                unsigned short pilot = pD->cmn.Pilot;
                const QStringList &sl = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 3];
                if (pilot > 0 && pilot <= sl.size())
                {
                    int nID = GetTrackID(pTrk);
                    if (LyMessageBox::warning(nullptr, QObject::tr("Flight control application"), QObject::tr("Flight controller“%1”applies to take over aircraft “%2.%3”，Do you agree?").arg(sl[pilot - 1]).arg(pD->cmn.Z).arg(pD->cmn.P), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
                    {
                        SendManualCommand(nID, 12, pilot, false);
                        LyXyhc::Get().ExecSql(QStringLiteral("INSERT INTO pilotUav VALUES(%1,%2)").arg(pilot).arg(nID));
                        LyXyhc::Get().ExecSql(QStringLiteral("UPDATE pilotUav SET uav=%1 WHERE pilotID=%2").arg(nID).arg(pilot));
                    }
                    else
                    {
                        SendManualCommand(nID, 12, pilot, true);
                    }
                }
            }
        }
        break;
        case 12:
        {
            unsigned short pilot = pD->cmn.Pilot;
            if (pilot == m_localPilot)
            {
                const QStringList &sl = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 3];
                LyMessageBox::information(nullptr, QObject::tr("Flight control application result"), QObject::tr("Commander[%0agrees]Flight controller'%1'applies to take over the aircraft'%2.%3'!").arg(pD->cmn.YesNo == 0 ? "" : QObject::tr("No")).arg(sl[pilot - 1]).arg(pD->cmn.Z).arg(pD->cmn.P));
            }
        }
        break;
        case 13:
        {
            unsigned short pilot = pD->cmn.Pilot;
            if (pilot == m_localPilot)
            {
                const QStringList &sl = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 3];
                LyMessageBox::information(nullptr, QObject::tr("Take back control"), QObject::tr("Flight controller'%1'has regained control of aircraft '%2.%3'!").arg(sl[pilot - 1]).arg(pD->cmn.Z).arg(pD->cmn.P));
            }
        }
        break;
        case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOTIME:
        {
            pTrk->WarnInfoTime_Second = pD->cmn.WarnInfoTime_Second;

#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
            QString strInfo = QObject::tr("Warn Time");
            if (pD->cmn.WarnInfoTime_Second > 0)
            {
                strInfo = QObject::tr("Time forward");
            }
            else if (pD->cmn.WarnInfoTime_Second = 0)
            {
                strInfo = QObject::tr("Time just");
            }
            else if (pD->cmn.WarnInfoTime_Second < 0)
            {
                strInfo = QObject::tr("Time back");
            }
            else
            {
                strInfo = QObject::tr("Time unknown");
            }
            SendMsgToAll_Command(QObject::tr("Warn Time"), strInfo);
#endif
        }
        break;
        case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOTHREAT:
        {
            pTrk->WarnInfoThreat_Second = pD->cmn.WarnInfoThreat_Second;

#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
            QString strInfo = QObject::tr("Threat unknown:") + QString::number(pD->cmn.WarnInfoThreat_Second);
            if (1 <= pD->cmn.WarnInfoThreat_Second && pD->cmn.WarnInfoThreat_Second <= 10)
            {
                strInfo = QObject::tr("Threat Level:") + QString::number(pD->cmn.WarnInfoThreat_Second);
            }
            else
            {
                strInfo = QObject::tr("Threat unknown");
            }
            SendMsgToAll_Command(QObject::tr("Warn Threat"), strInfo);
#endif

        }
        break;
        case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOOBSTACLEDIST:
        {
            pTrk->WarnInfoObstacleDist_Second = pD->cmn.WarnInfoObstacleDist_Second;

#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
            QString strInfo = QObject::tr("ObstacleDist unknown:") + QString::number(pD->cmn.WarnInfoObstacleDist_Second);
            if (1 <= pD->cmn.WarnInfoObstacleDist_Second && pD->cmn.WarnInfoObstacleDist_Second <= 10)
            {
                strInfo = QObject::tr("ObstacleDist Level:") + QString::number(pD->cmn.WarnInfoObstacleDist_Second);
            }
            else
            {
                strInfo = QObject::tr("ObstacleDist unknown");
            }
            SendMsgToAll_Command(QObject::tr("Warn ObstacleDist"), strInfo);
#endif
        }
        break;
        case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOREMAINENERGY:
        {
            pTrk->WarnInfoRemainEnergy_Second = pD->cmn.WarnInfoRemainEnergy_Second;

#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
            QString strInfo = QObject::tr("RemainEnergy unknown:") + QString::number(pD->cmn.WarnInfoRemainEnergy_Second);
            if (1 <= pD->cmn.WarnInfoRemainEnergy_Second && pD->cmn.WarnInfoRemainEnergy_Second <= 100)
            {
                strInfo = QObject::tr("RemainEnergy:") + QString::number(pD->cmn.WarnInfoRemainEnergy_Second);
            }
            else
            {
                strInfo = QObject::tr("RemainEnergy unknown");
            }
            SendMsgToAll_Command(QObject::tr("Warn ObstacleDist"), strInfo);
#endif
        }
        break;
        case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFORADIO:
        {
            pTrk->WarnInfoRadio_Second = pD->cmn.WarnInfoRadio_Second;

#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
            QString strInfo = QObject::tr("Radio unknown:") + QString::number(pD->cmn.WarnInfoRadio_Second);

            if (pD->cmn.WarnInfoRadio_Second == 1)
            {
                strInfo = QObject::tr("Beyond communication");
            }
            else if (pD->cmn.WarnInfoRadio_Second == 2)
            {
                strInfo = QObject::tr("Be disturbed");
            }
            else if (pD->cmn.WarnInfoRadio_Second == 3)
            {
                strInfo = QObject::tr("Poor communication");
            }
            else if (pD->cmn.WarnInfoRadio_Second == 4)
            {
                strInfo = QObject::tr("Communication fault");
            }
            else
            {
                strInfo = QObject::tr("Radio unknown");
            }
            SendMsgToAll_Command(QObject::tr("Warn ObstacleDist"), strInfo);
#endif
        }
        break;
        case NETHEADER_UCTYPE_INTERVENCE_CONTROL_AMMOREMAIN:
        {
            pTrk->AmmoRemain_Second = pD->cmn.AmmoRemain_Second;
#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
            QString strInfo = QObject::tr("Ammo unknown:") + QString::number(pD->cmn.AmmoRemain_Second);
            if (1 <= pD->cmn.AmmoRemain_Second && pD->cmn.AmmoRemain_Second <= 10)
            {
                strInfo = QString::number(pD->cmn.AmmoRemain_Second);
            }
            else
            {
                strInfo = QObject::tr("Ammo unknown");
            }
            SendMsgToAll_Command(QObject::tr("Ammo Remain"), strInfo);
#endif
        }
        break;
        case NETHEADER_UCTYPE_INTERVENCE_CONTROL_LOADTYPE:
        {
            pTrk->LoadType_Second = pD->cmn.LoadType_Second;

#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
            QString strInfo = QObject::tr("Load unknown:") + QString::number(pD->cmn.WarnInfoRadio_Second);
            if (pD->cmn.LoadType_Second == 1)
            {
                strInfo = QObject::tr("GaoDe54");
            }
            else if (pD->cmn.LoadType_Second == 2)
            {
                strInfo = QObject::tr("GaoDe");
            }
            else if (pD->cmn.LoadType_Second == 3)
            {
                strInfo = QObject::tr("YunHanTong");
            }
            else if (pD->cmn.LoadType_Second == 4)
            {
                strInfo = QObject::tr("InfraredAiming54");
            }
            else
            {
                strInfo = QObject::tr("Load unknown");
            }
            SendMsgToAll_Command(QObject::tr("Load Type"), strInfo);
#endif
        }
        break;
        case NETHEADER_UCTYPE_INTERVENCE_CONTROL_REPORT:
        {
             pTrk->NoReport = pD->cmn.NoReport;

#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
            SendMsgToAll_Command(QObject::tr("Report"), QObject::tr("Report:") + QString::number(pD->cmn.NoReport));
#endif
        }
        break;
        case 128:
        {
            if (pTrk->m_nMarkID == 0)
            {
                pTrk->m_nMarkAssign = 0;

                qnzkna::framework::MessageReferenceProps props;
                QString strUAVID = QString::number(pTrk->unNodeID);
                props.put("WeaponID", strUAVID.toStdString());
                props.put("TargetID", "");
                IXyhcActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVAssignMark", nullptr, 0, props));

            }
            pTrk->m_nMarkID = (pD->cmn.StationID << 8) | pD->cmn.MarkID;
            if (pTrk->m_nMarkID & 0x00008000)
            {
                LyXyhc::Get().UpdateContextMenuMark();

                pTrk->m_dMarkX = 0;
                pTrk->m_dMarkY = 0;
                pTrk->m_dMarkH = 0;
                pTrk->m_nMarkVehicleDistShow = 0;
                pTrk->m_dMarkVehicleDist = DBL_MAX;
                pTrk->m_dMarkVehicleHeight = DBL_MAX;
                pTrk->m_dMarkVehicleYaw = DBL_MAX;
                if (((LyXyhc::Get().m_vslAttributeItem.size() - 2) >= 0)
                        && ((LyXyhc::Get().m_vslAttributeItem.size() - 2) < LyXyhc::Get().m_vslAttributeItem.size())
                        && ((pD->cmn.MarkID - 1) >= 0)
                        && ((pD->cmn.MarkID - 1) < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size()))
                {
                    IFunctionInterface &ifi = LyXyhcInterface::Get().GetFunctionInterface();
                    QVariant vt = ifi.Invoke("tsbh.target.query_location",
                                             LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][pD->cmn.MarkID - 1]);
                    if (vt.canConvert<QVariantList>())
                    {
                        const QVariantList &vl = vt.toList();
                        if (vl.size() == 2)
                        {
                            pTrk->m_nMarkAssign = 1;

                            pTrk->m_dMarkX = vl[0].toDouble();
                            pTrk->m_dMarkY = vl[1].toDouble();
                        }
                    }

                    if (IXyhcActivator::Get().getPIMarkManageService() != nullptr)
                    {
                        const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
                        if (pIMarkManageService != nullptr)
                        {
                            const QString targetID = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][pD->cmn.MarkID - 1];
                            const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                            if (!targetItem.GetID().empty())
                            {
                                strncpy(pTrk->m_strMarkID, targetItem.GetID().c_str(), targetItem.GetID().size());
                                pTrk->m_strMarkID[sizeof(pTrk->m_strMarkID) - 1] = '\0';
                                pTrk->m_nMarkAssign = 1;
                                pTrk->m_dMarkX = targetItem.GetLon();
                                pTrk->m_dMarkY = targetItem.GetLat();
                                pTrk->m_dMarkH = targetItem.GetHeight();
                                pTrk->m_nMarkVehicleDistShow = 0;
                                pTrk->m_dMarkVehicleDist = DBL_MAX;
                                pTrk->m_dMarkVehicleHeight = DBL_MAX;
                                pTrk->m_dMarkVehicleYaw = DBL_MAX;

                                qnzkna::framework::MessageReferenceProps props;
                                QString strUAVID = QString::number(pTrk->unNodeID);
                                props.put("WeaponID", strUAVID.toStdString());
                                props.put("TargetID", targetID.toStdString());
                                IXyhcActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVAssignMark", nullptr, 0, props));

                            }
                        }
                    }
                }
            }
        }
        break;
        default:
            break;
        }
        return true;
    }
    break;
    default:
        return false;
    }

    QTrack *pTrack = GetTrack(pD->cmn.Z, pD->cmn.P, pD->header.unNodeType, pD->header.unNodeID, pD);

    pTrack->Type_Second = pD->cmn.Type_Second;
    pTrack->Type_First = pD->cmn.Type_First;
    pTrack->Model_Second = pD->cmn.Model_Second;
    pTrack->Model_First = pD->cmn.Model_First;
    pTrack->IFF_Second = pD->cmn.IFF_Second;
    pTrack->IFF_First = pD->cmn.IFF_First;
    pTrack->State_Second = pD->cmn.State_Second;
    pTrack->State_First = pD->cmn.State_First;
    pTrack->Task_Second = pD->cmn.Task_Second;
    pTrack->Task_First = pD->cmn.Task_First;
    pTrack->Formation_Second = pD->cmn.Formation_Second;
    pTrack->Formation_First = pD->cmn.Formation_First;

    pTrack->WarnInfoTime_First = pD->cmn.WarnInfoTime_First;
    pTrack->WarnInfoTime_Second = pD->cmn.WarnInfoTime_Second;

    pTrack->WarnInfoThreat_First = pD->cmn.WarnInfoThreat_First;
    pTrack->WarnInfoThreat_Second = pD->cmn.WarnInfoThreat_Second;

    pTrack->WarnInfoObstacleDist_First = pD->cmn.WarnInfoObstacleDist_First;
    pTrack->WarnInfoObstacleDist_Second = pD->cmn.WarnInfoObstacleDist_Second;

    pTrack->WarnInfoRemainEnergy_First = pD->cmn.WarnInfoRemainEnergy_First;
    pTrack->WarnInfoRemainEnergy_Second = pD->cmn.WarnInfoRemainEnergy_Second;

    pTrack->WarnInfoRadio_First = pD->cmn.WarnInfoRadio_First;
    pTrack->WarnInfoRadio_Second = pD->cmn.WarnInfoRadio_Second;

    pTrack->AmmoRemain_First = pD->cmn.AmmoRemain_First;
    pTrack->AmmoRemain_Second = pD->cmn.AmmoRemain_Second;

    pTrack->LoadType_First = pD->cmn.LoadType_First;
    pTrack->LoadType_Second = pD->cmn.LoadType_Second;

    pTrack->Power = pD->cmn.Power;
    pTrack->RemainEnergy = pD->cmn.RemainEnergy;
    pTrack->Pilot = pD->cmn.Pilot;
    pTrack->Pitch = pD->cmn.Pitch;
    pTrack->Roll = pD->cmn.Roll;
    pTrack->m_nHangXiang = pD->cmn.Course;
    pTrack->m_nSuDu = pD->cmn.Speed;
    pTrack->m_nGaoDu = pD->cmn.Altitude;
    pTrack->m_nGaoDuRel = pD->cmn.Altitude;
    pTrack->m_Longitude = pD->cmn.Longitude;
    pTrack->m_Latitude = pD->cmn.Latitude;

    pTrack->m_lsPoints.push_back(pt);

    {

        pTrack->unNodeType = pD->header.unNodeType;
        pTrack->unUnuse1 = pD->header.unUnuse1;
        pTrack->unUnuse2 = pD->header.unUnuse2;
        pTrack->unUnuse3 = 0;
        pTrack->unNodeID = pD->header.unNodeID;

    }

    if (pTrack->m_nMarkID & 0x00008000)
    {
        if (pTrack->m_dMarkX == 0 && pTrack->m_dMarkY == 0)
        {
            const long int m_vslAttributeItemSize = LyXyhc::Get().m_vslAttributeItem.size() - 2;
            if ((pD->cmn.MarkID - 1 >= 0) && m_vslAttributeItemSize >= 0 && m_vslAttributeItemSize < LyXyhc::Get().m_vslAttributeItem.size())
            {
                if ((LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size() > (pD->cmn.MarkID - 1)))
                {
                    LyXyhc::Get().UpdateContextMenuMark();

                    IFunctionInterface &ifi = LyXyhcInterface::Get().GetFunctionInterface();
                    QVariant vt = ifi.Invoke("tsbh.target.query_location",
                                             LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][pD->cmn.MarkID - 1]);
                    if (vt.canConvert<QVariantList>())
                    {
                        const QVariantList &vl = vt.toList();
                        if (vl.size() == 2)
                        {
                            pTrack->m_nMarkAssign = 1;

                            pTrack->m_dMarkX = vl[0].toDouble();
                            pTrack->m_dMarkY = vl[1].toDouble();
                        }
                    }

                    if (IXyhcActivator::Get().getPIMarkManageService() != nullptr)
                    {
                        const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
                        if (pIMarkManageService != nullptr)
                        {
                            const QString targetID = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][pD->cmn.MarkID - 1];
                            const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                            if (!targetItem.GetID().empty())
                            {
                                strncpy(pTrack->m_strMarkID,  targetItem.GetID().c_str(), targetItem.GetID().size());
                                pTrack->m_strMarkID[sizeof(pTrack->m_strMarkID) - 1] = '\0';
                                pTrack->m_nMarkAssign = 1;
                                pTrack->m_dMarkX = targetItem.GetLon();
                                pTrack->m_dMarkY = targetItem.GetLat();
                                pTrack->m_dMarkH = targetItem.GetHeight();
                                pTrack->m_nMarkVehicleDistShow = 0;
                                pTrack->m_dMarkVehicleDist = DBL_MAX;
                                pTrack->m_dMarkVehicleHeight = DBL_MAX;
                                pTrack->m_dMarkVehicleYaw = DBL_MAX;
                            }
                        }
                    }
                }
            }
        }
    }
    else if (pTrack->m_nMarkID & 0x00007FFF)
    {
        pTrack->m_dMarkX = 0;
        pTrack->m_dMarkY = 0;
        pTrack->m_dMarkH = 0;
        pTrack->m_nMarkVehicleDistShow = 0;
        pTrack->m_dMarkVehicleDist = DBL_MAX;
        pTrack->m_dMarkVehicleHeight = DBL_MAX;
        pTrack->m_dMarkVehicleYaw = DBL_MAX;

        QTrack *pTrk = FindTrack((pTrack->m_nMarkID & 0x00007F00) >> 8, pTrack->m_nMarkID & 0x000000FF);
        if (pTrk != nullptr && pTrk->m_lsPoints.size())
        {
            const TrackPt &pt0 = pTrk->m_lsPoints.back();
            if (LyXyhc::Get().GetProj() != nullptr)
            {
                pTrack->m_nMarkAssign = 2;
                LyXyhc::Get().GetProj()->xyjw(pt0.dX, pt0.dY, &pTrack->m_dMarkX, &pTrack->m_dMarkY);
            }
        }
    }

    static IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != nullptr && pEarth->IsVisible())
    {
        static ITrackManager *pTrackManager = nullptr;
        static IGraphics *pGrp = pEarth->GetGraphics();
        if (pTrackManager == nullptr)
        {
            pTrackManager = pEarth->GetTrackManager();
        }

        if (pTrackManager != nullptr)
        {
            GVector gv;
            const TrackPt &t = pTrack->m_lsPoints.back();
            m_pProj->xyjw(t.dX, t.dY, &gv.dLon, &gv.dLat);
            gv.dLon = pD->cmn.Longitude;
            gv.dLat = pD->cmn.Latitude;
            gv.dAlt = pD->cmn.Altitude;

            gv.fYaw = pTrack->m_nHangXiang;
            gv.fPitch = pTrack->Pitch;
            gv.fRoll = pTrack->Roll;

            QColor color = pTrack->m_Color;

            gv.nType = 4;
            static char szT[1024];
            sprintf(szT, "%d.%d%s", pD->cmn.Z, pD->cmn.P, GetWeiBiao(*pTrack).c_str());
            GVector gv2 = gv;
            if (gv2.dAlt <= 0.000001 && gv2.dAlt >= -0.000001)
            {
                gv2.dAlt = gv2.dAlt + 0.0005;
            }
            //pTrackManager->UpdateTrackPosition(szT, gv2);
            ITrack *pTrk = pTrackManager->GetTrack(szT);
            if (pTrk != nullptr)
            {
                pTrk->SetDisplayPoints(1);
                pTrk->ShowLabel(true);
                static unsigned char sucI = 0;
                if (sucI++ < 128)
                {
                    pTrk->SetRate(0.99);
                }
                else
                {
                    pTrk->SetRate(1.01);
                }

                Uav3D &uav3D = g_mapUav[pD->cmn.Z << 8 | pD->cmn.P];
                uav3D.pTrack = pTrk;

#ifndef USING_LY_CCIP_80MM_DEFINE
                if (uav3D.pBombTrack == nullptr && uav3D.pBombTrack2 == nullptr)
                {
                    if (pTrack->m_MarkVehicleCalShow == 1)
                    {
                        uav3D.pBombTrack = CreateParabola(pTrack->m_nSuDu + 200, pTrack->m_nGaoDu, gv.fPitch);
                        uav3D.pBombTrack2 = CreateParabola2(pTrack->m_nSuDu + 200, pTrack->m_nGaoDu, gv.fPitch);

                        if (pTrack->m_MarkVehicleCalShowChange == 1)
                        {
                            pTrack->m_MarkVehicleCalShowChange = 0;
                            if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                            {
                                if (uav3D.pBombTrack != nullptr)
                                {
                                    uav3D.pBombTrack->SetVisible(true);
                                }

                                if (uav3D.pBombTrack2 != nullptr)
                                {
                                    uav3D.pBombTrack2->SetVisible(true);
                                }
                            }
                            else
                            {
                                if (uav3D.pBombTrack != nullptr)
                                {
                                    uav3D.pBombTrack->SetVisible(false);
                                }

                                if (uav3D.pBombTrack2 != nullptr)
                                {
                                    uav3D.pBombTrack2->SetVisible(false);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (pTrack->m_MarkVehicleCalShowChange == 1)
                        {
                            pTrack->m_MarkVehicleCalShowChange = 0;
                            if (uav3D.pBombTrack != nullptr)
                            {
                                uav3D.pBombTrack->SetVisible(false);
                            }

                            if (uav3D.pBombTrack2 != nullptr)
                            {
                                uav3D.pBombTrack2->SetVisible(false);
                            }
                        }
                    }
                }
                if (uav3D.pBombTrack != nullptr && uav3D.pBombTrack2 != nullptr)
                {
                    if (pTrack->m_MarkVehicleCalShow == 1)
                    {
                        uav3D.pBombTrack->SetPosition(gv.dLon, gv.dLat, gv.dAlt);
                        uav3D.pBombTrack->SetYawPitchRoll(gv.fYaw, gv.fPitch - 180, gv.fRoll);
                        uav3D.pBombTrack2->SetPosition(gv.dLon, gv.dLat, gv.dAlt);
                        uav3D.pBombTrack2->SetYawPitchRoll(gv.fYaw, gv.fPitch - 180, gv.fRoll);
                        if (pTrack->m_MarkVehicleCalShowChange == 1)
                        {
                            pTrack->m_MarkVehicleCalShowChange = 0;
                            if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                            {
                                if (uav3D.pBombTrack != nullptr)
                                {
                                    uav3D.pBombTrack->SetVisible(true);
                                }

                                if (uav3D.pBombTrack2 != nullptr)
                                {
                                    uav3D.pBombTrack2->SetVisible(true);
                                }
                            }
                            else
                            {
                                if (uav3D.pBombTrack != nullptr)
                                {
                                    uav3D.pBombTrack->SetVisible(false);
                                }

                                if (uav3D.pBombTrack2 != nullptr)
                                {
                                    uav3D.pBombTrack2->SetVisible(false);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (pTrack->m_MarkVehicleCalShowChange == 1)
                        {
                            pTrack->m_MarkVehicleCalShowChange = 0;
                            if (uav3D.pBombTrack != nullptr)
                            {
                                uav3D.pBombTrack->SetVisible(false);
                            }

                            if (uav3D.pBombTrack2 != nullptr)
                            {
                                uav3D.pBombTrack2->SetVisible(false);
                            }
                        }
                    }
                }
#elif defined USING_LY_CCIP_80MM_DEFINE
                if (uav3D.pUavMarkTrack == nullptr)
                {
                    if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                    {
                        const double dLongitudeUav = gv.dLon;
                        const double dLatitudeUav = gv.dLat;
                        const double dAltitudeUav = gv.dAlt;

                        const double dLongitudeMark = pTrack->m_dMarkX;
                        const double dLatitudeMark = pTrack->m_dMarkY;
                        const double dAltitudeMark = pTrack->m_dMarkH;

                        uav3D.pUavMarkTrack = UpdateUavMarkTrack_80mm(uav3D.pUavMarkTrack,
                                                                          dLongitudeUav, dLatitudeUav, dAltitudeUav,
                                                                          dLongitudeMark, dLatitudeMark, dAltitudeMark);
                        QColor colorTrack(color);
                        colorTrack.setAlpha(255);
                        QColor colorTemp;
                        colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());

                        if (uav3D.pUavMarkTrack != nullptr)
                        {
                            uav3D.pUavMarkTrack->SetColor(colorTemp.rgba());
                        }
                    }
                    else
                    {
                        if (uav3D.pUavMarkTrack != nullptr)
                        {
                            IEarth *pEarth = IEarth::GetEarth();
                            if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
                            {
                                pEarth->GetGraphics()->RemoveObject(uav3D.pUavMarkTrack);
                            }
                        }
                    }
                }
                else if (uav3D.pUavMarkTrack != nullptr)
                {
                    if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                    {
                        if (uav3D.Uav3DMarkTrackShowTimes >= 1)
                        {
                            const double dLongitudeUav = gv.dLon;
                            const double dLatitudeUav = gv.dLat;
                            const double dAltitudeUav = gv.dAlt;

                            const double dLongitudeMark = pTrack->m_dMarkX;
                            const double dLatitudeMark = pTrack->m_dMarkY;
                            const double dAltitudeMark = pTrack->m_dMarkH;

                            uav3D.pUavMarkTrack = UpdateUavMarkTrack_80mm(uav3D.pUavMarkTrack,
                                                                              dLongitudeUav, dLatitudeUav, dAltitudeUav,
                                                                              dLongitudeMark, dLatitudeMark, dAltitudeMark);
                            QColor colorTrack(color);
                            colorTrack.setAlpha(255);
                            QColor colorTemp;
                            colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());

                            if (uav3D.pUavMarkTrack != nullptr)
                            {
                                uav3D.pUavMarkTrack->SetColor(colorTemp.rgba());
                            }
                            uav3D.Uav3DMarkTrackShowTimes = 0;
                        }
                        else
                        {
                            uav3D.Uav3DMarkTrackShowTimes++;
                            if (uav3D.pUavMarkTrack != nullptr)
                            {
                                IEarth *pEarth = IEarth::GetEarth();
                                if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
                                {
                                    pEarth->GetGraphics()->RemoveObject(uav3D.pUavMarkTrack);
                                }
                            }
                        }

                    }
                    else
                    {
                        if (uav3D.pUavMarkTrack != nullptr)
                        {
                            IEarth *pEarth = IEarth::GetEarth();
                            if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
                            {
                                pEarth->GetGraphics()->RemoveObject(uav3D.pUavMarkTrack);
                            }
                        }
                    }
                }

                if (uav3D.pBombTrack == nullptr && uav3D.pBombTrack2 == nullptr)
                {
                    if (pTrack->m_MarkVehicleCalShow == 1)
                    {
                        double x = 0, y = 0, z = 0;
                        const double dRelAltitude = (gv.dAlt - pTrack->m_dMarkH) + 1;
                        uav3D.pBombTrack = CreateParabola_80mm(gv.dLon, gv.dLat, gv.dAlt, (pTrack->m_nSuDu < 0 ? 0 : pTrack->m_nSuDu), dRelAltitude, gv.fPitch,
                                                               x, y, z);
                        uav3D.pBombTrack2 = CreateParabola2_80mm(x, y, z);

                        QColor colorTrack(color);
                        colorTrack.setAlpha(255);
                        QColor colorTemp;
                        colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());

                        if (uav3D.pBombTrack != nullptr)
                        {
                            uav3D.pBombTrack->SetColor(colorTemp.rgba());
                        }

                        if (uav3D.pBombTrack2 != nullptr)
                        {
                            uav3D.pBombTrack2->SetColor(colorTemp.rgba());
                        }

                        if (uav3D.pBombTrack != nullptr)
                        {
                            uav3D.pBombTrack->SetVisible(true);
                        }

                        if (pTrack->m_MarkVehicleCalShowChange == 1)
                        {
                            pTrack->m_MarkVehicleCalShowChange = 0;
                            if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                            {
                                if (uav3D.pBombTrack != nullptr)
                                {
                                    uav3D.pBombTrack->SetVisible(true);
                                }

                                if (uav3D.pBombTrack2 != nullptr)
                                {
                                    uav3D.pBombTrack2->SetVisible(true);
                                }
                            }
                            else
                            {
                                if (uav3D.pBombTrack != nullptr)
                                {
                                    uav3D.pBombTrack->SetVisible(false);
                                }

                                if (uav3D.pBombTrack2 != nullptr)
                                {
                                    uav3D.pBombTrack2->SetVisible(false);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (pTrack->m_MarkVehicleCalShowChange == 1)
                        {
                            pTrack->m_MarkVehicleCalShowChange = 0;
                            if (uav3D.pBombTrack != nullptr)
                            {
                                uav3D.pBombTrack->SetVisible(false);
                            }

                            if (uav3D.pBombTrack2 != nullptr)
                            {
                                uav3D.pBombTrack2->SetVisible(false);
                            }
                        }
                    }
                }
                if (uav3D.pBombTrack != nullptr && uav3D.pBombTrack2 != nullptr)
                {
                    if (pTrack->m_MarkVehicleCalShow == 1)
                    {
                        if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                        {
                            double x = 0, y = 0, z = 0;
                            const double dRelAltitude = (gv.dAlt - pTrack->m_dMarkH) + 1;
                            uav3D.pBombTrack = UpdateParabola_80mm(uav3D.pBombTrack, gv.dLon, gv.dLat, gv.dAlt, (pTrack->m_nSuDu < 0 ? 0 : pTrack->m_nSuDu), dRelAltitude, gv.fPitch, x, y, z);

                            if (uav3D.pBombTrack != nullptr)
                            {
                                uav3D.pBombTrack->SetPosition(gv.dLon, gv.dLat, gv.dAlt);
                                uav3D.pBombTrack->SetYawPitchRoll(gv.fYaw, 0, 0);
                            }

                            uav3D.pBombTrack2 = UpdateParabola2_80mm(uav3D.pBombTrack2, x, y, z);
                            if (uav3D.pBombTrack2 != nullptr)
                            {
                                double dLon = gv.dLon, dLat = gv.dLat;

                                uav3D.pBombTrack2->SetPosition(dLon, dLat, z);
                                uav3D.pBombTrack2->SetYawPitchRoll(gv.fYaw, 0 - 180, 0);
                            }

                            QColor colorTrack(color);
                            colorTrack.setAlpha(255);
                            QColor colorTemp;
                            colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());

                            if (uav3D.pBombTrack != nullptr)
                            {
                                uav3D.pBombTrack->SetColor(colorTemp.rgba());
                            }

                            if (uav3D.pBombTrack2 != nullptr)
                            {
                                uav3D.pBombTrack2->SetColor(colorTemp.rgba());
                            }

                            if (uav3D.pBombTrack != nullptr)
                            {
                                uav3D.pBombTrack->SetVisible(true);
                            }

                            if (uav3D.pBombTrack2 != nullptr)
                            {
                                uav3D.pBombTrack2->SetVisible(true);
                            }
                        }
                        else
                        {
                            if (uav3D.pBombTrack != nullptr)
                            {
                                uav3D.pBombTrack->SetVisible(false);
                            }

                            if (uav3D.pBombTrack2 != nullptr)
                            {
                                uav3D.pBombTrack2->SetVisible(false);
                            }
                        }
                    }
                    else
                    {
                        if (pTrack->m_MarkVehicleCalShowChange == 1)
                        {
                            pTrack->m_MarkVehicleCalShowChange = 0;
                            if (uav3D.pBombTrack != nullptr)
                            {
                                uav3D.pBombTrack->SetVisible(false);
                            }

                            if (uav3D.pBombTrack2 != nullptr)
                            {
                                uav3D.pBombTrack2->SetVisible(false);
                            }
                        }
                    }
                }
#endif
                if (uav3D.pSightingLine == nullptr && uav3D.pSightingLine2 == nullptr)
                {
                    if (pTrack->SensorViewShow == 1)
                    {

						qnzkna::SensorManage::SensorInfoMap tempSensorInfo;

						bool isGetSenerInfo = IXyhcActivator::Get().getPISensorManageService()->
							GetSensorInfosInWeaponComponent(std::to_string(pD->header.unNodeID), &tempSensorInfo);
						if (isGetSenerInfo)
						{
							std::string str_fov_h;
							std::string str_fov_w;
							std::string str_pitch;
							std::string str_yaw;
							std::string str_roll;
							std::string str_dis;
							bool isReadInfo = false;
							auto sensorItor = tempSensorInfo.begin();
							while (sensorItor != tempSensorInfo.end())
							{
								if (!sensorItor->second.GetSensorParam("视场角-高", str_fov_h))
								{
									sensorItor++;
									continue;
								}
								if (!sensorItor->second.GetSensorParam("视场角-宽", str_fov_w))
								{
									sensorItor++;
									continue;
								}
								if (!sensorItor->second.GetSensorParam("俯仰角", str_pitch))
								{
									sensorItor++;
									str_pitch = "0";
								}
								if (!sensorItor->second.GetSensorParam("偏航角", str_yaw))
								{
									sensorItor++;
									str_yaw = "0";
								}
								if (!sensorItor->second.GetSensorParam("横滚角", str_roll))
								{
									sensorItor++;
									str_roll = "0";
								}
								if (!sensorItor->second.GetSensorParam("最远探测距离", str_dis))
								{
									sensorItor++;
									continue;
								}
								isGetSenerInfo = true;
								break;
							}
							if (isGetSenerInfo)
							{
								int fov_h = std::stoi(str_fov_h);
								int fov_w = std::stoi(str_fov_w);
								double pitch = std::stod(str_pitch);
								double yaw = std::stod(str_yaw);
								double roll = std::stod(str_roll);
								int dis = std::stoi(str_dis);
								std::vector<IUserMesh2*> discoverZones = CreateDetectionZone(fov_w, fov_h, dis);
								if (discoverZones.size() == 2)
								{
									uav3D.pSightingLine = discoverZones[0];
									uav3D.pSightingLine2 = discoverZones[1];
								}
							}
						}

                        if (uav3D.pSightingLine != nullptr)
                        {
                            uav3D.pSightingLine->SetVisible(true);
                        }

                        if (uav3D.pSightingLine2 != nullptr)
                        {
                            uav3D.pSightingLine2->SetVisible(true);
                        }
                    }
                    else
                    {
                        if (uav3D.pSightingLine != nullptr)
                        {
                            uav3D.pSightingLine->SetVisible(false);
                        }

                        if (uav3D.pSightingLine2 != nullptr)
                        {
                            uav3D.pSightingLine2->SetVisible(false);
                        }
                    }
                }
                if (uav3D.pSightingLine != NULL && uav3D.pSightingLine2 != NULL)
                {
                    if (pTrack->SensorViewShow == 1)
                    {

                        const float fPitch = gv.fPitch + pTrack->SensorPitch;
                        const float fRoll = gv.fRoll + pTrack->SensorRoll;
                        const float fYaw = gv.fYaw + pTrack->SensorYaw;

                        uav3D.pSightingLine->SetPosition(gv.dLon, gv.dLat, gv.dAlt);

                        uav3D.pSightingLine->SetYawPitchRoll(fYaw, fPitch, fRoll);
                        uav3D.pSightingLine2->SetPosition(gv.dLon, gv.dLat, gv.dAlt);

                        uav3D.pSightingLine2->SetYawPitchRoll(fYaw, fPitch, fRoll);

                        QColor colorTrack(color);
                        colorTrack.setAlpha(255);
                        QColor colorTemp;
                        colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());

                        if (uav3D.pSightingLine != nullptr && uav3D.pSightingLine2 != nullptr)
                        {
                            colorTrack.setAlpha(40);
                            colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());
                            uav3D.pSightingLine->SetColor(colorTemp.rgba());
                            colorTrack.setAlpha(80);
                            colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());
                            uav3D.pSightingLine2->SetColor(colorTemp.rgba());
                        }

                        if (pTrack->SensorViewShowChange == 1)
                        {
                            pTrack->SensorViewShowChange = 0;
                            if (uav3D.pSightingLine != nullptr)
                            {
                                uav3D.pSightingLine->SetVisible(true);
                            }

                            if (uav3D.pSightingLine2 != nullptr)
                            {
                                uav3D.pSightingLine2->SetVisible(true);
                            }
                        }
                    }
                    else
                    {
                        if (pTrack->SensorViewShowChange == 1)
                        {
                            pTrack->SensorViewShowChange = 0;
                            if (uav3D.pSightingLine != nullptr)
                            {
                                uav3D.pSightingLine->SetVisible(false);
                            }

                            if (uav3D.pSightingLine2 != nullptr)
                            {
                                uav3D.pSightingLine2->SetVisible(false);
                            }
                        }
                    }
                }

                if (uav3D.colorSightingLine != color)
                {
                    uav3D.colorSightingLine = color;

                    QColor colorTrack(color);
                    colorTrack.setAlpha(255);
                    QColor colorTemp;
                    colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());

                    if (pTrk != nullptr)
                    {
                        pTrk->SetColor(pTrk->GetType(), colorTemp.rgba());
                    }

                    if (uav3D.pBombTrack != nullptr)
                    {
                        uav3D.pBombTrack->SetColor(colorTemp.rgba());
                    }

                    if (uav3D.pBombTrack2 != nullptr)
                    {
                        uav3D.pBombTrack2->SetColor(colorTemp.rgba());
                    }

                    if (uav3D.pSightingLine != nullptr && uav3D.pSightingLine2 != nullptr)
                    {
                        colorTrack.setAlpha(40);
                        colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());
                        uav3D.pSightingLine->SetColor(colorTemp.rgba());
                        colorTrack.setAlpha(80);
                        colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());
                        uav3D.pSightingLine2->SetColor(colorTemp.rgba());
                    }
                }

            }

        }

    }
    return true;
}

bool CTrackMgr::Process_XygsZC_02(const unsigned long long &unNodeID, const XygsZC_02 xygsZC_02)
{
    if (unNodeID == 0 || xygsZC_02.unNodeID == 0 || xygsZC_02.unNodeType != NODE_TYPE_SENSOR)
    {
        return false;
    }

    emit m_XygsZC_02_Process.Process_XygsZC_02_Sig(unNodeID, xygsZC_02);
    return true;
}

QTrack *CTrackMgr::SearchTrack(const QPoint &pt)
{
    double dX, dY;
    TrackMap::iterator it;
    for (it = m_Track.begin(); it != m_Track.end(); it++)
    {
        QTrack *pTrack = &it->second;
        TrackPtList::reverse_iterator it2;
        it2 = pTrack->m_lsPoints.rbegin();
        if (it2 != pTrack->m_lsPoints.rend())
        {
            QPoint pt2 = m_pProj->Map2Screen(it2->dX, it2->dY);
            QRect rect(pt2.x() + pTrack->m_nOffsetX * OFFSET_RATE - 5, pt2.y() + pTrack->m_nOffsetY * OFFSET_RATE - 5, 36 + 10, 10 + 5);
            if (rect.contains(pt))
            {
                return pTrack;
            }
        }
    }
    return NULL;
}

void SendManualCommand(int nID, int nSubType, int nCmd, bool bNo)
{
    TrackInfo ti;
    memset(&ti, 0, sizeof(ti));
    ti.header.ucType = 254;
    ti.header.ucSubType = nSubType;
    ti.header.unLength = sizeof(ti.header) + sizeof(ti.cmn);
    ti.cmn.Z = nID >> 8;
    ti.cmn.P = (nID & 0xFF);
    switch (nSubType)
    {
    case 1:
        ti.cmn.Type_Second = nCmd;
        break;
    case 2:
        ti.cmn.Model_Second = nCmd;
        break;
    case 3:
        ti.cmn.IFF_Second = nCmd;
        break;
    case 4:
        ti.cmn.State_Second = nCmd;
        break;
    case 5:
        ti.cmn.Task_Second = nCmd;
        break;
    case 6:
        ti.cmn.Formation_Second = nCmd;
        break;
    case 7:
        ti.cmn.Power = nCmd;
        break;
    case 8:
        ti.cmn.RemainEnergy = nCmd;
        break;
    case 10:
        ti.cmn.YesNo = bNo;
    case 12:
        ti.cmn.YesNo = bNo;
    case 9:
    case 11:
    case 13:
        ti.cmn.Pilot = nCmd;
        break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOTIME:
    {
        ti.cmn.WarnInfoTime_Second = nCmd;
    }
    break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOTHREAT:
    {
        ti.cmn.WarnInfoThreat_Second = nCmd;
    }
    break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOOBSTACLEDIST:
    {
        ti.cmn.WarnInfoObstacleDist_Second = nCmd;
    }
    break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOREMAINENERGY:
    {
        ti.cmn.WarnInfoRemainEnergy_Second = nCmd;
    }
    break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFORADIO:
    {
        ti.cmn.WarnInfoRadio_Second = nCmd;
    }
    break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL_AMMOREMAIN:
    {
        ti.cmn.AmmoRemain_Second = nCmd;
    }
    break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL_LOADTYPE:
    {
        ti.cmn.LoadType_Second = nCmd;
    }
    break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL_REPORT:
    {
        ti.cmn.NoReport = nCmd;
    }
    break;
    case NETHEADER_UCTYPE_INTERVENCE_CONTROL_DISAPPER:
    {
        ti.cmn.Disapper = nCmd;
    }
    case 128:
        ti.cmn.StationID = nCmd >> 8;
        ti.cmn.MarkID = (nCmd & 0xFF);
        break;
    default:
        break;
    }

    LyXyhcInterface::Get().SendData((const char *)&ti, ti.header.unLength);
}

void CTrackMgr::OnMenuAction(QTrack *pTrack, QAction *pAction)
{
    if (pTrack == nullptr || pAction == nullptr)
    {
        return;
    }

    uint id = pAction->data().toUInt();
    if (id >= 65535 && id < 75535)
    {
        switch (id)
        {
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Synergy_Disapper:
        {
            pTrack->m_status.XiaoShi = 1;

            const int nValue = 1;
            int nID = GetTrackID(pTrack);
            SendManualCommand(nID, NETHEADER_UCTYPE_INTERVENCE_CONTROL_DISAPPER, nValue);
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Report_Report:
        {
            bool ok = false;
            const int nValue = pAction->property("LYSD_Value").toInt(&ok);
            if (ok)
            {
                int nID = GetTrackID(pTrack);
                SendManualCommand(nID, NETHEADER_UCTYPE_INTERVENCE_CONTROL_REPORT, nValue);
            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_WarnInfo_WarnInfoTime:
        {
            bool ok = false;
            const int loadType = pAction->property("LYSD_Value").toInt(&ok);
            if (ok)
            {
                int nID = GetTrackID(pTrack);
                SendManualCommand(nID, NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOTIME, loadType);

#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
                QString strInfo = QObject::tr("Warn Time");
                if (loadType == 1)
                {
                    strInfo = QObject::tr("Time forward");
                }
                else if (loadType == 2)
                {
                    strInfo = QObject::tr("Time just");
                }
                else if (loadType == 3)
                {
                    strInfo = QObject::tr("Time back");
                }
                else
                {
                    strInfo = QObject::tr("Time unknown");
                }
                SendMsgToAll_Command(QObject::tr("Warn Time"), strInfo, false);
#endif
            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_WarnInfo_WarnInfoThreat:
        {
            bool ok = false;
            const int warnInfoThreat = pAction->property("LYSD_Value").toInt(&ok);
            if (ok)
            {
                int nID = GetTrackID(pTrack);
                SendManualCommand(nID, NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOTHREAT, warnInfoThreat);
#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
                QString strInfo = QObject::tr("Threat unknown:") + QString::number(warnInfoThreat);
                if (1 <= warnInfoThreat && warnInfoThreat <= 10)
                {
                    strInfo = QObject::tr("Threat Level:") + QString::number(warnInfoThreat);
                }
                else
                {
                    strInfo = QObject::tr("Threat unknown");
                }
                SendMsgToAll_Command(QObject::tr("Warn Threat"), strInfo, false);
#endif
            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_WarnInfo_WarnInfoObstacleDist:
        {
            bool ok = false;
            const int warnInfoObstacleDist = pAction->property("LYSD_Value").toInt(&ok);
            if (ok)
            {
                int nID = GetTrackID(pTrack);
                SendManualCommand(nID, NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOOBSTACLEDIST, warnInfoObstacleDist);
#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
                QString strInfo = QObject::tr("ObstacleDist unknown:") + QString::number(warnInfoObstacleDist);
                if (1 <= warnInfoObstacleDist && warnInfoObstacleDist <= 10)
                {
                    strInfo = QObject::tr("ObstacleDist Level:") + QString::number(warnInfoObstacleDist);
                }
                else
                {
                    strInfo = QObject::tr("ObstacleDist unknown");
                }
                SendMsgToAll_Command(QObject::tr("Warn ObstacleDist"), strInfo, false);
#endif
            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_WarnInfo_WarnInfoRemainEnergy:
        {
            bool ok = false;
            const int warnInfoRemainEnergy = pAction->property("LYSD_Value").toInt(&ok);
            if (ok)
            {
                int nID = GetTrackID(pTrack);
                SendManualCommand(nID, NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFOREMAINENERGY, warnInfoRemainEnergy);
#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
                QString strInfo = QObject::tr("RemainEnergy:") + QString::number(warnInfoRemainEnergy);
                if (1 <= warnInfoRemainEnergy && warnInfoRemainEnergy <= 100)
                {
                    strInfo = QObject::tr("RemainEnergy:") + QString::number(warnInfoRemainEnergy);
                }
                else
                {
                    strInfo = QObject::tr("RemainEnergy unknown");
                }
                SendMsgToAll_Command(QObject::tr("Warn ObstacleDist"), strInfo, false);
#endif
            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_WarnInfo_WarnInfoRadio:
        {
            bool ok = false;
            const int warnInfoRadio = pAction->property("LYSD_Value").toInt(&ok);
            if (ok)
            {
                int nID = GetTrackID(pTrack);
                SendManualCommand(nID, NETHEADER_UCTYPE_INTERVENCE_CONTROL_WARNINFORADIO, warnInfoRadio);
#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
                QString strInfo = QObject::tr("Radio unknown:") + QString::number(warnInfoRadio);

                if (warnInfoRadio == 1)
                {
                    strInfo = QObject::tr("Beyond communication");
                }
                else if (warnInfoRadio == 2)
                {
                    strInfo = QObject::tr("Be disturbed");
                }
                else if (warnInfoRadio == 3)
                {
                    strInfo = QObject::tr("Poor communication");
                }
                else if (warnInfoRadio == 4)
                {
                    strInfo = QObject::tr("Communication fault");
                }
                else
                {
                    strInfo = QObject::tr("Radio unknown");
                }
                SendMsgToAll_Command(QObject::tr("Warn ObstacleDist"), strInfo, false);
#endif
            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_AmmoRemain_AmmoRemain:
        {
            bool ok = false;
            const int ammoRemain = pAction->property("LYSD_Value").toInt(&ok);
            if (ok)
            {
                int nID = GetTrackID(pTrack);
                SendManualCommand(nID, NETHEADER_UCTYPE_INTERVENCE_CONTROL_AMMOREMAIN, ammoRemain);
#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
                QString strInfo = QObject::tr("Ammo unknown:") + QString::number(ammoRemain);
                if (1 <= ammoRemain && ammoRemain <= 10)
                {
                    strInfo = QString::number(ammoRemain);
                }
                else
                {
                    strInfo = QObject::tr("Ammo unknown");
                }
                SendMsgToAll_Command(QObject::tr("Ammo Remain"), strInfo, false);
#endif
            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_LoadType_LoadType:
        {
            bool ok = false;
            const int loadType = pAction->property("LYSD_Value").toInt(&ok);
            if (ok)
            {
                int nID = GetTrackID(pTrack);
                SendManualCommand(nID, NETHEADER_UCTYPE_INTERVENCE_CONTROL_LOADTYPE, loadType);
#ifdef USE_LYSD_DATAMANAGE_SYSTEMINFO_SYSTEMINFO_SHOW
                QString strInfo = QObject::tr("Load unknown:") + QString::number(loadType);
                if (loadType == 1)
                {
                    strInfo = QObject::tr("GaoDe54");
                }
                else if (loadType == 2)
                {
                    strInfo = QObject::tr("GaoDe");
                }
                else if (loadType == 3)
                {
                    strInfo = QObject::tr("YunHanTong");
                }
                else if (loadType == 4)
                {
                    strInfo = QObject::tr("InfraredAiming54");
                }
                else
                {
                    strInfo = QObject::tr("Load unknown");
                }
                SendMsgToAll_Command(QObject::tr("Load Type"), strInfo, false);
#endif
            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_FullTrack_ScoutView:
            if (pTrack->SensorViewShow == 0)
            {
                pTrack->SensorViewShow = 1;
                m_WeiBiaoViewShowStringListMap["SensorViewHide"].removeAll(QString::number(pTrack->unNodeID));
                SaveWeiBiaoViewShowConfigureINI();
            }
            else
            {
                pTrack->SensorViewShow = 0;
                m_WeiBiaoViewShowStringListMap["SensorViewHide"].append(QString::number(pTrack->unNodeID));
                SaveWeiBiaoViewShowConfigureINI();
            }
            pTrack->SensorViewShowChange = 1;
            break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_FullTrack_BallisticLine:
            if (pTrack->m_MarkVehicleCalShow == 0)
            {
                pTrack->m_MarkVehicleCalShow = 1;
            }
            else
            {
                pTrack->m_MarkVehicleCalShow = 0;
            }
            pTrack->m_MarkVehicleCalShowChange = 1;
            break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_FullTrack_BallisticInformation:
            if (pTrack->m_MarkVehicleCalLabelShow == 0)
            {
                pTrack->m_MarkVehicleCalLabelShow = 1;
            }
            else
            {
                pTrack->m_MarkVehicleCalLabelShow = 0;
            }
            break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_FullTrack_ClearTrackLine:
            if (pTrack->m_lsPoints.size() > 1)
            {
                pTrack->m_lsPoints.erase(pTrack->m_lsPoints.begin(), --pTrack->m_lsPoints.end());
            }
            break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Synergy_PlannedSynergy:
        {

            qDebug() << tr2("弹出 计划协同 对话框") << pTrack->unNodeNameSimple;
            auto *pDlgPlannedSynergy = LyXyhc::Get().getDlgPlannedSynergy();
            if (pDlgPlannedSynergy != nullptr)
            {
                if (pDlgPlannedSynergy->isVisible())
                {
                    pDlgPlannedSynergy->hide();

                }
                pDlgPlannedSynergy->SetLineItemText(0, pTrack->unNodeNameSimple);
                pDlgPlannedSynergy->SetLineItemText(1, QStringLiteral("%1 %2").arg(pTrack->m_Longitude, 0, 'f', 6).arg(pTrack->m_Latitude, 0, 'f', 6));

                pDlgPlannedSynergy->SetLineItemText(2, QStringLiteral("%1 %2").arg(pTrack->m_nHangXiang, 0, 'f', 2).arg(pTrack->m_nSuDu, 0, 'f', 2));

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {

                    pDlgPlannedSynergy->SetLineItemText(3, QStringLiteral("%1").arg(pTrack->m_dMarkVehicleDist3D, 0, 'f', 2));

                    pDlgPlannedSynergy->SetLineItemText(4, QStringLiteral("%1").arg(pTrack->m_dMarkH, 0, 'f', 2));
                }
                else
                {
                    pDlgPlannedSynergy->SetLineItemText(3, QStringLiteral("-"));
                    pDlgPlannedSynergy->SetLineItemText(4, QStringLiteral("-"));
                }

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {
                    if (((pTrack->m_nMarkID & 0xFF) - 1) < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size()
                            && IXyhcActivator::Get().getPIMarkManageService() != nullptr)
                    {
                        const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
                        if (pIMarkManageService != nullptr)
                        {
                            const QString targetID = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][((pTrack->m_nMarkID & 0xFF) - 1)];
                            const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                            if (!targetItem.GetID().empty())
                            {
                                pDlgPlannedSynergy->SetLineItemText(5,
                                                                    QObject::tr(targetItem.GetID().c_str())
                                                                    + ", " + QObject::tr(ESystemMarkTypeToString(targetItem.GetType()))
                                                                    + ", " + QObject::tr(ESystemMarkMaterialToString(targetItem.GetMaterial()))
                                                                   );
                            }
                        }
                    }

                }
                else
                {
                    pDlgPlannedSynergy->SetLineItemText(5, QStringLiteral("-"));
                }

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {
                    pDlgPlannedSynergy->SetLineItemText(6, QStringLiteral("%1 %2").arg(pTrack->m_dMarkX, 0, 'f', 6).arg(pTrack->m_dMarkY, 0, 'f', 6));
                }
                else
                {
                    pDlgPlannedSynergy->SetLineItemText(6, QStringLiteral("-"));
                }

                pDlgPlannedSynergy->SetLineItemText(7, QString(QObject::tr("laser")));

                pDlgPlannedSynergy->SetLineItemText(8, "-");
                pDlgPlannedSynergy->show();

            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Synergy_ZoneSynergy:
        {
            auto *pDlgZoneSynergy = LyXyhc::Get().getPDlgZoneSynergy();
            if (pDlgZoneSynergy != nullptr)
            {
                if (pDlgZoneSynergy->isVisible())
                {
                    pDlgZoneSynergy->hide();

                }
                pDlgZoneSynergy->SetLineItemText(0, pTrack->unNodeNameSimple);
                pDlgZoneSynergy->SetLineItemText(1, QStringLiteral("%1 %2").arg(pTrack->m_Longitude, 0, 'f', 6).arg(pTrack->m_Latitude, 0, 'f', 6));

                pDlgZoneSynergy->SetLineItemText(2, QStringLiteral("%1 %2").arg(pTrack->m_nHangXiang, 0, 'f', 2).arg(pTrack->m_nSuDu, 0, 'f', 2));

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {

                    pDlgZoneSynergy->SetLineItemText(3, QStringLiteral("%1").arg(pTrack->m_dMarkVehicleDist3D, 0, 'f', 2));

                    pDlgZoneSynergy->SetLineItemText(4, QStringLiteral("%1").arg(pTrack->m_dMarkH, 0, 'f', 2));
                }
                else
                {
                    pDlgZoneSynergy->SetLineItemText(3, QStringLiteral("-"));
                    pDlgZoneSynergy->SetLineItemText(4, QStringLiteral("-"));
                }

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {
                    if (((pTrack->m_nMarkID & 0xFF) - 1) < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size()
                            && IXyhcActivator::Get().getPIMarkManageService() != nullptr)
                    {
                        const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
                        if (pIMarkManageService != nullptr)
                        {
                            const QString targetID = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][((pTrack->m_nMarkID & 0xFF) - 1)];
                            const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                            if (!targetItem.GetID().empty())
                            {
                                pDlgZoneSynergy->SetLineItemText(5,
                                                                     QObject::tr(targetItem.GetID().c_str())
                                                                     + ", " + QObject::tr(ESystemMarkTypeToString(targetItem.GetType()))
                                                                     + ", " + QObject::tr(ESystemMarkMaterialToString(targetItem.GetMaterial()))
                                                                    );
                            }
                        }
                    }
                }
                else
                {
                    pDlgZoneSynergy->SetLineItemText(5, QStringLiteral("-"));
                }

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {
                    pDlgZoneSynergy->SetLineItemText(6, QStringLiteral("%1 %2").arg(pTrack->m_dMarkX, 0, 'f', 6).arg(pTrack->m_dMarkY, 0, 'f', 6));
                }
                else
                {
                    pDlgZoneSynergy->SetLineItemText(6, QStringLiteral("-"));
                }

                pDlgZoneSynergy->SetLineItemText(7, QString(QObject::tr("laser")));

                pDlgZoneSynergy->SetLineItemText(8, "-");
                pDlgZoneSynergy->show();

            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Synergy_TimeSynergy:
        {

            qDebug() << tr2("弹出 时间协同 对话框") << pTrack->unNodeNameSimple;
            auto *pDlgTimeSynergy = LyXyhc::Get().getPDlgTimeSynergy();
            if (pDlgTimeSynergy != nullptr)
            {
                if (pDlgTimeSynergy->isVisible())
                {
                    pDlgTimeSynergy->hide();

                }
                pDlgTimeSynergy->SetLineItemText(0, pTrack->unNodeNameSimple);
                pDlgTimeSynergy->SetLineItemText(1, QStringLiteral("%1 %2").arg(pTrack->m_Longitude, 0, 'f', 6).arg(pTrack->m_Latitude, 0, 'f', 6));

                pDlgTimeSynergy->SetLineItemText(2, QStringLiteral("%1 %2").arg(pTrack->m_nHangXiang, 0, 'f', 2).arg(pTrack->m_nSuDu, 0, 'f', 2));

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {

                    pDlgTimeSynergy->SetLineItemText(3, QStringLiteral("%1").arg(pTrack->m_dMarkVehicleDist3D / pTrack->m_nSuDu, 0, 'f', 2));

                    pDlgTimeSynergy->SetLineItemText(4, QStringLiteral("%1").arg(pTrack->m_dMarkH, 0, 'f', 2));
                }
                else
                {
                    pDlgTimeSynergy->SetLineItemText(3, QStringLiteral("-"));
                    pDlgTimeSynergy->SetLineItemText(4, QStringLiteral("-"));
                }

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {
                    if (((pTrack->m_nMarkID & 0xFF) - 1) < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size()
                            && IXyhcActivator::Get().getPIMarkManageService() != nullptr)
                    {
                        const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
                        if (pIMarkManageService != nullptr)
                        {
                            const QString targetID = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][((pTrack->m_nMarkID & 0xFF) - 1)];
                            const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                            if (!targetItem.GetID().empty())
                            {
                                pDlgTimeSynergy->SetLineItemText(5,
                                                                 QObject::tr(targetItem.GetID().c_str())
                                                                 + ", " + QObject::tr(ESystemMarkTypeToString(targetItem.GetType()))
                                                                 + ", " + QObject::tr(ESystemMarkMaterialToString(targetItem.GetMaterial()))
                                                                );
                            }
                        }
                    }

                }
                else
                {
                    pDlgTimeSynergy->SetLineItemText(5, QStringLiteral("-"));
                }

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {
                    pDlgTimeSynergy->SetLineItemText(6, QStringLiteral("%1 %2").arg(pTrack->m_dMarkX, 0, 'f', 6).arg(pTrack->m_dMarkY, 0, 'f', 6));
                }
                else
                {
                    pDlgTimeSynergy->SetLineItemText(6, QStringLiteral("-"));
                }

                pDlgTimeSynergy->SetLineItemText(7, QString(QObject::tr("laser")));

                pDlgTimeSynergy->SetLineItemText(8, "-");
                pDlgTimeSynergy->show();

            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Synergy_InformationSupportSynergy:
        {
            auto *pDlgInfoSynergy = LyXyhc::Get().getPDlgInfoSynergy();
            if (pDlgInfoSynergy != nullptr)
            {
                if (pDlgInfoSynergy->isVisible())
                {
                    pDlgInfoSynergy->hide();

                }

                pDlgInfoSynergy->SetLineItemText(0, pTrack->unNodeNameSimple);

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {
                    if (((pTrack->m_nMarkID & 0xFF) - 1) < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size()
                            && IXyhcActivator::Get().getPIMarkManageService() != nullptr)
                    {
                        const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
                        if (pIMarkManageService != nullptr)
                        {
                            const QString targetID = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][((pTrack->m_nMarkID & 0xFF) - 1)];
                            const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                            if (!targetItem.GetID().empty())
                            {
                                double dLon = targetItem.GetLon();
                                double dLat = targetItem.GetLat();
                                QString strText = QObject::tr(targetItem.GetID().c_str()) + ", " + QObject::tr(targetItem.GetName().c_str());
                                pDlgInfoSynergy->SetLineItemText(1, strText);

                                double x1(0), y1(0), x2(0), y2(0);
                                IProj *pProj = LyXyhc::Get().GetProj();
                                if (pProj != nullptr)
                                {
                                    QPoint screenpoint;
                                    screenpoint = pProj->Jwd2Screen(dLon, dLat);

                                    pProj->jwxy(dLon, dLat, &x1, &y1);
                                    strText = QObject::tr("Logical coordinates") + ": " + QString::number(x1, 'f', 6) + ", " + QString::number(y1, 'f', 6);
                                    pDlgInfoSynergy->SetLineItemText(2, strText);

                                    pProj->Screen2Meter(screenpoint, x2, y2);
                                    strText = QObject::tr("Cartesian coordinates") + ": " + QString::number(x2, 'f', 6) + ", " + QString::number(y2, 'f', 6);
                                    pDlgInfoSynergy->SetLineItemText(3, strText);

                                    strText = QObject::tr("Geographic coordinates") + ": " + QString::number(dLon, 'f', 6) + ", " + QString::number(dLat, 'f', 6);
                                    pDlgInfoSynergy->SetLineItemText(4, strText);
                                }
                            }
                        }
                    }
                }
                else
                {
                    pDlgInfoSynergy->SetLineItemText(1, QStringLiteral("-"));
                    pDlgInfoSynergy->SetLineItemText(2, QStringLiteral("-"));
                    pDlgInfoSynergy->SetLineItemText(3, QStringLiteral("-"));
                    pDlgInfoSynergy->SetLineItemText(4, QStringLiteral("-"));
                }
                pDlgInfoSynergy->SetLineItemText(5, QString(QObject::tr("in progress")));
                pDlgInfoSynergy->SetLineItemText(6, QString(QObject::tr("implement")));
                pDlgInfoSynergy->InitCombobox();
                pDlgInfoSynergy->show();

            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Synergy_ScanSynergy:
        {
            auto *pDlgScanSynergy01 = LyXyhc::Get().getPDlgScanSynergy01();
            if (pDlgScanSynergy01 != nullptr)
            {
                if (pDlgScanSynergy01->isVisible())
                {
                    pDlgScanSynergy01->hide();

                }

                pDlgScanSynergy01->SetLineItemText(0, pTrack->unNodeNameSimple);
                pDlgScanSynergy01->SetLineItemText(1, QString(QObject::tr("1km")));
                pDlgScanSynergy01->SetLineItemText(2, QString(QObject::tr("1m")));
                pDlgScanSynergy01->SetLineItemText(3, QString(QString::number(pTrack->m_nHangXiang, 'f', 2)));
                pDlgScanSynergy01->SetLineItemText(4, QString(QObject::tr("0")));
                pDlgScanSynergy01->SetLineItemText(5, QString(QObject::tr("0%")));
                pDlgScanSynergy01->SetLineItemText(6, QString(QObject::tr("0%")));
                pDlgScanSynergy01->InitEdit();
                pDlgScanSynergy01->show();

            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Synergy_LeadTransitionSynergy:
        {
            auto *pDlgScanSynergy02 = LyXyhc::Get().getPDlgScanSynergy02();
            if (pDlgScanSynergy02 != nullptr)
            {
                if (pDlgScanSynergy02->isVisible())
                {
                    pDlgScanSynergy02->hide();

                }
                pDlgScanSynergy02->SetLineItemText(0, pTrack->unNodeNameSimple);

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {
                    if (((pTrack->m_nMarkID & 0xFF) - 1) < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size()
                            && IXyhcActivator::Get().getPIMarkManageService() != nullptr)
                    {
                        const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
                        if (pIMarkManageService != nullptr)
                        {
                            const QString targetID = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][((pTrack->m_nMarkID & 0xFF) - 1)];
                            const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                            if (!targetItem.GetID().empty())
                            {
                                pDlgScanSynergy02->SetLineItemText(1,
                                                                   QObject::tr(targetItem.GetID().c_str())
                                                                   + ", " + QObject::tr(ESystemMarkTypeToString(targetItem.GetType()))
                                                                   + ", " + QObject::tr(ESystemMarkMaterialToString(targetItem.GetMaterial()))
                                                                  );
                            }
                        }
                    }
                }
                else
                {
                    pDlgScanSynergy02->SetLineItemText(1, QStringLiteral("-"));
                }

                pDlgScanSynergy02->SetLineItemText(2, QString(QObject::tr("1km near the target")));
                pDlgScanSynergy02->SetLineItemText(3, QStringLiteral("%1,%2,1").arg(pTrack->m_nHangXiang, 0, 'f', 2).arg(pTrack->Pitch, 0, 'f', 2));
                pDlgScanSynergy02->SetLineItemText(4, QStringLiteral("%1").arg(pTrack->m_dMarkVehicleDist3D, 0, 'f', 2));
                pDlgScanSynergy02->SetLineItemText(5, QString(QObject::tr("60°")));
                pDlgScanSynergy02->SetLineItemText(6, QString(QObject::tr("Discover and monitor")));
                pDlgScanSynergy02->SetLineItemText(7, QString(QObject::tr("0time")));
                pDlgScanSynergy02->SetLineItemText(8, QString(QObject::tr("0%")));
                pDlgScanSynergy02->InitWindow();
                pDlgScanSynergy02->show();

            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Synergy_TrackingMarkSynergy:
        {
            auto *pDlgFireSynergy01 = LyXyhc::Get().getPDlgFireSynergy01();
            if (pDlgFireSynergy01 != nullptr)
            {
                if (pDlgFireSynergy01->isVisible())
                {
                    pDlgFireSynergy01->hide();

                }
                pDlgFireSynergy01->SetLineItemText(0, pTrack->unNodeNameSimple);
                pDlgFireSynergy01->SetLineItemText(1, QString(QObject::tr("-")));
                pDlgFireSynergy01->SetLineItemText(2, QString(QObject::tr("nothing")));
                pDlgFireSynergy01->SetLineItemText(3, QString(QObject::tr("100%")));
                pDlgFireSynergy01->SetLineItemText(4, QString(QObject::tr("01")));
                pDlgFireSynergy01->SetLineItemText(5, QString(QObject::tr("smooth")));
                pDlgFireSynergy01->SetLineItemText(6, QString(QObject::tr("MB001")));
                pDlgFireSynergy01->SetLineItemText(7, QString(QObject::tr("10㎡")));
                pDlgFireSynergy01->SetLineItemText(8, QString(QObject::tr("80%")));
                pDlgFireSynergy01->InitWindow();

                pDlgFireSynergy01->show();

            }
        }
        break;
        case LyXyhc::MapMenuCommandOper::MapMenuCommandOper_Synergy_GuidanceControlInfoSynergy:
        {
            auto *pDlgFireSynergy02 = LyXyhc::Get().getPDlgFireSynergy02();
            if (pDlgFireSynergy02 != nullptr)
            {
                if (pDlgFireSynergy02->isVisible())
                {
                    pDlgFireSynergy02->hide();

                }

                pDlgFireSynergy02->SetLineItemText(0, pTrack->unNodeNameSimple);
                pDlgFireSynergy02->SetLineItemText(1, QString(QObject::tr("KY001")));

                if (pTrack->m_nMarkAssign == 1 || pTrack->m_nMarkAssign == 2)
                {
                    if (((pTrack->m_nMarkID & 0xFF) - 1) < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2].size()
                            && IXyhcActivator::Get().getPIMarkManageService() != nullptr)
                    {
                        const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
                        if (pIMarkManageService != nullptr)
                        {
                            const QString targetID = LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 2][((pTrack->m_nMarkID & 0xFF) - 1)];
                            const auto &targetItem = pIMarkManageService->GetMarkFromDB(targetID);
                            if (!targetItem.GetID().empty())
                            {
                                pDlgFireSynergy02->SetLineItemText(2, QObject::tr(targetItem.GetID().c_str())
                                                                   + ", " + QObject::tr(ESystemMarkTypeToString(targetItem.GetType()))
                                                                   + ", " + QObject::tr(ESystemMarkMaterialToString(targetItem.GetMaterial()))
                                                                  );
                                pDlgFireSynergy02->SetLineItemText(3, QObject::tr(targetItem.GetID().c_str()));

                                double x1(0), y1(0), x2(0), y2(0);
                                IProj *pProj = LyXyhc::Get().GetProj();
                                if (pProj != nullptr)
                                {
                                    double dLon = targetItem.GetLon();
                                    double dLat = targetItem.GetLat();

                                    QPoint screenpoint;
                                    screenpoint = pProj->Jwd2Screen(dLon, dLat);

                                    pProj->jwxy(dLon, dLat, &x1, &y1);
                                    QString strText = QObject::tr("Logical coordinates") + ": " + QString::number(x1, 'f', 6) + ", " + QString::number(y1, 'f', 6);
                                    pDlgFireSynergy02->SetLineItemText(5, strText);

                                    pProj->Screen2Meter(screenpoint, x2, y2);
                                    strText = QObject::tr("Cartesian coordinates") + ": " + QString::number(x2, 'f', 6) + ", " + QString::number(y2, 'f', 6);
                                    pDlgFireSynergy02->SetLineItemText(6, strText);

                                    strText = QObject::tr("Geographic coordinates") + ": " + QString::number(dLon, 'f', 6) + ", " + QString::number(dLat, 'f', 6);
                                    pDlgFireSynergy02->SetLineItemText(4, strText);
                                }
                            }
                        }
                    }
                }
                else
                {
                    pDlgFireSynergy02->SetLineItemText(2, QStringLiteral("-"));
                    pDlgFireSynergy02->SetLineItemText(3, QStringLiteral("-"));
                    pDlgFireSynergy02->SetLineItemText(4, QStringLiteral("-"));
                    pDlgFireSynergy02->SetLineItemText(5, QStringLiteral("-"));
                    pDlgFireSynergy02->SetLineItemText(6, QStringLiteral("-"));
                }
                pDlgFireSynergy02->SetLineItemText(7, QString(QObject::tr("AssistAttack")));
                pDlgFireSynergy02->SetLineItemText(8, QString(QObject::tr("Success")));
                pDlgFireSynergy02->InitWindow();

                pDlgFireSynergy02->show();

            }
        }
        break;
        default:
            break;
        }
    }
    else if (id >= 9 && id < 65535)
    {
        std::vector<int> vi;
        int nS = 24;
        vi.push_back(nS);
        for (int i = 0; i < LyXyhc::Get().m_vslAttributeItem.size(); i++)
        {
            nS += LyXyhc::Get().m_vslAttributeItem[i].size();
            vi.push_back(nS);
        }
        int nID = GetTrackID(pTrack);
        if (id < 25)
        {
            pTrack->m_status.m_cColor = id - 9;
            pTrack->m_status.UserColor = 1;
            pTrack->m_Color = GetColor(pTrack->m_status.m_cColor).rgba();
        }
        else if (id <= vi[1])
        {
            pTrack->Type_Second = id - vi[0];
            SendManualCommand(nID, 1, pTrack->Type_Second);
        }
        else if (id <= vi[2])
        {
            pTrack->Model_Second = id - vi[1];
            SendManualCommand(nID, 2, pTrack->Model_Second);
        }
        else if (id <= vi[3])
        {
            pTrack->IFF_Second = id - vi[2];
            SendManualCommand(nID, 3, pTrack->IFF_Second);
        }
        else if (id <= vi[4])
        {
            pTrack->State_Second = id - vi[3];
            SendManualCommand(nID, 4, pTrack->State_Second);
        }
        else if (id <= vi[5])
        {
            pTrack->Task_Second = id - vi[4];
            SendManualCommand(nID, 5, pTrack->Task_Second);
        }
        else if (id <= vi[6])
        {
            pTrack->Formation_Second = id - vi[5];
            SendManualCommand(nID, 6, pTrack->Formation_Second);
        }
        else if (id <= vi[7])
        {
            pTrack->Power = id - vi[6];
            SendManualCommand(nID, 7, pTrack->Power);
        }
        else if (id <= vi[8])
        {
            pTrack->RemainEnergy = id - vi[7];
            SendManualCommand(nID, 8, pTrack->RemainEnergy);
        }
        else if (id <= vi[9])
        {

            SendManualCommand(nID, 9, id - vi[8]);
        }
        else if (id <= vi[10])
        {

            const int nMarkID = (1 << 15) | ((id - vi[9]) & 0xFF);
            SendManualCommand(nID, 128, nMarkID);

        }
        else if (id <= vi[11])
        {
            if ((LyXyhc::Get().m_vslAttributeItem.size() - 1 >= 0)
                    && (LyXyhc::Get().m_vslAttributeItem.size() - 1 < LyXyhc::Get().m_vslAttributeItem.size())
                    && (((id - vi[10]) & 0xFF) - 1 >= 0)
                    && (((id - vi[10]) & 0xFF) - 1 < LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 1].size())
               )
            {
                int d1 = 0, d2 = 0;
                sscanf(LyXyhc::Get().m_vslAttributeItem[LyXyhc::Get().m_vslAttributeItem.size() - 1][((id - vi[10]) & 0xFF) - 1].toLatin1().data(),
                       "%d.%d", &d1, &d2);

                const int nMarkID = ((d1 & 0x7F) << 8) | (d2 & 0xFF);
                SendManualCommand(nID, 128, nMarkID);

            }
        }
        else if (id == (vi[11] + 1))
        {
            pTrack->m_nMarkID = 0;
            memset(pTrack->m_strMarkID, 0, sizeof(pTrack->m_strMarkID));

            SendManualCommand(nID, 128, pTrack->m_nMarkID);
            SendManualCommand(nID, 128, pTrack->m_nMarkID);

        }
        else if (id == (vi[11] + 2))
        {
            pTrack->Pilot = m_localPilot;
            SendManualCommand(nID, 11, pTrack->Pilot);
        }
        else if (id == (vi[11] + 3))
        {
            QString msg = QObject::tr("Do you want to revoke the control of flight controller [%1]over[%2.%3]?").arg(LyXyhc::Get().GetPilotName(pTrack->Pilot)).arg((nID >> 8) & 0xFF).arg(nID & 0xFF);
            if (QMessageBox::Yes == LyMessageBox::question(NULL, QObject::tr("confirm"), msg, QMessageBox::Yes | QMessageBox::No))
            {
                SendManualCommand(nID, 13, pTrack->Pilot);
                LyXyhc::Get().ExecSql(QStringLiteral("DELETE FROM pilotUav WHERE uav=%1").arg(nID));
            }
        }
    }
    else
    {
        switch (id)
        {
        case 0:
        {
            pTrack->m_status.WeiBiao = !pTrack->m_status.WeiBiao;

            if (pTrack->m_status.WeiBiao)
            {
                m_WeiBiaoViewShowStringListMap["WeiBiaoViewShow"].append(QString::number(pTrack->unNodeID));
                SaveWeiBiaoViewShowConfigureINI();
            }
            else
            {
                m_WeiBiaoViewShowStringListMap["WeiBiaoViewShow"].removeAll(QString::number(pTrack->unNodeID));
                SaveWeiBiaoViewShowConfigureINI();
            }
        }
        break;
        case 1:
            pTrack->m_status.XianHangJi = !pTrack->m_status.XianHangJi;
            break;
        case 2:
            if (pTrack->m_nDisplayCount < 4096)
            {
                pTrack->m_nDisplayCount += 5;
            }
            pTrack->m_status.QuanHangJi = 0;
            break;
        case 3:
            if (pTrack->m_nDisplayCount > 9)
            {
                pTrack->m_nDisplayCount -= 5;
            }
            pTrack->m_status.QuanHangJi = 0;
            break;
        case 4:
            pTrack->m_nDisplayCount = 5;
            pTrack->m_status.QuanHangJi = 0;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            pTrack->m_status.QuanHangJi = id - 5;
            break;
        default:
            break;
        }
    }
}

void CTrackMgr::Tidy()
{
    for (TrackMap::iterator it = m_Track.begin(); it != m_Track.end(); it++)
    {
        QTrack &track = it->second;
        if (track.m_status.XiaoShi > 1)
        {
            track.m_status.XiaoShi--;
        }
    }
}

int CTrackMgr::AddReportMenu(QTrack *pTrack)
{
    int nRole = LyXyhcInterface::Get().GetSettings().value("User/Seat").toInt();
    if (nRole == 2)
    {
        return pTrack->NoReport + 1;
    }
    return 0;
}

void CTrackMgr::UpdateDrawm_Track(unsigned char ucZH, unsigned char ucPH)
{

    int nID = (ucZH << 8) | ucPH;
    auto it = m_Track.find(nID);
    if (it != m_Track.end())
    {
        m_Track.erase(it);
    }

}

void CTrackMgr::ReadWeiBiaoViewShowConfigureINI()
{
    QSettings settingsWeiBiaoViewShow("../data/conf/WeiBiaoViewShow.ini", QSettings::IniFormat);
    settingsWeiBiaoViewShow.setIniCodec("UTF-8");
    settingsWeiBiaoViewShow.beginGroup("WeiBiaoViewShowSettings");

    if (settingsWeiBiaoViewShow.contains("WeiBiaoViewShow"))
    {
        const QVariant weiBiaoViewShowTemp = settingsWeiBiaoViewShow.value("WeiBiaoViewShow", false);
        if (weiBiaoViewShowTemp.canConvert(QMetaType::QStringList))
        {
            const QStringList weiBiaoViewShowStringList = weiBiaoViewShowTemp.toStringList();
            m_WeiBiaoViewShowStringListMap.insert("WeiBiaoViewShow", weiBiaoViewShowStringList);
        }
    }

    if (settingsWeiBiaoViewShow.contains("WeiBiaoViewOffSetPosition"))
    {
        const QVariant weiBiaoViewOffSetPositionTemp = settingsWeiBiaoViewShow.value("WeiBiaoViewOffSetPosition", false);
        if (weiBiaoViewOffSetPositionTemp.canConvert(QMetaType::QStringList))
        {
            const QStringList weiBiaoViewOffSetPositionStringList = weiBiaoViewOffSetPositionTemp.toStringList();
            m_WeiBiaoViewShowStringListMap.insert("WeiBiaoViewOffSetPosition", weiBiaoViewOffSetPositionStringList);
        }
    }

    if (settingsWeiBiaoViewShow.contains("WeiBiaoViewColor"))
    {
        const QVariant weiBiaoViewColorTemp = settingsWeiBiaoViewShow.value("WeiBiaoViewColor", false);
        if (weiBiaoViewColorTemp.canConvert(QMetaType::QStringList))
        {
            const QStringList weiBiaoViewColorStringList = weiBiaoViewColorTemp.toStringList();
            m_WeiBiaoViewShowStringListMap.insert("WeiBiaoViewColor", weiBiaoViewColorStringList);
        }
    }

    if (settingsWeiBiaoViewShow.contains("SensorViewHide"))
    {
        const QVariant sensorViewTemp = settingsWeiBiaoViewShow.value("SensorViewHide", false);
        if (sensorViewTemp.canConvert(QMetaType::QStringList))
        {
            const QStringList sensorViewStringList = sensorViewTemp.toStringList();
            m_WeiBiaoViewShowStringListMap.insert("SensorViewHide", sensorViewStringList);
        }
    }

    settingsWeiBiaoViewShow.endGroup();

    settingsWeiBiaoViewShow.beginGroup("TelemetryWidgetShowSettings");
    if (settingsWeiBiaoViewShow.contains("TelemetryWidgetShow"))
    {
        const QVariant telemetryWidgetShowTemp = settingsWeiBiaoViewShow.value("TelemetryWidgetShow", false);
        if (telemetryWidgetShowTemp.canConvert(QMetaType::Bool))
        {
            const bool telemetryWidgetShow = telemetryWidgetShowTemp.toBool();
            m_pTelemetryWidgetShow = telemetryWidgetShow;
        }
    }
    settingsWeiBiaoViewShow.endGroup();
}

void CTrackMgr::SaveWeiBiaoViewShowConfigureINI()
{
    QSettings settingsWeiBiaoViewShow("../data/conf/WeiBiaoViewShow.ini", QSettings::IniFormat);
    settingsWeiBiaoViewShow.setIniCodec("UTF-8");
    settingsWeiBiaoViewShow.beginGroup("WeiBiaoViewShowSettings");

    settingsWeiBiaoViewShow.setValue("WeiBiaoViewShow", m_WeiBiaoViewShowStringListMap["WeiBiaoViewShow"]);
    settingsWeiBiaoViewShow.setValue("WeiBiaoViewOffSetPosition", m_WeiBiaoViewShowStringListMap["WeiBiaoViewOffSetPosition"]);
    settingsWeiBiaoViewShow.setValue("WeiBiaoViewColor", m_WeiBiaoViewShowStringListMap["WeiBiaoViewColor"]);
    settingsWeiBiaoViewShow.setValue("SensorViewHide", m_WeiBiaoViewShowStringListMap["SensorViewHide"]);

    settingsWeiBiaoViewShow.endGroup();

    settingsWeiBiaoViewShow.beginGroup("TelemetryWidgetShowSettings");
    settingsWeiBiaoViewShow.setValue("TelemetryWidgetShow", m_pTelemetryWidgetShow);
    settingsWeiBiaoViewShow.endGroup();
}

IUserMesh2 *CreateSightingLine(unsigned int l, int a, int b)
{
    IUserMesh2 *pUM = nullptr;
    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != nullptr)
    {
        pUM = pEarth->GetGraphics()->CreateUserMesh2();
    }
    a = abs(a);
    if (b == 0)
    {
        b = a;
    }
    a >>= 1;
    b >>= 1;
    int nSize = 5;
    VERTEX *pVerter = (VERTEX *)malloc(nSize * sizeof(VERTEX));
    if (pVerter != nullptr)
    {
        int i;
        pVerter[0].x = 0;
        pVerter[0].y = 0;
        pVerter[0].z = 0;
        pVerter[1].x = -a;
        pVerter[1].y = -b;
        pVerter[1].z = l;
        pVerter[2].x = -a;
        pVerter[2].y = b;
        pVerter[2].z = l;
        pVerter[3].x = a;
        pVerter[3].y = b;
        pVerter[3].z = l;
        pVerter[4].x = a;
        pVerter[4].y = -b;
        pVerter[4].z = l;
        pUM->SetPoints(nSize, pVerter);
		unsigned short *pIndex = (unsigned short *)malloc((nSize + 3) * sizeof(short));
        if (pIndex != NULL)
        {
            for (i = 0; i < nSize; i++)
            {
                pIndex[i] = i;
            }
            pUM->SetIndexs(i, pIndex, IUserMesh::USERMESH_TRIANGLE_FAN, false);
            pUM->SetIndexs(i - 1, pIndex + 1, IUserMesh::USERMESH_POLYGON);
            free(pIndex);
        }
        free(pVerter);
    }

    return pUM;
}

IUserMesh2 *CreateParabola(float v, int h, float angle)
{
    IUserMesh2 *pUM = nullptr;
    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != nullptr)
    {
        pUM = pEarth->GetGraphics()->CreateUserMesh2();
    }

    angle = angle * PI / 180;
    double dVh = v * sin(angle);
    double dVx = v * cos(angle);
    double t = (dVh + sqrt(dVh * dVh + 2 * 9.8 * h)) / 9.8;
    double dX = dVx * t;
    int nSize = ceil(t);
    VERTEX *pVerter = (VERTEX *)malloc((nSize + 1) * sizeof(VERTEX) * 2);
    if (pVerter != nullptr)
    {
        int i;
        int nI = 0;
        for (i = 0; i < nSize; i++)
        {
            float fS = -dVx * i;
            pVerter[nI].x = 0;
            pVerter[nI].y = fS;
            pVerter[nI].z = -dVh * i + 9.8 * i * i / 2;
            nI++;
            pVerter[nI].x = 0;
            pVerter[nI].y = fS;
            pVerter[nI].z = h;
            nI++;
        }
        pVerter[nI].x = 0;
        pVerter[nI].y = -dX;
        pVerter[nI].z = h - 1E-6;
        nI++;
        pVerter[nI].x = 0;
        pVerter[nI].y = -dX;
        pVerter[nI].z = h;
        nI++;
        pUM->SetPoints(nI, pVerter, IUserMesh::USERMESH_QUAD_STRIP);

        free(pVerter);
        pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
        pUM->SetState(IUserMesh::LINEWIDTH, 3);
    }
    return pUM;
}

IUserMesh2 *CreateParabola2(float v, int h, float angle)
{
    angle = angle * PI / 180;
    double dVh = v * sin(angle);
    double t = (dVh + sqrt(dVh * dVh + 2 * 9.8 * h)) / 9.8;
    double dX = v * cos(angle) * t;
    double dZ = h;
    double dM = 10;

    static int snBombR = 0;
    static int snBombH = 0;
    static int snBombPrecision = 0;
    if (snBombR == 0)
    {
        QString strIni = LyXyhc::Get().GetConfDir();
        strIni += "ly.ini";
        QFile file(strIni);
        if (file.exists())
        {
            QSettings ini(strIni, QSettings::IniFormat);
            ini.beginGroup("TrackMgr");
            snBombR = ini.value("BombR", "20").toInt();
            snBombH = ini.value("BombH", "50").toInt();
            snBombPrecision = ini.value("BombPrecision", "30000").toInt();
        }
    }

    CCreateShape cs;
    SHPObject *pShp = cs.CreateCircle(0, -dX, snBombR, snBombPrecision, true, &dM, &dZ);
    IUserMesh2 *pUM = nullptr;
    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != nullptr && pShp != nullptr)
    {
        IGraphics *pGrp = pEarth->GetGraphics();
        if (pGrp != nullptr)
        {

            pUM = pGrp->CreateUserMesh2();
            int nSize = pShp->nVertices + 1;
            VERTEX *pVerter = (VERTEX *)malloc(nSize * sizeof(VERTEX));
            if (pVerter != nullptr)
            {
                int i;
                pVerter[0].x = 0;
                pVerter[0].y = -dX;
                pVerter[0].z = h - snBombH;
                for (i = 0; i < pShp->nVertices; i++)
                {
                    pVerter[i + 1].x = pShp->padfX[i];
                    pVerter[i + 1].y = pShp->padfY[i];
                    pVerter[i + 1].z = h;
                }
                pUM->SetPoints(nSize, pVerter);
				unsigned short *pIndex = (unsigned short *)malloc((nSize + 3) * sizeof(short));
                if (pIndex != nullptr)
                {
                    for (i = 0; i <= pShp->nVertices; i++)
                    {
                        pIndex[i] = i;
                    }
                    pUM->SetIndexs(i, pIndex, IUserMesh::USERMESH_TRIANGLE_FAN, false);
                    pUM->SetIndexs(i - 1, pIndex + 1, IUserMesh::USERMESH_POLYGON);
                    free(pIndex);
                }
                free(pVerter);
                pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
            }
        }
    }
    SHPDestroyObject(pShp);
    return pUM;
}

#ifdef USING_LY_CCIP_80MM_DEFINE
IUserMesh1 *UpdateUavMarkTrack_80mm(IUserMesh1 *pUM,
                                      const double dLongitudeUav, const double dLatitudeUav, const double dAltitudeUav,
                                      const double dLongitudeMark, const double dLatitudeMark, const double dAltitudeMark)
{
    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
    {
        if (pUM != nullptr)
        {
            pEarth->GetGraphics()->RemoveObject(pUM);
        }
    }

    IUserMesh1 *pUM2 = nullptr;
    if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
    {
        pUM2 = pEarth->GetGraphics()->CreateUserMesh1();
    }

    if (pUM2 != nullptr)
    {
        VERTEX pVerter[2];

        int nI = 0;
        pVerter[nI].x = dLongitudeUav;
        pVerter[nI].y = dLatitudeUav;
        pVerter[nI].z = dAltitudeUav;
        nI++;

        pVerter[nI].x = dLongitudeMark;
        pVerter[nI].y = dLatitudeMark;
        pVerter[nI].z = dAltitudeMark;
        nI++;

        if (nI > 0)
        {
            pUM2->SetPoints(nI, pVerter, IUserMesh::USERMESH_LINES);
        }
        pUM2->SetState(IUserMesh::POINT, IUserMesh::USERMESH_LINE);
        pUM2->SetState(IUserMesh::LINEWIDTH, 2);
        pUM2->SetState(IUserMesh::LINESTIPPLE, 5);
    }

    return pUM2;
}

IUserMesh2 *CreateParabola_80mm(const double dLongitude, const double dLatitude, const double dAltitude,
                                const double dVehicleSpeed, const double dRelAltitude, const double dPitchAngle,
                                double &x, double &y, double &z)
{
    IUserMesh2 *pUM = nullptr;
    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
    {
        pUM = pEarth->GetGraphics()->CreateUserMesh2();
    }

    x = 0;
    y = 0;
    z = 0;

    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(dLongitude, dLatitude, dAltitude, dVehicleSpeed, dRelAltitude, dPitchAngle, ccipdata_80mmlist);

    const unsigned long long ccipdata_80mmlistSize = ccipdata_80mmlist.size();
    if (pUM != nullptr && ccipdata_80mmlistSize > 0 && ccipdata_80mmlistSize < 10000)
    {
        const auto &ccipdata_80mmlistLastData = ccipdata_80mmlist.back();
        x = ccipdata_80mmlistLastData.y;
        y = ccipdata_80mmlistLastData.x;
        z = dAltitude - std::fabs(ccipdata_80mmlistLastData.zd);

        const double dAlt = std::fabs(ccipdata_80mmlistLastData.zd);

        int nI = 0;

        const int ccipdata_80mmlistSizeSumVLine = 5;
        const int ccipdata_80mmlistSizeStep = (ccipdata_80mmlistSize / ccipdata_80mmlistSizeSumVLine + 1);

        std::vector<VERTEX> pVerter;

        {
            for (const auto &data : ccipdata_80mmlist)
            {

                if (nI % ccipdata_80mmlistSizeStep == 0)
                {
                    pVerter.push_back(VERTEX{ 0, 0, 0 });
                    pVerter[nI].x = 0;
                    pVerter[nI].y = data.x;
                    pVerter[nI].z = data.zd;
                    nI++;

                    pVerter.push_back(VERTEX{ 0, 0, 0 });
                    pVerter[nI].x = 0;
                    pVerter[nI].y = data.x;
                    pVerter[nI].z = -dAlt;
                    nI++;
                }

                pVerter.push_back(VERTEX{ 0, 0, 0 });
                pVerter[nI].x = 0;
                pVerter[nI].y = data.x;
                pVerter[nI].z = data.zd;
                nI++;
            }

            if (nI > 0)
            {
                pUM->SetPoints(nI, pVerter.data(), IUserMesh::USERMESH_LINE_STRIP);
            }
            pVerter.clear();

            pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
            pUM->SetState(IUserMesh::LINEWIDTH, 3);
        }
    }

    return pUM;
}

IUserMesh2 *UpdateParabola_80mm(IUserMesh2 *pUM,
                                const double dLongitude, const double dLatitude, const double dAltitude,
                                const double dVehicleSpeed, const double dRelAltitude, const double dPitchAngle,
                                double &x, double &y, double &z)
{
    if (pUM == nullptr)
    {
        return nullptr;
    }

    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
    {
        pEarth->GetGraphics()->RemoveObject(pUM);
    }

    IUserMesh2 *pUM2 = nullptr;
    if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
    {
        pUM2 = pEarth->GetGraphics()->CreateUserMesh2();
    }

    x = 0;
    y = 0;
    z = 0;

    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(dLongitude, dLatitude, dAltitude, dVehicleSpeed, dRelAltitude, dPitchAngle, ccipdata_80mmlist);

    const unsigned long long ccipdata_80mmlistSize = ccipdata_80mmlist.size();
    if (pUM2 != nullptr && ccipdata_80mmlistSize > 0 && ccipdata_80mmlistSize < 10000)
    {
        const auto &ccipdata_80mmlistLastData = ccipdata_80mmlist.back();
        x = ccipdata_80mmlistLastData.y;
        y = ccipdata_80mmlistLastData.x;
        z = dAltitude - std::fabs(ccipdata_80mmlistLastData.zd);

        const double dAlt = std::fabs(ccipdata_80mmlistLastData.zd);

        int nI = 0;

        const int ccipdata_80mmlistSizeSumVLine = 5;
        const int ccipdata_80mmlistSizeStep = (ccipdata_80mmlistSize / ccipdata_80mmlistSizeSumVLine + 1);

        std::vector<VERTEX> pVerter;

        {
            for (const auto &data : ccipdata_80mmlist)
            {

                if (nI % ccipdata_80mmlistSizeStep == 0)
                {
                    pVerter.push_back(VERTEX{ 0, 0, 0 });
                    pVerter[nI].x = 0;
                    pVerter[nI].y = data.x;
                    pVerter[nI].z = data.zd;
                    nI++;

                    pVerter.push_back(VERTEX{ 0, 0, 0 });
                    pVerter[nI].x = 0;
                    pVerter[nI].y = data.x;
                    pVerter[nI].z = -dAlt;
                    nI++;
                }

                pVerter.push_back(VERTEX{ 0, 0, 0 });
                pVerter[nI].x = 0;
                pVerter[nI].y = data.x;
                pVerter[nI].z = data.zd;
                nI++;
            }

            if (nI > 0)
            {
                pUM2->SetPoints(nI, pVerter.data(), IUserMesh::USERMESH_LINE_STRIP);
            }
            pVerter.clear();

            pUM2->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
            pUM2->SetState(IUserMesh::LINEWIDTH, 3);
        }
    }

    return pUM2;
}

IUserMesh2 *CreateParabola2_80mm(const double x, const double y, const double z)
{
    double dX = y;
    double dZ = z;
    double dM = 10;

    static int snBombR = 0;
    static int snBombH = 0;
    static int snBombPrecision = 0;
    if (snBombR == 0)
    {
        QString strIni = LyXyhc::Get().GetConfDir();
        strIni += "ly.ini";
        QFile file(strIni);
        if (file.exists())
        {
            QSettings ini(strIni, QSettings::IniFormat);
            ini.beginGroup("TrackMgr");
            snBombR = ini.value("BombR", "20").toInt();
            snBombH = ini.value("BombH", "50").toInt();
            snBombPrecision = ini.value("BombPrecision", "30000").toInt();
        }
    }

    CCreateShape cs;
    SHPObject *pShp = cs.CreateCircle(0, -dX, snBombR, snBombPrecision, true, &dM, &dZ);
    IUserMesh2 *pUM = nullptr;
    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != nullptr && pShp != nullptr)
    {
        IGraphics *pGrp = pEarth->GetGraphics();
        if (pGrp != nullptr)
        {
            pUM = pGrp->CreateUserMesh2();
            int nSize = pShp->nVertices + 1;

            std::vector<VERTEX> pVerter;

            {
                int i;
                pVerter.push_back(VERTEX{ 0, 0, 0 });
                pVerter[0].x = 0;
                pVerter[0].y = -dX;
                pVerter[0].z = z - snBombH;
                for (i = 0; i < pShp->nVertices; i++)
                {
                    pVerter.push_back(VERTEX{ 0, 0, 0 });
                    pVerter[i + 1].x = pShp->padfX[i];
                    pVerter[i + 1].y = pShp->padfY[i];
                    pVerter[i + 1].z = z;
                }
                pUM->SetPoints(nSize, pVerter.data());

                std::vector<unsigned short> pIndex;

                {
                    for (i = 0; i <= pShp->nVertices; i++)
                    {
                        pIndex.push_back(0);
                        pIndex[i] = i;
                    }
                    pUM->SetIndexs(i, pIndex.data(), IUserMesh::USERMESH_TRIANGLE_FAN, false);
                    pUM->SetIndexs(i - 1, pIndex.data() + 1, IUserMesh::USERMESH_POLYGON);

                    pIndex.clear();
                }

                pVerter.clear();
                pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
            }
        }
    }
    SHPDestroyObject(pShp);
    return pUM;
}

IUserMesh2 *UpdateParabola2_80mm(IUserMesh2 *pUM, const double x, const double y, const double z)
{
    if (pUM == nullptr)
    {
        return nullptr;
    }

    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != nullptr && pEarth->GetGraphics() != nullptr)
    {
        pEarth->GetGraphics()->RemoveObject(pUM);
    }

    double dX = y;
    double dZ = 0;
    double dM = 0;

    static int snBombR = 0;
    static int snBombH = 0;
    static int snBombPrecision = 0;
    if (snBombR == 0)
    {
        QString strIni = LyXyhc::Get().GetConfDir();
        strIni += "ly.ini";
        QFile file(strIni);
        if (file.exists())
        {
            QSettings ini(strIni, QSettings::IniFormat);
            ini.beginGroup("TrackMgr");
            snBombR = ini.value("BombR", "20").toInt();
            snBombH = ini.value("BombH", "50").toInt();
            snBombPrecision = ini.value("BombPrecision", "30000").toInt();
        }
    }

    CCreateShape cs;
    SHPObject *pShp = cs.CreateCircle(0, -dX, snBombR, snBombPrecision, true, &dM, &dZ);
    IUserMesh2 *pUM2 = nullptr;
    if (pEarth != nullptr && pShp != nullptr)
    {
        IGraphics *pGrp = pEarth->GetGraphics();
        if (pGrp != nullptr)
        {
            pUM2 = pGrp->CreateUserMesh2();
            int nSize = pShp->nVertices + 1;

            std::vector<VERTEX> pVerter;

            {
                int i;
                pVerter.push_back(VERTEX{ 0, 0, 0 });
                pVerter[0].x = 0;
                pVerter[0].y = -dX;
                pVerter[0].z = z - snBombH;

                pVerter[0].x = pShp->padfX[0];
                pVerter[0].y = pShp->padfY[0];
                pVerter[0].z = 0;

                for (i = 0; i < pShp->nVertices; i++)
                {
                    pVerter.push_back(VERTEX{ 0, 0, 0 });
                    pVerter[i + 1].x = pShp->padfX[i];
                    pVerter[i + 1].y = pShp->padfY[i];
                    pVerter[i + 1].z = 0;
                }

                pUM2->SetPoints(nSize, pVerter.data(), IUserMesh::USERMESH_LINE_STRIP);

                pVerter.clear();
                pUM2->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
            }
        }
    }
    SHPDestroyObject(pShp);
    return pUM2;
}

#endif

