cmake_minimum_required(VERSION 2.8)

project(LY_UAVFlyControl_DJI_Dock)


set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 11)

#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#  Qt C:\Qt\Qt5.11.2\5.11.2\msvc2015_64\lib\cmake\Qt5
set(QT_VERSION 5 CACHE STRING "Set to 5 or 6 for Qt5 or Qt6")


add_definitions(-DLY_UAVFLYCONTROL_DJI_DOCK_LIBRARY)

find_package(Qt5 ${PROJECT_VERSION} REQUIRED COMPONENTS Core Network Gui Widgets)
find_package(Qt5 ${PROJECT_VERSION} REQUIRED OPTIONAL_COMPONENTS Quick WebSockets)


set(QT_SBOM_DEFAULT_QT_LICENSE_ID_LIBRARIES "QT_COMMERCIAL_OR_GPL3")


set(QtMqtt_Src_Dir ${CMAKE_CURRENT_SOURCE_DIR}/qtmqtt/src/QtMqtt)


set(QtMqtt_srcs
    ${QtMqtt_Src_Dir}/qmqttauthenticationproperties.cpp ${QtMqtt_Src_Dir}/qmqttauthenticationproperties.h
    ${QtMqtt_Src_Dir}/qmqttclient.cpp ${QtMqtt_Src_Dir}/qmqttclient.h ${QtMqtt_Src_Dir}/qmqttclient_p.h
    ${QtMqtt_Src_Dir}/qmqttconnection.cpp ${QtMqtt_Src_Dir}/qmqttconnection_p.h
    ${QtMqtt_Src_Dir}/qmqttconnectionproperties.cpp ${QtMqtt_Src_Dir}/qmqttconnectionproperties.h ${QtMqtt_Src_Dir}/qmqttconnectionproperties_p.h
    ${QtMqtt_Src_Dir}/qmqttcontrolpacket.cpp ${QtMqtt_Src_Dir}/qmqttcontrolpacket_p.h
    ${QtMqtt_Src_Dir}/qmqttglobal.h ${QtMqtt_Src_Dir}/qmqttglobal_p.h
    ${QtMqtt_Src_Dir}/qmqttmessage.cpp ${QtMqtt_Src_Dir}/qmqttmessage.h ${QtMqtt_Src_Dir}/qmqttmessage_p.h
    ${QtMqtt_Src_Dir}/qmqttpublishproperties.cpp ${QtMqtt_Src_Dir}/qmqttpublishproperties.h ${QtMqtt_Src_Dir}/qmqttpublishproperties_p.h
    ${QtMqtt_Src_Dir}/qmqttsubscription.cpp ${QtMqtt_Src_Dir}/qmqttsubscription.h ${QtMqtt_Src_Dir}/qmqttsubscription_p.h
    ${QtMqtt_Src_Dir}/qmqttsubscriptionproperties.cpp ${QtMqtt_Src_Dir}/qmqttsubscriptionproperties.h
    ${QtMqtt_Src_Dir}/qmqtttopicfilter.cpp ${QtMqtt_Src_Dir}/qmqtttopicfilter.h
    ${QtMqtt_Src_Dir}/qmqtttopicname.cpp ${QtMqtt_Src_Dir}/qmqtttopicname.h
    ${QtMqtt_Src_Dir}/qmqtttype.cpp ${QtMqtt_Src_Dir}/qmqtttype.h
)

add_subdirectory(djicloude/dji/sdk)

set(dji_coloudapi_srcs_Dir ${CMAKE_CURRENT_SOURCE_DIR}/djicloude)

set(dji_coloudapi_srcs
    ${dji_coloudapi_srcs_Dir}/DJICloudeAPI.h ${dji_coloudapi_srcs_Dir}/DJICloudeAPI.cpp ${dji_coloudapi_srcs_Dir}/DJICloudeAPI.ui
    ${dji_coloudapi_srcs_Dir}/DJIDockState.h ${dji_coloudapi_srcs_Dir}/DJIDockState.cpp ${dji_coloudapi_srcs_Dir}/DJIDockState.ui
    ${dji_coloudapi_srcs_Dir}/DJIDock2Control.h ${dji_coloudapi_srcs_Dir}/DJIDock2Control.cpp ${dji_coloudapi_srcs_Dir}/DJIDock2Control.ui
    ${dji_coloudapi_srcs_Dir}/DJIDockControl.h ${dji_coloudapi_srcs_Dir}/DJIDockControl.cpp ${dji_coloudapi_srcs_Dir}/DJIDockControl.ui
    ${dji_coloudapi_srcs_Dir}/UAVProtocol.h ${dji_coloudapi_srcs_Dir}/UAVProtocol.cpp
    ${dji_coloudapi_srcs}
)

