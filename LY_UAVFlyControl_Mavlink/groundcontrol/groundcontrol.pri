#-------------------------------------------------
#
# Project created by QtCreator 2015-09-19T17:37:16
#
#-------------------------------------------------

QT += core
QT += widgets
QT += positioning
QT += xml
QT += network

qtHaveModule(texttospeech): CONFIG += groundcontrol_texttospeech

groundcontrol_texttospeech {
    DEFINES += GROUNDCONTROL_TEXTTOSPEECH_ENABLE
    QT += texttospeech
}

CONFIG += c++11


DEFINES += NOMINMAX
DEFINES += NO_ARDUPILOT_DIALECT


SOURCE_DIR = $$PWD
LIBS_DIR   = $$PWD


linux {
    linux-g++ | linux-g++-64 | linux-g++-32 | linux-clang {
        message("Linux build")
        CONFIG  += LinuxBuild
        DEFINES += __STDC_LIMIT_MACROS
        DEFINES += QGC_GST_TAISYNC_ENABLED
        DEFINES += QGC_GST_MICROHARD_ENABLED
        linux-clang {
            message("Linux clang")
            QMAKE_CXXFLAGS += -Qunused-arguments -fcolor-diagnostics
        } else {
            #QMAKE_CXXFLAGS += -H # Handy for debugging why something is getting built when an include file is touched
            QMAKE_CXXFLAGS_WARN_ON += -Werror \
                -Wno-deprecated-copy \      # These come from mavlink headers
                -Wno-unused-parameter \     # gst_plugins-good has these errors
                -Wno-implicit-fallthrough   # gst_plugins-good has these errors
        }
    } else : linux-rasp-pi2-g++ {
        message("Linux R-Pi2 build")
        CONFIG += LinuxBuild
        DEFINES += __STDC_LIMIT_MACROS __rasp_pi2__
        DEFINES += QGC_GST_TAISYNC_ENABLED
        DEFINES += QGC_GST_MICROHARD_ENABLED
    } else : android-clang {
        CONFIG += AndroidBuild MobileBuild
        DEFINES += __android__
        DEFINES += __STDC_LIMIT_MACROS
        DEFINES += QGC_ENABLE_BLUETOOTH
        DEFINES += QGC_GST_TAISYNC_ENABLED
        DEFINES += QGC_GST_MICROHARD_ENABLED
        QMAKE_CXXFLAGS_WARN_ON += -Werror \
            -Wno-unused-parameter \             # gst_plugins-good has these errors
            -Wno-implicit-fallthrough \         # gst_plugins-good has these errors
            -Wno-unused-command-line-argument \ # from somewhere in Qt generated build files
            -Wno-parentheses-equality           # android gstreamer header files
        QMAKE_CFLAGS_WARN_ON += \
            -Wno-unused-command-line-argument   # from somewhere in Qt generated build files
        target.path = $$DESTDIR
        equals(ANDROID_TARGET_ARCH, armeabi-v7a)  {
            DEFINES += __androidArm32__
            message("Android Arm 32 bit build")
        } else:equals(ANDROID_TARGET_ARCH, arm64-v8a)  {
            DEFINES += __androidArm64__
            message("Android Arm 64 bit build")
        } else:equals(ANDROID_TARGET_ARCH, x86)  {
            CONFIG += Androidx86Build
            DEFINES += __androidx86__
            message("Android x86 build")
        } else {
            error("Unsupported Android architecture: $${ANDROID_TARGET_ARCH}")
        }
    } else {
        error("Unsuported Linux toolchain, only GCC 32- or 64-bit is supported")
    }
} else : win32 {
    contains(QMAKE_TARGET.arch, x86_64) {
        message("Windows build")
        CONFIG += WindowsBuild
        DEFINES += __STDC_LIMIT_MACROS
        DEFINES += QGC_GST_TAISYNC_ENABLED
        DEFINES += QGC_GST_MICROHARD_ENABLED
        QMAKE_CFLAGS -= -Zc:strictStrings
        QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
        QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO -= -Zc:strictStrings
        QMAKE_CXXFLAGS -= -Zc:strictStrings
        QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
        QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO -= -Zc:strictStrings
        QMAKE_CXXFLAGS_WARN_ON += /W3 \
            /wd4005 \   # silence warnings about macro redefinition, these come from the shapefile code with is external
            /wd4290 \   # ignore exception specifications
            /wd4267 \   # silence conversion from 'size_t' to 'int', possible loss of data, these come from gps drivers shared with px4
            /wd4100     # unreferenced formal parameter - gst-plugins-good
    } else {
        error("Unsupported Windows toolchain, only Visual Studio 2017 64 bit is supported")
    }
} else : macx {
    macx-clang | macx-llvm {
        message("Mac build")
        CONFIG  += MacBuild
        CONFIG  += x86_64
        CONFIG  -= x86
        DEFINES += QGC_GST_TAISYNC_ENABLED
        DEFINES += QGC_GST_MICROHARD_ENABLED
        QMAKE_CXXFLAGS += -fvisibility=hidden
        QMAKE_CXXFLAGS_WARN_ON += -Werror \
            -Wno-unused-parameter \         # gst-plugins-good
            -Wno-deprecated-declarations    # eigen
    } else {
        error("Unsupported Mac toolchain, only 64-bit LLVM+clang is supported")
    }
} else : ios {
    message("iOS build")
    CONFIG  += iOSBuild MobileBuild app_bundle NoSerialBuild
    CONFIG  -= bitcode
    DEFINES += __ios__
    DEFINES += QGC_NO_GOOGLE_MAPS
    DEFINES += NO_SERIAL_LINK
    DEFINES += QGC_DISABLE_UVC
    DEFINES += QGC_GST_TAISYNC_ENABLED
    QMAKE_IOS_DEPLOYMENT_TARGET = 11.0
    QMAKE_APPLE_TARGETED_DEVICE_FAMILY = 1,2 # Universal
    QMAKE_LFLAGS += -Wl,-no_pie
} else {
    error("Unsupported build platform, only Linux, Windows, Android and Mac (Mac OS and iOS) are supported")
}

