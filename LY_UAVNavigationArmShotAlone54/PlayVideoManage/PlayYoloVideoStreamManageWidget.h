#ifndef PLAYYOLOVIDEOSTREAMMANAGEWIDGET_H
#define PLAYYOLOVIDEOSTREAMMANAGEWIDGET_H

#include "IPlayVideoManage.h"
#include "YoloVideoStreamManageRepc.h"

#include <QWidget>
#include <QMutex>
#include <QTimer>
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QLinkedList>
#include <QProcess>
#include <QDateTime>
#include <QThread>

#include "Protocol/XygsZC/Bag_XygsZCDefines.h"
#include "Protocol/XygsZC54/Bag_XygsZC54Defines.h"
#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"
#include "QGeoCoordinate"

namespace Ui
{
class PlayYoloVideoStreamManageWidget;
}

namespace qnzkna
{
namespace PlayVideoManage
{

class PlayYoloVideoStreamManageWidget : public IPlayVideoManage
{
    Q_OBJECT

public:
    explicit PlayYoloVideoStreamManageWidget(QWidget *parent = nullptr);
    virtual ~PlayYoloVideoStreamManageWidget();

    QWidget *GetPlayYoloVideoStreamManageWidgetParentWidget();

    virtual QPoint VideoWidgetPos() const override;
    virtual QSize VideoWidgetSize() const override;
    virtual std::string getStrFileName() const override;
    virtual void setStrFileName(const std::string &strFileName) override;

    virtual volatile bool getStop() const override;
    virtual void setStop(volatile bool Stop) override;

    virtual volatile bool getPause() const override;
    virtual void setPause(volatile bool Pause) override;

    virtual void Lock() override;
    virtual void UnLock() override;

    WId WinId() const;
    virtual void Show() override;
    virtual void setVideoType(int videoType) override;

    bool LaunchExternalSoftware(const QString &exePath, const QStringList &arguments = QStringList());
    void ExternalSoftwareProcessStarted();

    void GimbalMoveAndCalaDis();

	void GimbalMoveAndCalaDis2();

    void LaserMeasureDistance();

	void LaserMeasureDistance2();

	void calcMeasureDistance();

	//跟踪飞行
	void uavTrackFlyTimeOut();

	void ReadYoloVideoPodConfig();

	//画幅计算方式
	QGeoCoordinate CalcTargetPosFromPod(std::string ComponentID, XygsZC_Mark_01 xygsZC_Target);
	//FOV计算方式
	QGeoCoordinate CalcTargetPosFromPod1(std::string ComponentID, XygsZC_Mark_01 xygsZC_Target);

	void LoadSendUAVIPIni();

    void StartUAVsTimers();
	void StopUAVsTimers();

    void SendAMapPodControlMsg(std::string ComponentID, AMapPod54Control &aMapPod54Control);
    void SendAMapPodControlZeroMsg(std::string ComponentID);
    std::pair<float, float> getVehicleDeflectionDistYawPitch54Value(const std::string &strVehicleID);
    void getVehicleDeflectionDistYawPitch54ZoomDistanceValue(const std::string &strVehicleID, const QString &strSensorID, float &deflectionAngleofNose, float &deflectionAngleofPitch);

    virtual bool StartVideoManageObjectHost(const CVideoDevice &videoDevice) override;
    virtual bool StartVideoManageThread(const CVideoDevice &videoDevice) override;
    virtual bool StopVideoManageThread() override;
    virtual void StartVideoPlay() override;
    virtual void StopVideoPlay()  override;

    int CheckXygszcTargetMapChange(std::map<int, QVector<XygsZC_Mark_01>> XygsZcTargetMap);

	//机器狗跟踪行走计算
	void q20ATraTrackingObjectWalk(XygsZC_Mark_01& xygsZC_Target, int disObject = 5, float objectRealW = 0.5, float objectRealH = 1.8);
signals:
    void ShowImage(QImage image);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void on_pushButton_PlayYoloVideoStreamManageWidget_Start_clicked();

    void on_pushButton_PlayYoloVideoStreamManageWidget_Stop_clicked();

    void on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_1_clicked();

    void on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_2_clicked();

    void on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_3_clicked();

    void on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_4_clicked();

    void on_checkBox_PlayYoloVideoStreamManageWidget_Yolo_stateChanged(int arg1);

	void on_checkBox_Track_stateChanged();
public:

