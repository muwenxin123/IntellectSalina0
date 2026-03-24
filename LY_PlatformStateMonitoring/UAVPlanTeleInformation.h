#ifndef UAVPLANTELEINFORMATION_H
#define UAVPLANTELEINFORMATION_H

#include <QString>
#include "UAVPlatformStateDialog.h"
#include "DataManage/NebulaUAVState/NebulaUAVState.h"

#pragma execution_character_set("utf-8")

#define UAV_NORMAL_STATE_STR  UAVPlatformStateDialog::tr("Normal")
#define UAV_WARING_STATE_STR  UAVPlatformStateDialog::tr("Abnormal")

#define UAV_STATES_ITEM_RC      1
#define UAV_STATES_ITEM_MC      (1<<1)
#define UAV_STATES_ITEM_Power   (1<<2)
#define UAV_STATES_ITEM_Sensor  (1<<3)
#define UAV_STATES_ITEM_GPS     (1<<4)
#define UAV_STATES_ITEM_Over    (1<<5)
#define UAV_STATES_ITEM_YJ      (1<<6)
#define UAV_STATES_ITEM_GD      (1<<7)

#define NebulaUAVState_RC                     (1<<0)
#define NebulaUAVState_MC                     (1<<1)
#define NebulaUAVState_Power                  (1<<2)
#define NebulaUAVState_Sensor                 (1<<3)
#define NebulaUAVState_GPS                    (1<<4)
#define NebulaUAVState_OverBorder             (1<<5)
#define NebulaUAVState_Emergency              (1<<6)
#define NebulaUAVState_INS                    (1<<7)
#define NebulaUAVState_SatelliteDirection     (1<<8)
#define NebulaUAVState_Ultrasound             (1<<9)
#define NebulaUAVState_RotationRate           (1<<10)

typedef enum _ECommState
{
    ECommState_Normal = 0,
    ECommState_TimeOut,
    ECommState_Disconnect
} ECommState;

const QString ECommStateToString(ECommState info);

typedef enum _EPlatformState
{
    EPlatformState_Null = 0,
    EPlatformState_Normal,
    EPlatformState_Err
} EPlatformState;

const QString EPlatformStateToString(EPlatformState info);

typedef enum _ELoadedupState
{
    ELoadedupState_Normal = 0,
    ELoadedupState_Err
} ELoadedupState;

typedef enum _ECtrlMode
{
    ECtrlMode_ZZ = 0,
    ECtrlMode_YK,
    ECtrlMode_ZW,

    ECtrlMode_Custom = NEBULAUAV_ECTRLMODE_CUSTOM
} ECtrlMode;

const QString ECtrlModeToString(ECtrlMode eMode);

typedef enum _EFlyPhase
{
    EFlyPhase_Ready = 0,
    EFlyPhase_PreFly,
    EFlyPhase_ReachSafeHeight,
    EFlyPhase_ReachWorkHeight,
    EFlyPhase_BeginCruise,
    EFlyPhase_Return,
    EFlyPhase_Landing,
    EFlyPhase_NULL1,
    EFlyPhase_LandFinish,
    EFlyPhase_LockBarred
} EFlyPhase;

const QString EFlyPhaseToString(EFlyPhase ePhase);

typedef enum _ELockState
{
    ELockState_NULL = 0,
    ELockState_Pos,
    ELockState_Height,
    ELockState_Both,
    ELockState_RigthBarrier,
    ELockState_LeftBarrier,
    ELockState_BackBarrier,
    ELockState_FrontBarrier,
} ELockState;

ELockState GetLockState(unsigned char nValue);

const QString ELockStateToString(ELockState eState);
const QString ELockStateToString(unsigned char eState);

typedef enum _EDiffState
{
    EDiffState_Null                  = 0,
    EDiffState_SinglePoint           = 1,
    EDiffState_GPS                   = 2,
    EDiffState_RTK                   = 3,
    EDiffState_RTKFixedSolution      = 4,
    EDiffState_RTKFFloatingSolution  = 5,
} EDiffState;

const QString EDiffStateToString(EDiffState eState);

enum
{
    COLUMN_COMMSTATE,
    COLUMN_PLATFORMSTATE,

    COLUMN_UAVNAME,
    COLUMN_CTRLMODE,
    COLUMN_FLYPHASE,
    COLUMN_VOLTAGE,
    COLUMN_AMPERE,

    COLUMN_LONLAN,
    COLUMN_HEIGHT,
    COLUMN_VELOCITY,
    COLUMN_ATTITUDE,
    COLUMN_DIRECTION,
    COLUMN_LOCKSTATE,

    COLUMN_FLYDIATANCE,
    COLUMN_FLYTIME
};

typedef enum MESSAGE_IMPORTANCE
{
    msgImportanceNormal,
    msgImportanceHigh,
    msgImportanceLow
} MESSAGE_IMPORTANCE;

EPlatformState GetPlatformState(short nValue);

#endif