# Enable ccache where we can
linux|macx|ios {
    system(which ccache) {
        message("Found ccache, enabling")
        !ios {
            QMAKE_CXX = ccache $$QMAKE_CXX
            QMAKE_CC  = ccache $$QMAKE_CC
        } else {
            QMAKE_CXX = $$PWD/tools/iosccachecc.sh
            QMAKE_CC  = $$PWD/tools/iosccachecxx.sh
        }
    }
}

!MacBuild:!AndroidBuild {
    # See QGCPostLinkCommon.pri for details on why MacBuild doesn't use DESTDIR
    # DESTDIR = staging
}

MobileBuild {
    DEFINES += __mobile__
}

StableBuild {
    message("Stable Build")
} else {
    message("Daily Build")
    DEFINES += DAILY_BUILD
}

INCLUDEPATH += \
    $$SOURCE_DIR \
    $$SOURCE_DIR/src \
    $$SOURCE_DIR/src/Audio \
    $$SOURCE_DIR/src/Geo \
    $$SOURCE_DIR/src/uas \
    $$SOURCE_DIR/src/Comm \
    $$SOURCE_DIR/src/Settings \
    $$SOURCE_DIR/src/Vehicle \
    $$SOURCE_DIR/src/FollowMe \
    $$SOURCE_DIR/src/AutoPilotPlugins \
    $$SOURCE_DIR/src/FirmwarePlugin \
    $$SOURCE_DIR/src/VehicleSetup \
    $$SOURCE_DIR/src/FactSystem \
    $$SOURCE_DIR/src/MissionManager \
    $$SOURCE_DIR/src/QmlControls \
    $$SOURCE_DIR/src/libs/eigen \
    $$SOURCE_DIR/src/Compression



