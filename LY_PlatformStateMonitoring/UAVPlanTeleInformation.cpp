#include "UAVPlanTeleInformation.h"

EPlatformState GetPlatformState(short nValue)
{
    if (!(UAV_STATES_ITEM_RC & nValue))
    {
        return EPlatformState_Err;
    }
    else if (!(UAV_STATES_ITEM_MC & nValue))
    {
        return EPlatformState_Err;
    }
    else if (!(UAV_STATES_ITEM_Power & nValue))
    {
        return EPlatformState_Err;
    }
    else if (!(UAV_STATES_ITEM_Sensor & nValue))
    {
        return EPlatformState_Err;
    }
    else if (!(UAV_STATES_ITEM_GPS & nValue))
    {
        return EPlatformState_Err;
    }
    else if (!(UAV_STATES_ITEM_Over & nValue))
    {
        return EPlatformState_Err;
    }
    else if (!(UAV_STATES_ITEM_YJ & nValue))
    {
        return EPlatformState_Err;
    }
    else if (!(UAV_STATES_ITEM_GD & nValue))
    {
        return EPlatformState_Err;
    }
    else
    {
        return EPlatformState_Normal;
    }
    return EPlatformState_Err;
}

const QString EDiffStateToString(EDiffState eState)
{
    if (EDiffState_Null == eState)
    {
        return UAVPlatformStateDialog::tr("Invalid");
    }
    else if (EDiffState_SinglePoint == eState)
    {
        return UAVPlatformStateDialog::tr("Single Point");
    }
    else if (EDiffState_GPS == eState)
    {
        return UAVPlatformStateDialog::tr("GPS");
    }
    else if (EDiffState_RTK == eState)
    {
        return UAVPlatformStateDialog::tr("RTK");
    }
    else if (EDiffState_RTKFixedSolution == eState)
    {
        return UAVPlatformStateDialog::tr("RTK Fixed solution");
    }
    else if (EDiffState_RTKFFloatingSolution == eState)
    {
        return UAVPlatformStateDialog::tr("RTK Float solution");
    }
    else
    {
        return UAVPlatformStateDialog::tr("[Invalid]");
    }
    return UAVPlatformStateDialog::tr("[Invalid]");
}

ELockState GetLockState(unsigned char nValue)
{
    if (0x01 & nValue)
    {
        return ELockState_Height;
    }
    else if (0x02 & nValue)
    {
        return ELockState_Pos;
    }
    else if (0x04 & nValue)
    {
        return ELockState_Both;
    }
    else if (0x10 & nValue)
    {
        return ELockState_RigthBarrier;
    }
    else if (0x20 & nValue)
    {
        return ELockState_LeftBarrier;
    }
    else if (0x40 & nValue)
    {
        return ELockState_BackBarrier;
    }
    else if (0x80 & nValue)
    {
        return ELockState_FrontBarrier;
    }
    return ELockState_NULL;
}

const QString ELockStateToString(ELockState eState)
{
    if (ELockState_NULL == eState)
    {
        return UAVPlatformStateDialog::tr("[unLock]");
    }
    else if (ELockState_Pos == eState)
    {
        return UAVPlatformStateDialog::tr("[Position]");
    }
    else if (ELockState_Height == eState)
    {
        return UAVPlatformStateDialog::tr("Altitude");
    }
    else if (ELockState_Both == eState)
    {
        return UAVPlatformStateDialog::tr("Pos&Alt");
    }
    else if (ELockState_RigthBarrier == eState)
    {
        return UAVPlatformStateDialog::tr("right barrier");
    }
    else if (ELockState_LeftBarrier == eState)
    {
        return UAVPlatformStateDialog::tr("left barrier");
    }
    else if (ELockState_BackBarrier == eState)
    {
        return UAVPlatformStateDialog::tr("behind barrier");
    }
    else if (ELockState_FrontBarrier == eState)
    {
        return UAVPlatformStateDialog::tr("front barrier");
    }
    else
    {
        return UAVPlatformStateDialog::tr("[Invalid]");
    }
    return UAVPlatformStateDialog::tr("[Invalid]");
}

