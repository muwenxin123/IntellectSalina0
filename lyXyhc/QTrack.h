#if !defined(AFX_QTRACK_H__B61A1A65_14F4_4B2A_A974_5A3953E334DD__INCLUDED_)
#define AFX_QTRACK_H__B61A1A65_14F4_4B2A_A974_5A3953E334DD__INCLUDED_

#include <list>

#include "DataManage/Mark/Mark.h"

struct TrackPt
{
    short	nH;
    time_t	nT;
    double	dX;
    double	dY;

    TrackPt()
    {
        memset(this, 0, sizeof(TrackPt));
    }
};

typedef std::list<TrackPt> TrackPtList;

class QTrack
{
public:
    QTrack();
    ~QTrack();
    void Reset();

private:
    QTrack(const QTrack &) = delete ;
    QTrack &operator=(const QTrack &) = delete ;

public:

    struct _Status
    {
        unsigned char Flag          : 1;
        unsigned char YanXi			: 1;
        unsigned char MoNiTuYan		: 1;
        unsigned char TingXian      : 1;
        unsigned char ZhanXiao      : 1;
        unsigned char XianHangJi    : 1;
        unsigned char ShanShuo      : 1;
        unsigned char ShanShuoZT    : 1;

        unsigned char UserColor     : 1;
        unsigned char WeiBiao       : 1;
        unsigned char Style         : 2;
        unsigned char QuanHangJi    : 2;
        unsigned char BaoChu		: 2;

        unsigned char XiaoShi		: 4;
        unsigned char m_cColor		: 4;
    } m_status;

    unsigned char	Power			: 4;
    unsigned char	RemainEnergy	: 4;

    unsigned char	IFF_First;
    unsigned char	IFF_Second;

    unsigned char	State_First;
    unsigned char	State_Second;

    unsigned char	Type_First;
    unsigned char	Type_Second;

    unsigned char	Model_First;
    unsigned char	Model_Second;

    unsigned char	Task_First;
    unsigned char	Task_Second;

    unsigned char	Formation_First;
    unsigned char	Formation_Second;
    char m_nOffsetX;
    char m_nOffsetY;

    signed int     WarnInfoTime_First;
    signed int     WarnInfoTime_Second;

    unsigned int   WarnInfoThreat_First;
    unsigned int   WarnInfoThreat_Second;

    unsigned int   WarnInfoObstacleDist_First;
    unsigned int   WarnInfoObstacleDist_Second;

    unsigned char  WarnInfoRemainEnergy_First;
    unsigned char  WarnInfoRemainEnergy_Second;

    unsigned char  WarnInfoRadio_First;
    unsigned char  WarnInfoRadio_Second;

    unsigned char  AmmoRemain_First;
    unsigned char  AmmoRemain_Second;

    unsigned char  LoadType_First;
    unsigned char  LoadType_Second;

    unsigned char	Pilot;
    unsigned char	unused1;
    unsigned char	unused2;
    unsigned char	NoReport;

    float           Pitch;
    float           Roll;

    unsigned short m_nDisplayCount;
    float          m_nGaoDu;
    float          m_nGaoDuRel;
    float          m_nSuDu;
    float          m_nHangXiang;
    double		   m_Longitude;
    double		   m_Latitude;

    float m_fMinX;
    float m_fMinY;
    float m_fMaxX;
    float m_fMaxY;

    unsigned int  m_Color;

    char          m_strMarkID[MAX_MARK_LEN_1];
    unsigned int  m_UavMarkTrackShowTimes;
    unsigned int  m_nMarkID;
    unsigned int  m_AutoAimingState;
    double        m_dMarkX;
    double        m_dMarkY;
    double        m_dMarkH;
    unsigned char m_MarkVehicleCalShow;
    unsigned char m_MarkVehicleCalLabelShow;
    unsigned char m_MarkVehicleCalShowChange;
    unsigned char m_MarkVehicleCalUnuse2;
    unsigned char m_nMarkVehicleCal;
    unsigned char m_nMarkVehicleCalLineDraw;
    unsigned char m_nMarkVehicleDistShow;
    unsigned char m_nMarkAssign;
    double        m_dMarkVehicleDist3D;
    double        m_dMarkVehicleDist;
    double        m_dMarkVehicleHeight;
    double        m_dMarkVehicleYaw;
    double        m_dMarkVehicleYawSame;
    double        m_dMarkVehicleMinX;
    double        m_dMarkVehicleMinY;

    unsigned char       unNodeType;
    unsigned char       unUnuse1;
    unsigned char       unUnuse2;
    unsigned char       unUnuse3;
    unsigned long long  unNodeID;
    char                unNodeNameSimple[24];

    unsigned char       SensorViewShow;
    unsigned char       SensorViewShowChange;
    unsigned char       SensorUnuse2;
    unsigned char       SensorUnuse3;
    float               SensorPitch;
    float               SensorRoll;
    float               SensorYaw;

    TrackPtList m_lsPoints;

    unsigned char getIFF() const
    {
        if (IFF_Second)
        {
            return IFF_Second;
        }
        return IFF_First;
    }
    unsigned char getState() const
    {
        if (State_Second)
        {
            return State_Second;
        }
        return State_First;
    }
    unsigned char getType() const
    {
        if (Type_Second)
        {
            return Type_Second;
        }
        return Type_First;
    }
    unsigned char getModel() const
    {
        if (Model_Second)
        {
            return Model_Second;
        }
        return Model_First;
    }
    unsigned char getTask() const
    {
        if (Task_Second)
        {
            return Task_Second;
        }
        return Task_First;
    }
    unsigned char getFormation() const
    {
        if (Formation_Second)
        {
            return Formation_Second;
        }
        return Formation_First;
    }
    unsigned char getWarnInfoTime() const
    {
        if (WarnInfoTime_Second)
        {
            return WarnInfoTime_Second;
        }
        return WarnInfoTime_First;
    }
    unsigned char getWarnInfoThreat() const
    {
        if (WarnInfoThreat_Second)
        {
            return WarnInfoThreat_Second;
        }
        return WarnInfoThreat_First;
    }
    unsigned char getWarnInfoObstacleDist() const
    {
        if (WarnInfoObstacleDist_Second)
        {
            return WarnInfoObstacleDist_Second;
        }
        return WarnInfoObstacleDist_First;
    }
    unsigned char getWarnInfoRemainEnergy() const
    {
        if (WarnInfoRemainEnergy_Second)
        {
            return WarnInfoRemainEnergy_Second;
        }
        return WarnInfoRemainEnergy_First;
    }
    unsigned char getWarnInfoRadio() const
    {
        if (WarnInfoRadio_Second)
        {
            return WarnInfoRadio_Second;
        }
        return WarnInfoRadio_First;
    }
    unsigned char getAmmoRemain() const
    {
        if (AmmoRemain_Second)
        {
            return AmmoRemain_Second;
        }
        return AmmoRemain_First;
    }
    unsigned char getLoadType() const
    {
        if (LoadType_Second)
        {
            return LoadType_Second;
        }
        return LoadType_First;
    }
};

#endif