SOURCES += \
    $$SOURCE_DIR/QGC.cc \
    $$SOURCE_DIR/JsonHelper.cc \
    $$SOURCE_DIR/KMLDomDocument.cpp \
    $$SOURCE_DIR/QGCApplication.cpp \
    $$SOURCE_DIR/QGCLoggingCategory.cc \
    $$SOURCE_DIR/QGCQGeoCoordinate.cc \
    $$SOURCE_DIR/QGCTemporaryFile.cc \
    $$SOURCE_DIR/QGCPalette.cpp \
    $$SOURCE_DIR/src/Audio/AudioOutput.cpp \
    $$SOURCE_DIR/src/Geo/QGCGeo.cc \
    $$SOURCE_DIR/src/Geo/UTMUPS.cpp \
    $$SOURCE_DIR/src/Geo/MGRS.cpp \
    $$SOURCE_DIR/src/Geo/Math.cpp \
    $$SOURCE_DIR/src/Geo/Utility.cpp \
    $$SOURCE_DIR/src/Geo/TransverseMercator.cpp \
    $$SOURCE_DIR/src/Geo/PolarStereographic.cpp \
    $$SOURCE_DIR/src/uas/UAS.cpp \
    $$SOURCE_DIR/src/uas/UASInterface.cpp \
    $$SOURCE_DIR/src/uas/UASMessageHandler.cpp \
    $$SOURCE_DIR/src/QmlControls/QGCGeoBoundingCube.cpp \
    $$SOURCE_DIR/src/QmlControls/QGroundControlQmlGlobal.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/AutoPilotPlugin.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/Generic/GenericAutoPilotPlugin.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/ESP8266Component.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/ESP8266ComponentController.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/MotorComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/RadioComponentController.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/SyslinkComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/SyslinkComponentController.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMAutoPilotPlugin.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMAirframeComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMAirframeComponentController.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMCameraComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMCompassCal.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMFlightModesComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMFlightModesComponentController.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMFollowComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMFollowComponentController.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMHeliComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMLightsComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMMotorComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMPowerComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMRadioComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSafetyComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSensorsComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSensorsComponentController.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSubFrameComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSubMotorComponentController.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMTuningComponent.cc \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4AutoPilotPlugin.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/ActuatorComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/AirframeComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/AirframeComponentAirframes.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/AirframeComponentController.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/CameraComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/FlightModesComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4AirframeLoader.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4FlightBehavior.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4RadioComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4SimpleFlightModesController.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4TuningComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PowerComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PowerComponentController.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/SafetyComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/SensorsComponent.cpp \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/SensorsComponentController.cpp \
    $$SOURCE_DIR/src/Comm/LinkConfiguration.cpp \
    $$SOURCE_DIR/src/Comm/LinkInterface.cpp \
    $$SOURCE_DIR/src/Comm/MAVLinkProtocol.cpp \
    $$SOURCE_DIR/src/Comm/MavlinkMessagesTimer.cpp \
    $$SOURCE_DIR/src/Comm/QGCMAVLink.cc \
    $$SOURCE_DIR/src/Comm/LocalLink.cpp \
    $$SOURCE_DIR/src/Compression/QGCLZMA.cc \
    $$SOURCE_DIR/src/Compression/QGCZlib.cc \
    $$SOURCE_DIR/src/FactSystem/Fact.cc \
    $$SOURCE_DIR/src/FactSystem/FactMetaData.cc \
    $$SOURCE_DIR/src/FactSystem/SettingsFact.cpp \
    $$SOURCE_DIR/src/FactSystem/FactSystem.cpp \
    $$SOURCE_DIR/src/FactSystem/FactPanelController.cpp \
    $$SOURCE_DIR/src/FactSystem/FactGroup.cpp \
    $$SOURCE_DIR/src/FactSystem/ParameterManager.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/CameraMetaData.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/FirmwarePlugin.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/FirmwarePluginManager.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/PX4/PX4FirmwarePlugin.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/PX4/PX4FirmwarePluginFactory.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/PX4/PX4ParameterMetaData.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/APMFirmwarePlugin.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/APMFirmwarePluginFactory.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/APMParameterMetaData.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/ArduCopterFirmwarePlugin.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/ArduPlaneFirmwarePlugin.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/ArduRoverFirmwarePlugin.cpp \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/ArduSubFirmwarePlugin.cpp \
    $$SOURCE_DIR/src/FollowMe/FollowMe.cc \
    $$SOURCE_DIR/src/MissionManager/PlanElementController.cpp \
    $$SOURCE_DIR/src/MissionManager/SimpleMissionItem.cpp \
    $$SOURCE_DIR/src/MissionManager/VisualMissionItem.cpp \
    $$SOURCE_DIR/src/MissionManager/QGCMapCircle.cpp \
    $$SOURCE_DIR/src/MissionManager/MissionCommandTree.cpp \
    $$SOURCE_DIR/src/MissionManager/MissionCommandList.cpp \
    $$SOURCE_DIR/src/MissionManager/MissionCommandUIInfo.cpp \
    $$SOURCE_DIR/src/MissionManager/MissionItem.cc \
    $$SOURCE_DIR/src/MissionManager/ComplexMissionItem.cpp \
    $$SOURCE_DIR/src/MissionManager/FixedWingLandingComplexItem.cpp \
    $$SOURCE_DIR/src/MissionManager/KMLPlanDomDocument.cpp \
    $$SOURCE_DIR/src/MissionManager/LandingComplexItem.cpp \
    $$SOURCE_DIR/src/MissionManager/MissionController.cpp \
    $$SOURCE_DIR/src/MissionManager/MissionManager.cpp \
    $$SOURCE_DIR/src/MissionManager/MissionSettingsItem.cpp \
    $$SOURCE_DIR/src/MissionManager/PlanManager.cpp \
    $$SOURCE_DIR/src/MissionManager/PlanMasterController.cpp \
    $$SOURCE_DIR/src/MissionManager/QGCMapPolygon.cpp \
    $$SOURCE_DIR/src/MissionManager/QGCMapPolyline.cpp \
    $$SOURCE_DIR/src/MissionManager/Section.cpp \
    $$SOURCE_DIR/src/MissionManager/SpeedSection.cpp \
    $$SOURCE_DIR/src/MissionManager/TakeoffMissionItem.cpp \
    $$SOURCE_DIR/src/MissionManager/VTOLLandingComplexItem.cpp \
    $$SOURCE_DIR/src/Settings/AppSettings.cpp \
    $$SOURCE_DIR/src/Settings/SettingsGroup.cpp \
    $$SOURCE_DIR/src/Settings/SettingsManager.cpp \
    $$SOURCE_DIR/src/Settings/UnitsSettings.cpp \
    $$SOURCE_DIR/src/Settings/APMMavlinkStreamRateSettings.cpp \
    $$SOURCE_DIR/src/Vehicle/CompInfo.cpp \
    $$SOURCE_DIR/src/Vehicle/CompInfoActuators.cpp \
    $$SOURCE_DIR/src/Vehicle/CompInfoEvents.cpp \
    $$SOURCE_DIR/src/Vehicle/CompInfoGeneral.cpp \
    $$SOURCE_DIR/src/Vehicle/CompInfoParam.cpp \
    $$SOURCE_DIR/src/Vehicle/ComponentInformationCache.cpp \
    $$SOURCE_DIR/src/Vehicle/ComponentInformationManager.cpp \
    $$SOURCE_DIR/src/Vehicle/FTPManager.cpp \
    $$SOURCE_DIR/src/Vehicle/InitialConnectStateMachine.cpp \
    $$SOURCE_DIR/src/Vehicle/StateMachine.cpp \
    $$SOURCE_DIR/src/Vehicle/HealthAndArmingCheckReport.cpp \
    $$SOURCE_DIR/src/Vehicle/MAVLinkStreamConfig.cpp \
    $$SOURCE_DIR/src/Vehicle/SysStatusSensorInfo.cpp \
    $$SOURCE_DIR/src/Vehicle/MultiVehicleManager.cpp \
    $$SOURCE_DIR/src/Vehicle/Vehicle.cpp \
    $$SOURCE_DIR/src/Vehicle/EventHandler.cpp \
    $$SOURCE_DIR/src/Vehicle/TerrainFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleBatteryFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleClockFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleDistanceSensorFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleEscStatusFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleEstimatorStatusFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleGPS2FactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleGPSFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleHygrometerFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleLocalPositionFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleLocalPositionSetpointFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleSetpointFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleTemperatureFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleVibrationFactGroup.cpp \
    $$SOURCE_DIR/src/Vehicle/VehicleWindFactGroup.cpp \
    $$SOURCE_DIR/src/VehicleSetup/VehicleComponent.cpp