set(lib_srcs
    LY_UAVFlyControl_DJI_Dock_global.h
    LY_UAVFlyControl_DJI_DockInterface.h LY_UAVFlyControl_DJI_DockInterface.cpp
    LY_UAVFlyControl_DJI_Dock.h LY_UAVFlyControl_DJI_Dock.cpp
    IUAVFlyControl_DJI_DockActivator.h IUAVFlyControl_DJI_DockActivator.cpp
    IDockRealTimeStateManageImpl.h IDockRealTimeStateManageImpl.cpp
    FlyControl/FlowLayout.h FlyControl/FlowLayout.cpp
    FlyControl/UAVPushButton.h FlyControl/UAVPushButton.cpp
    FlyControl/UAVSelectDialog.h FlyControl/UAVSelectDialog.cpp FlyControl/UAVSelectDialog.ui
    FlyControl/UAVFlyManage_DJI_DockDialog.h FlyControl/UAVFlyManage_DJI_DockDialog.cpp FlyControl/UAVFlyManage_DJI_DockDialog.ui
    FlyControl/IUAVFlyManage_DJI_WayLine_UploadDialog.h FlyControl/IUAVFlyManage_DJI_WayLine_UploadDialog.cpp FlyControl/IUAVFlyManage_DJI_WayLine_UploadDialog.ui
    FlyControl/IUAVFlyManage_DJI_SendTask_Dialog.h FlyControl/IUAVFlyManage_DJI_SendTask_Dialog.cpp FlyControl/IUAVFlyManage_DJI_SendTask_Dialog.ui
	FlyControl/IUAVFlyManage_DJI_TakeoffToPoint.h FlyControl/IUAVFlyManage_DJI_TakeoffToPoint.cpp FlyControl/IUAVFlyManage_DJI_TakeoffToPoint.ui
	FlyControl/IUAVFlyMessage_DJI_WayLine_Up.h FlyControl/IUAVFlyMessage_DJI_WayLine_Up.cpp
    TaskManage/UAVFlyManage_DJITaskManageDialog.h TaskManage/UAVFlyManage_DJITaskManageDialog.cpp TaskManage/UAVFlyManage_DJITaskManageDialog.ui
    TaskManage/UAVFlyManage_DJIMissionManagerDialog.h TaskManage/UAVFlyManage_DJIMissionManagerDialog.cpp TaskManage/UAVFlyManage_DJIMissionManagerDialog.ui
	wpml_codec/DJI_UAVName2Type.h wpml_codec/DJI_UAVName2Type.cpp
	wpml_codec/common.h
	wpml_codec/core.h wpml_codec/core.cpp
	wpml_codec/enums.h wpml_codec/enums.cpp
	wpml_codec/macros.h
	wpml_codec/structs.h
	wpml_codec/utils.h wpml_codec/utils.cpp
	wpml_codec/version.h
	wpml_codec/wpml_codec.h
    ${lib_srcs}
)

list(APPEND lib_libs LyUcisPluginFrameWork ${LYUI_LIBRARIES} ${LYCORES_LIBRARIES} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql Qt5::Xml Qt5::Network Qt5::Charts Qt5::Positioning Qt5::TextToSpeech tinyxml2.lib minizip.lib zlib.lib aws-cpp-sdk-core.lib aws-cpp-sdk-s3.lib) 


add_library(${PROJECT_NAME} SHARED ${QtMqtt_srcs} ${dji_coloudapi_srcs} ${lib_srcs} ${lib_hdrs} ${lib_uis})


target_include_directories(${PROJECT_NAME} PRIVATE ${Qt${QT_VERSION_MAJOR}Core_PRIVATE_INCLUDE_DIRS})

include_directories(${QtMqtt_Src_Dir})
include_directories(${dji_coloudapi_srcs_Dir})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtmqtt/src/)
include_directories(. ../../include ../../include/lyUI ../../include/3d ../../3rdParty/include ../../include/aws)

target_link_libraries(${PROJECT_NAME}
        Qt5::CorePrivate
        Qt5::Core
        Qt5::Network
        Qt5::CorePrivate
        ${lib_libs}
)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

install(EXPORT ${PROJECT_NAME}-targets DESTINATION lib/cmake FILE ${PROJECT_NAME}-config.cmake)