    typedef struct __AMapPod54ControlStatusInfo
    {
        AMapPod54ControlStatus    aMapPod54ControlStatus;
        bool                      selectedCurrentSensor;
        unsigned long long int    targetBatchTime;
        float32_t                 targetLongitude;
        float32_t                 targetLatitude;
        int16_t                   targetAltitude;
        bool                      targetBatchStart;
        bool                      targetBatch;
        bool                      laserStarted;
        bool                      laserDistanceContinueStarted;
        bool                      laserDistanceStarted;
        unsigned int              receivedCmdFeedback;
        time_t                    laserDistanceTimes;
        unsigned long int         laserDistanceCount;
        unsigned long int         Z;
        unsigned long int         P;
        QTimer                   *laserTimer;
        __AMapPod54ControlStatusInfo()
        {
            memset(&aMapPod54ControlStatus, 0, sizeof(aMapPod54ControlStatus));
            selectedCurrentSensor = false;
            targetBatchTime = 0;
            targetLongitude = 0;
            targetLatitude = 0;
            targetAltitude = 0;
            targetBatchStart = false;
            targetBatch = true;
            laserStarted = false;
            laserDistanceContinueStarted = false;
            laserDistanceStarted = false;
            receivedCmdFeedback = 0;
            laserDistanceTimes = 0;
            laserDistanceCount = 0;
            Z = 0;
            P = 0;
            laserTimer = nullptr;
        }
    } AMapPod54ControlStatusInfo;

private:
    Ui::PlayYoloVideoStreamManageWidget *ui;
    QMutex                               m_Mutex;
    WId                                  m_WId;
    CVideoDevice                         m_VideoDevice;

    QMutex							m_Mutex1;
    QMutex							m_MutexLaser;
	QMutex                           m_MtexUavTrack;
	bool                                 m_AddTargetToDB;
	double                               m_FocalDis;
	double                               m_LongFrame;
	double                               m_ShortFrame;
	//
	double m_FocalMin = 4.7;
	double m_FocalMax = 47;
	double m_ViewFrontMax = 36.9;
	double m_ViewFrontMin = 3.9;
	double m_ViewLeftMax = 61.3;
	double m_ViewLeftMin = 6.8;
	int m_MaxZoom = 10;
public:
    volatile bool                        m_Stop;
    volatile bool                        m_Pause;
    std::string                          m_strFileName;
    static const QString                 s_RemoteObjectPrefix;
    QProcess                            *m_ProcessExternalSoftware;
    YoloVideoStreamManageRepc            m_YoloVideoStreamManageRepc;
    unsigned int                         m_ProcessExternalSoftwareTimes;
    QTimer                               m_ProcessExternalSoftwareTimer;

    XygsZC_Mark_01				m_lastTargetInfo;
    std::map<unsigned long long, XygsZC_Mark_01>m_lastTargetInfoMap;
    std::map<QString, XygsZC_Mark_01> m_detectTargetMap;
    QStringList						  m_strDiscernUAVIDList;
    QTimer								  m_GimbalMoveTimer;
    QTimer								 m_laserMeasureTimer;
    std::map<unsigned long long, std::pair<bool, std::vector<QTimer *>>> m_UAVsTimerMap;
    int countTimes = 0;
    std::vector<unsigned long long> targetSetVec;

    QMap<QString, AMapPod54ControlStatusInfo>                        m_AMapPod54ControlStatusInfoMap;

	//laser
	QTimer *timerLaser = nullptr;
	std::map<QString, XygsZC_Mark_01> m_LaserTargetMap;

	//Track
	std::map<std::string, bool>uavTrackMap;
public:
    std::vector<XygsZC_Mark_01> m_laserTargetVec;
    std::map<unsigned long long, std::vector<XygsZC_Mark_01>>m_laserTargetMap;
    bool bIslser = false;
    std::map<unsigned long long, bool>bIslserMap;
    int nLaserDisIndex = 0;
    std::map<QString, std::pair<bool, int>> nLaserIndexMap;
    std::map<QString, QString> TargetID_PMap;

	std::map<int, QString> YoloConfigTypeMap;
	//指点
	std::map<QString,bool>SetPointTargetIdMap;

	//跟踪飞行
	QTimer *m_uavTrackTimer;
	std::vector<XygsZC_Mark_01> trackFlyVec;

    int calDIsIndex = 0;
    int moveIndex = 0;
    bool Indexdirection = true;
    int laserIndex = 0;

    int maxRectDis = 270;

    double dLonLast = 0.0;
    double dLatLast = 0.0;

	float m_Q20AxSendLast = 0.0;
	float m_Q20AySendLast = 0.0;
	unsigned long long int m_Q20AWeaponID;
	float FOV_w = 61.5;

};

}
}

#endif