HEADERS += \
    $$SOURCE_DIR/QGC.h \
    $$SOURCE_DIR/JsonHelper.h \
    $$SOURCE_DIR/KMLDomDocument.h \
    $$SOURCE_DIR/QGCApplication.h \
    $$SOURCE_DIR/QGCLoggingCategory.h \
    $$SOURCE_DIR/QGCQGeoCoordinate.h \
    $$SOURCE_DIR/QGCTemporaryFile.h \
    $$SOURCE_DIR/QGCPalette.h \
    $$SOURCE_DIR/src/Audio/AudioOutput.h \
    $$SOURCE_DIR/src/Geo/QGCGeo.h \
    $$SOURCE_DIR/src/Geo/UTMUPS.hpp \
    $$SOURCE_DIR/src/Geo/MGRS.hpp \
    $$SOURCE_DIR/src/Geo/Math.hpp \
    $$SOURCE_DIR/src/Geo/Utility.h \
    $$SOURCE_DIR/src/Geo/Utility.hpp \
    $$SOURCE_DIR/src/Geo/TransverseMercator.hpp \
    $$SOURCE_DIR/src/Geo/PolarStereographic.hpp \
    $$SOURCE_DIR/src/uas/UAS.h \
    $$SOURCE_DIR/src/uas/UASInterface.h \
    $$SOURCE_DIR/src/uas/UASMessageHandler.h \
    $$SOURCE_DIR/src/QmlControls/QGCGeoBoundingCube.h \
    $$SOURCE_DIR/src/QmlControls/QGroundControlQmlGlobal.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/AutoPilotPlugin.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/Generic/GenericAutoPilotPlugin.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/ESP8266Component.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/ESP8266ComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/MotorComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/RadioComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/SyslinkComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/Common/SyslinkComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMAutoPilotPlugin.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMAirframeComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMAirframeComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMCameraComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMCompassCal.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMFlightModesComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMFlightModesComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMFollowComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMFollowComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMHeliComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMLightsComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMMotorComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMPowerComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMRadioComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSafetyComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSensorsComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSensorsComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSubFrameComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMSubMotorComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/APM/APMTuningComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4AutoPilotPlugin.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/ActuatorComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/AirframeComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/AirframeComponentAirframes.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/AirframeComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/CameraComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/FlightModesComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4AirframeLoader.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4FlightBehavior.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4RadioComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4SimpleFlightModesController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PX4TuningComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PowerComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/PowerComponentController.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/SafetyComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/SensorsComponent.h \
    $$SOURCE_DIR/src/AutoPilotPlugins/PX4/SensorsComponentController.h \
    $$SOURCE_DIR/src/Comm/LinkConfiguration.h \
    $$SOURCE_DIR/src/Comm/LinkInterface.h \
    $$SOURCE_DIR/src/Comm/MAVLinkProtocol.h \
    $$SOURCE_DIR/src/Comm/MavlinkMessagesTimer.h \
    $$SOURCE_DIR/src/Comm/QGCMAVLink.h \
    $$SOURCE_DIR/src/Comm/LocalLink.h \
    $$SOURCE_DIR/src/Compression/QGCLZMA.h \
    $$SOURCE_DIR/src/Compression/QGCZlib.h \
    $$SOURCE_DIR/src/FactSystem/Fact.h \
    $$SOURCE_DIR/src/FactSystem/FactMetaData.h \
    $$SOURCE_DIR/src/FactSystem/SettingsFact.h \
    $$SOURCE_DIR/src/FactSystem/FactSystem.h \
    $$SOURCE_DIR/src/FactSystem/FactPanelController.h \
    $$SOURCE_DIR/src/FactSystem/FactGroup.h \
    $$SOURCE_DIR/src/FactSystem/ParameterManager.h \
    $$SOURCE_DIR/src/FirmwarePlugin/CameraMetaData.h \
    $$SOURCE_DIR/src/FirmwarePlugin/FirmwarePlugin.h \
    $$SOURCE_DIR/src/FirmwarePlugin/FirmwarePluginManager.h \
    $$SOURCE_DIR/src/FirmwarePlugin/PX4/PX4FirmwarePlugin.h \
    $$SOURCE_DIR/src/FirmwarePlugin/PX4/PX4FirmwarePluginFactory.h \
    $$SOURCE_DIR/src/FirmwarePlugin/PX4/PX4ParameterMetaData.h \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/APMFirmwarePlugin.h \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/APMFirmwarePluginFactory.h \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/APMParameterMetaData.h \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/ArduCopterFirmwarePlugin.h \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/ArduPlaneFirmwarePlugin.h \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/ArduRoverFirmwarePlugin.h \
    $$SOURCE_DIR/src/FirmwarePlugin/APM/ArduSubFirmwarePlugin.h \
    $$SOURCE_DIR/src/FollowMe/FollowMe.h \
    $$SOURCE_DIR/src/MissionManager/PlanElementController.h \
    $$SOURCE_DIR/src/MissionManager/SimpleMissionItem.h \
    $$SOURCE_DIR/src/MissionManager/VisualMissionItem.h \
    $$SOURCE_DIR/src/MissionManager/MissionCommandTree.h \
    $$SOURCE_DIR/src/MissionManager/QGCMapCircle.h \
    $$SOURCE_DIR/src/MissionManager/MissionCommandList.h \
    $$SOURCE_DIR/src/MissionManager/MissionCommandUIInfo.h \
    $$SOURCE_DIR/src/MissionManager/MissionItem.h \
    $$SOURCE_DIR/src/MissionManager/ComplexMissionItem.h \
    $$SOURCE_DIR/src/MissionManager/FixedWingLandingComplexItem.h \
    $$SOURCE_DIR/src/MissionManager/KMLPlanDomDocument.h \
    $$SOURCE_DIR/src/MissionManager/LandingComplexItem.h \
    $$SOURCE_DIR/src/MissionManager/MissionController.h \
    $$SOURCE_DIR/src/MissionManager/MissionManager.h \
    $$SOURCE_DIR/src/MissionManager/MissionSettingsItem.h \
    $$SOURCE_DIR/src/MissionManager/PlanManager.h \
    $$SOURCE_DIR/src/MissionManager/PlanMasterController.h \
    $$SOURCE_DIR/src/MissionManager/QGCMapPolygon.h \
    $$SOURCE_DIR/src/MissionManager/QGCMapPolyline.h \
    $$SOURCE_DIR/src/MissionManager/Section.h \
    $$SOURCE_DIR/src/MissionManager/SpeedSection.h \
    $$SOURCE_DIR/src/MissionManager/TakeoffMissionItem.h \
    $$SOURCE_DIR/src/MissionManager/VTOLLandingComplexItem.h \
    $$SOURCE_DIR/src/Settings/AppSettings.h \
    $$SOURCE_DIR/src/Settings/SettingsGroup.h \
    $$SOURCE_DIR/src/Settings/SettingsManager.h \
    $$SOURCE_DIR/src/Settings/UnitsSettings.h \
    $$SOURCE_DIR/src/Settings/APMMavlinkStreamRateSettings.h \
    $$SOURCE_DIR/src/Vehicle/CompInfo.h \
    $$SOURCE_DIR/src/Vehicle/CompInfoActuators.h \
    $$SOURCE_DIR/src/Vehicle/CompInfoEvents.h \
    $$SOURCE_DIR/src/Vehicle/CompInfoGeneral.h \
    $$SOURCE_DIR/src/Vehicle/CompInfoParam.h \
    $$SOURCE_DIR/src/Vehicle/ComponentInformationCache.h \
    $$SOURCE_DIR/src/Vehicle/ComponentInformationManager.h \
    $$SOURCE_DIR/src/Vehicle/FTPManager.h \
    $$SOURCE_DIR/src/Vehicle/InitialConnectStateMachine.h \
    $$SOURCE_DIR/src/Vehicle/StateMachine.h \
    $$SOURCE_DIR/src/Vehicle/HealthAndArmingCheckReport.h \
    $$SOURCE_DIR/src/Vehicle/MAVLinkStreamConfig.h \
    $$SOURCE_DIR/src/Vehicle/SysStatusSensorInfo.h \
    $$SOURCE_DIR/src/Vehicle/MultiVehicleManager.h \
    $$SOURCE_DIR/src/Vehicle/Vehicle.h \
    $$SOURCE_DIR/src/Vehicle/EventHandler.h \
    $$SOURCE_DIR/src/Vehicle/TerrainFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleBatteryFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleClockFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleDistanceSensorFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleEscStatusFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleEstimatorStatusFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleGPS2FactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleGPSFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleHygrometerFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleLocalPositionFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleLocalPositionSetpointFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleSetpointFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleTemperatureFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleVibrationFactGroup.h \
    $$SOURCE_DIR/src/Vehicle/VehicleWindFactGroup.h \
    $$SOURCE_DIR/src/VehicleSetup/VehicleComponent.h