const QString ELockStateToString(unsigned char eState)
{
    QString elockstate("");
    if (0x01 & eState)
    {
        elockstate += UAVPlatformStateDialog::tr("Altitude");
    }
    else if (0x02 & eState)
    {
        elockstate += UAVPlatformStateDialog::tr("[Position]");
    }
    else if (0x04 &  eState)
    {
        elockstate += UAVPlatformStateDialog::tr("Pos&Alt");
    }
    else if (0x08 &  eState)
    {
        elockstate += QStringLiteral("");
    }
    else if (0x10 & eState)
    {
        elockstate += UAVPlatformStateDialog::tr("right barrier");
    }
    else if (0x20 & eState)
    {
        elockstate += UAVPlatformStateDialog::tr("left barrier");
    }
    else if (0x40 & eState)
    {
        elockstate += UAVPlatformStateDialog::tr("behind barrier");
    }
    else if (0x80 & eState)
    {
        elockstate += UAVPlatformStateDialog::tr("front barrier");
    }

    if (elockstate.isEmpty())
    {
        return UAVPlatformStateDialog::tr("[Invalid]");
    }

    return elockstate;
}

const QString EFlyPhaseToString(EFlyPhase ePhase)
{
    if (EFlyPhase_Ready == ePhase)
    {
        return UAVPlatformStateDialog::tr("Ready");
    }
    else if (EFlyPhase_PreFly == ePhase)
    {
        return UAVPlatformStateDialog::tr("To fly");
    }
    else if (EFlyPhase_ReachSafeHeight == ePhase)
    {
        return UAVPlatformStateDialog::tr("Reach safe altitude");
    }
    else if (EFlyPhase_ReachWorkHeight == ePhase)
    {
        return UAVPlatformStateDialog::tr("Reach work altitude");
    }
    else if (EFlyPhase_BeginCruise == ePhase)
    {
        return UAVPlatformStateDialog::tr("[Begin to cruise]");
    }
    else if (EFlyPhase_Return == ePhase)
    {
        return UAVPlatformStateDialog::tr("[Turn back]");
    }
    else if (EFlyPhase_Landing == ePhase)
    {
        return UAVPlatformStateDialog::tr("Landing");
    }
    else if (EFlyPhase_NULL1 == ePhase)
    {
        return UAVPlatformStateDialog::tr("NULL");
    }
    else if (EFlyPhase_LandFinish == ePhase)
    {
        return UAVPlatformStateDialog::tr("Landing finish");
    }
    else if (EFlyPhase_LockBarred == ePhase)
    {
        return UAVPlatformStateDialog::tr("Lock");
    }
    else
    {
        return UAVPlatformStateDialog::tr("[Invalid]");
    }
    return UAVPlatformStateDialog::tr("[Invalid]");
}

const QString ECtrlModeToString(ECtrlMode eMode)
{
    if (ECtrlMode_ZZ == eMode)
    {
        return UAVPlatformStateDialog::tr("[Auto]");
    }
    else if (ECtrlMode_YK == eMode)
    {
        return UAVPlatformStateDialog::tr("Remote control");
    }
    else if (ECtrlMode_ZW == eMode)
    {
        return UAVPlatformStateDialog::tr("Stabilization");
    }
    else
    {
        return UAVPlatformStateDialog::tr("[Invalid]");
    }
    return UAVPlatformStateDialog::tr("[Invalid]");
}

const QString EPlatformStateToString(EPlatformState info)
{
    if (EPlatformState_Null == info)
    {
        return UAVPlatformStateDialog::tr("Invalid");
    }
    else if (EPlatformState_Normal == info)
    {
        return UAVPlatformStateDialog::tr("Normal");
    }
    else if (EPlatformState_Err == info)
    {
        return UAVPlatformStateDialog::tr("Abnormal");
    }
    return UAVPlatformStateDialog::tr("[Invalid]");
}

const QString ECommStateToString(ECommState info)
{
    if (ECommState_Normal == info)
    {
        return UAVPlatformStateDialog::tr("Connect");
    }
    else if (ECommState_TimeOut == info)
    {
        return UAVPlatformStateDialog::tr("Timeout");
    }
    else if (ECommState_Disconnect == info)
    {
        return UAVPlatformStateDialog::tr("Disconnect");
    }
    return UAVPlatformStateDialog::tr("[Invalid]");
}
