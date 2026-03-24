#if !defined(AFX_CTRACKMGR_H__B61A1A65_14F4_4B2A_A974_5A3953E334DD__INCLUDED_)
#define AFX_CTRACKMGR_H__B61A1A65_14F4_4B2A_A974_5A3953E334DD__INCLUDED_

#include <QMap>
#include <map>
#include "QTrack.h"
#include "track.h"
#include "QObject"
#include "LogRecord.h"
#include "Protocol/XygsZC/Bag_XygsZCDefines.h"
#include "socketutil.h"
#include "XygsZC_02_Process.h"
#include <map3d/IGraphics.h>

typedef std::map<int, QTrack> TrackMap;

#define OFFSET_RATE		5

class IProj;
class QAction;
class QPainter;
class QPoint;
class QTableWidget;

class TrackList;


class CTrackMgr : public QObject
{
    Q_OBJECT
public:
    CTrackMgr();
    ~CTrackMgr();

    void setPProj(IProj *pProj)
    {
        m_pProj = pProj;
    }
    void SaveWeiBiaoViewShowConfigureINITrackOffset(QTrack *pTrack);
    void Draw(QPainter &painter, IProj *pProj);
    bool Process(int nLength, char *lpData);
    bool Process_XygsZC_02(const unsigned long long &unNodeID, const XygsZC_02 XygsZC_02);
    QTrack *SearchTrack(const QPoint &pt);
    const QTrack *FindTrackNodeID(const unsigned long long &unNodeID);
    bool UpdateAutoAimingState(const unsigned long long unNodeID, const unsigned int autoAimingState);
    QTrack *FindTrack(unsigned char ucZH, unsigned char ucPH);
    void OnMenuAction(QTrack *pTrack, QAction *pAction);
    void Tidy();
    int AddReportMenu(QTrack *pTrack);
    void UpdateDrawm_Track(unsigned char ucZH, unsigned char ucPH);

signals:
    void sendMarkID(int);

private:
    void   ReadWeiBiaoViewShowConfigureINI();
    void   SaveWeiBiaoViewShowConfigureINI();
    double HeadAngle(const double angle) const;
    QTrack *GetTrack(unsigned char ucZH, unsigned char ucPH);
    QTrack *GetTrack(unsigned char ucZH, unsigned char ucPH, unsigned char unNodeType, unsigned long long	unNodeID, TrackInfo *pD);
    int GetTrackID(QTrack *pTrack);
    int IfZhNeedProcess(unsigned char ucZhanHao);
    int IfZhDisplay(unsigned char ucZhanHao);
    void ChangeUav3DColor(unsigned int Z, unsigned int P, const QColor &color);

	std::vector<IUserMesh2 *> CreateDetectionZone(float angle_l, float angle_w, float dis_m);

public:
    TrackMap                                  m_Track;
    std::map<unsigned long long, std::string> m_TrackName;
    QWidget                                  *m_pTelemetryWidget = nullptr;
    QTableWidget                             *m_pTelemetry       = nullptr;
    TrackList                                *m_TrackList        = nullptr;
    bool                                      m_pTelemetryWidgetShow;
    bool                                      m_pTelemetryWidgetShowDegree;

	static CLogRecordCVS                               s_TractorCVS;

private:
    XygsZC_02_Process                         m_XygsZC_02_Process;
    IProj                                    *m_pProj;
    int                                       m_localPilot;
    QMap<QString, QStringList>                m_WeiBiaoViewShowStringListMap;
	SocketUtil *udpSocket = nullptr;
	QString m_udpAddress = "";
	quint16 m_udpPort = 0;
};

#endif