RESOURCES *= $$SOURCE_DIR/src/FirmwarePlugin/APM/APMResources.qrc
RESOURCES *= $$SOURCE_DIR/src/FirmwarePlugin/PX4/PX4Resources.qrc

CustomBuild {
    exists($$PWD/custom/qgroundcontrol.qrc) {
        message("Using custom qgroundcontrol.qrc")
        RESOURCES += $$SOURCE_DIR/custom/qgroundcontrol.qrc
    } else {
        RESOURCES += $$SOURCE_DIR/qgroundcontrol.qrc
    }
    exists($$PWD/custom/qgcresources.qrc) {
        message("Using custom qgcresources.qrc")
        RESOURCES += $$SOURCE_DIR/custom/qgcresources.qrc
    } else {
        RESOURCES += $$SOURCE_DIR/qgcresources.qrc
    }
    exists($$SOURCE_DIR/custom/qgcimages.qrc) {
        message("Using custom qgcimages.qrc")
        RESOURCES += $$SOURCE_DIR/custom/qgcimages.qrc
    } else {
        RESOURCES += $$SOURCE_DIR/qgcimages.qrc
    }
    exists($$PWD/custom/InstrumentValueIcons.qrc) {
        message("Using custom InstrumentValueIcons.qrc")
        RESOURCES += $$SOURCE_DIR/custom/InstrumentValueIcons.qrc
    } else {
        RESOURCES += $$SOURCE_DIR/resources/InstrumentValueIcons/InstrumentValueIcons.qrc
    }
} else {
    DEFINES += QGC_APPLICATION_NAME=\"\\\"QGroundControl\\\"\"
    DEFINES += QGC_ORG_NAME=\"\\\"QGroundControl.org\\\"\"
    DEFINES += QGC_ORG_DOMAIN=\"\\\"org.qgroundcontrol\\\"\"
    RESOURCES += \
        $$SOURCE_DIR/qgroundcontrol.qrc \
        $$SOURCE_DIR/qgcresources.qrc \
        $$SOURCE_DIR/qgcimages.qrc \
        $$SOURCE_DIR/resources/InstrumentValueIcons/InstrumentValueIcons.qrc \
}


