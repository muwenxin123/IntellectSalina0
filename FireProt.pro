TEMPLATE     = subdirs
DESTDIR += ../../bin
DEPENDPATH += .  forms  include  qrc  sources
CONFIG += qt warn_on release
CONFIG += debug_and_release
CONFIG += no_docs_target order

SUBDIRS += \
    LY_TinyXml \
    FireProt \
    LyUcisPluginFrameWork \
    LY_CollaborativeSearchService \
    lyXyhc \
    LY_CommandUnitManageService \
    LY_ConfigBundleControlService \
    LY_PlatformStateMonitoring \
    LY_PositionManageService \
    LY_ProtocolParse_XygsServer \
    LY_SensorManageService \
    LY_SystemMaintenanceService \
    LY_AmmunitionManageService \
    LY_MarkManageService \
    LY_TaskManage \
    LY_TaskManageService \
    LY_TaskPlatMatch \
    LY_UAVFlyControl_Mavlink \
    LY_UAVRealTimeStateManageService \
    LY_WeaponManageService \
    Measure \
    LY_IntellectImageShangBo \
    LY_UAVNavigationArmShotAloneFireProt \
    LY_ZoneManager \
    LY_AgriMachineManager


TRANSLATIONS += Qt_zh_CN.ts