#
# [REQUIRED] LZMA decompression library
HEADERS+= \
    $$SOURCE_DIR/src/libs/xz-embedded/linux/include/linux/xz.h \
    $$SOURCE_DIR/src/libs/xz-embedded/linux/lib/xz/xz_lzma2.h \
    $$SOURCE_DIR/src/libs/xz-embedded/linux/lib/xz/xz_private.h \
    $$SOURCE_DIR/src/libs/xz-embedded/linux/lib/xz/xz_stream.h \
    $$SOURCE_DIR/src/libs/xz-embedded/userspace/xz_config.h
SOURCES += \
    $$SOURCE_DIR/src/libs/xz-embedded/linux/lib/xz/xz_crc32.c \
    $$SOURCE_DIR/src/libs/xz-embedded/linux/lib/xz/xz_crc64.c \
    $$SOURCE_DIR/src/libs/xz-embedded/linux/lib/xz/xz_dec_lzma2.c \
    $$SOURCE_DIR/src/libs/xz-embedded/linux/lib/xz/xz_dec_stream.c
INCLUDEPATH += \
    $$SOURCE_DIR/src/libs/xz-embedded/userspace \
    $$SOURCE_DIR/src/libs/xz-embedded/linux/include/linux
DEFINES += XZ_DEC_ANY_CHECK XZ_USE_CRC64

# [REQUIRED] zlib library
HEADERS+= \
    $$SOURCE_DIR/src/libs/zlib/crc32.h \
    $$SOURCE_DIR/src/libs/zlib/deflate.h \
    $$SOURCE_DIR/src/libs/zlib/gzguts.h \
    $$SOURCE_DIR/src/libs/zlib/inffast.h \
    $$SOURCE_DIR/src/libs/zlib/inffixed.h \
    $$SOURCE_DIR/src/libs/zlib/inflate.h \
    $$SOURCE_DIR/src/libs/zlib/inftrees.h \
    $$SOURCE_DIR/src/libs/zlib/trees.h \
    $$SOURCE_DIR/src/libs/zlib/zconf.h \
    $$SOURCE_DIR/src/libs/zlib/zlib.h \
    $$SOURCE_DIR/src/libs/zlib/zutil.h
SOURCES += \
    $$SOURCE_DIR/src/libs/zlib/adler32.c \
    $$SOURCE_DIR/src/libs/zlib/compress.c \
    $$SOURCE_DIR/src/libs/zlib/crc32.c \
    $$SOURCE_DIR/src/libs/zlib/deflate.c \
    $$SOURCE_DIR/src/libs/zlib/gzclose.c \
    $$SOURCE_DIR/src/libs/zlib/gzlib.c \
    $$SOURCE_DIR/src/libs/zlib/gzread.c \
    $$SOURCE_DIR/src/libs/zlib/gzwrite.c \
    $$SOURCE_DIR/src/libs/zlib/infback.c \
    $$SOURCE_DIR/src/libs/zlib/inffast.c \
    $$SOURCE_DIR/src/libs/zlib/inflate.c \
    $$SOURCE_DIR/src/libs/zlib/inftrees.c \
    $$SOURCE_DIR/src/libs/zlib/trees.c \
    $$SOURCE_DIR/src/libs/zlib/uncompr.c \
    $$SOURCE_DIR/src/libs/zlib/zutil.c
INCLUDEPATH += \
    $$SOURCE_DIR/src/libs/zlib

#
# [REQUIRED] Events submodule
HEADERS+= \
    $$SOURCE_DIR/libevents/libevents/libs/cpp/protocol/receive.h \
    $$SOURCE_DIR/libevents/libevents/libs/cpp/parse/health_and_arming_checks.h \
    $$SOURCE_DIR/libevents/libevents/libs/cpp/parse/parser.h \
    $$SOURCE_DIR/libevents/libevents/libs/cpp/generated/events_generated.h \
    $$SOURCE_DIR/libevents/libevents_definitions.h

SOURCES += \
    $$SOURCE_DIR/libevents/libevents/libs/cpp/protocol/receive.cpp \
    $$SOURCE_DIR/libevents/libevents/libs/cpp/parse/health_and_arming_checks.cpp \
    $$SOURCE_DIR/libevents/libevents/libs/cpp/parse/parser.cpp \
    $$SOURCE_DIR/libevents/definitions.cpp

INCLUDEPATH += \
        $$SOURCE_DIR/libevents \
        $$SOURCE_DIR/libevents/libs/cpp/parse

#
# [REQUIRED] Add support for the MAVLink communications protocol.
#
# By default MAVLink dialect is hardwired to arudpilotmega. The reason being
# the current codebase supports both PX4 and APM flight stack. PX4 flight stack
# only uses common MAVLink specifications, whereas APM flight stack uses custom
# MAVLink specifications which adds to common. So by using the adupilotmega dialect
# QGC can support both in the same codebase.

# Once the mavlink helper routines include support for multiple dialects within
# a single compiled codebase this hardwiring of dialect can go away. But until then
# this "workaround" is needed.

# In the mean time, it’s possible to define a completely different dialect by defining the
# location and name below.

# check for user defined settings in user_config.pri if not already set as qmake argument
isEmpty(MAVLINKPATH_REL) {
    exists(user_config.pri):infile(user_config.pri, MAVLINKPATH_REL) {
        MAVLINKPATH_REL = $$fromfile(user_config.pri, MAVLINKPATH_REL)
        message($$sprintf("Using user-supplied relativ mavlink path '%1' specified in user_config.pri", $$MAVLINKPATH_REL))
    } else {
        MAVLINKPATH_REL = ../../../include/Protocol/mavlink/v2.0
    }
}

isEmpty(MAVLINKPATH) {
    exists(user_config.pri):infile(user_config.pri, MAVLINKPATH) {
        MAVLINKPATH     = $$fromfile(user_config.pri, MAVLINKPATH)
        message($$sprintf("Using user-supplied mavlink path '%1' specified in user_config.pri", $$MAVLINKPATH))
    } else {
        MAVLINKPATH     = $$LIBS_DIR/$$MAVLINKPATH_REL
    }
}

isEmpty(MAVLINK_CONF) {
    exists(user_config.pri):infile(user_config.pri, MAVLINK_CONF) {
        MAVLINK_CONF = $$fromfile(user_config.pri, MAVLINK_CONF)
        message($$sprintf("Using user-supplied mavlink dialect '%1' specified in user_config.pri", $$MAVLINK_CONF))
    } else {
        MAVLINK_CONF = ardupilotmega
    }
}

# If defined, all APM specific MAVLink messages are disabled
contains (CONFIG, QGC_DISABLE_APM_MAVLINK) {
    message("Disable APM MAVLink support")
    DEFINES += NO_ARDUPILOT_DIALECT
    CONFIG  += ArdupilotDisabled
} else {
    CONFIG  += ArdupilotEnabled
}

# First we select the dialect, checking for valid user selection
# Users can override all other settings by specifying MAVLINK_CONF as an argument to qmake
!isEmpty(MAVLINK_CONF) {
    message($$sprintf("Using MAVLink dialect '%1'.", $$MAVLINK_CONF))
}

# Then we add the proper include paths dependent on the dialect.
INCLUDEPATH += $$MAVLINKPATH

exists($$MAVLINKPATH/common) {
    !isEmpty(MAVLINK_CONF) {
        count(MAVLINK_CONF, 1) {
            exists($$MAVLINKPATH/$$MAVLINK_CONF) {
                INCLUDEPATH += $$MAVLINKPATH/$$MAVLINK_CONF
                DEFINES += $$sprintf('QGC_USE_%1_MESSAGES', $$upper($$MAVLINK_CONF))
            } else {
                error($$sprintf("MAVLink dialect '%1' does not exist at '%2'!", $$MAVLINK_CONF, $$MAVLINKPATH_REL))
            }
        } else {
            error(Only a single mavlink dialect can be specified in MAVLINK_CONF)
        }
    } else {
        INCLUDEPATH += $$MAVLINKPATH/common
    }
} else {
    error($$sprintf("MAVLink folder does not exist at '%1'! Run 'git submodule init && git submodule update' on the command line.",$$MAVLINKPATH_REL))
}

message($$TARGET)
