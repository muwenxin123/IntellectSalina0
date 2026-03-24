cmake_minimum_required(VERSION 2.8)
project(FireProt)
set(CMAKE_CXX_STANDARD 11)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(3rd REQUIRED)
find_package(OSG REQUIRED)
find_package(LyCore REQUIRED)
find_package(Ffmpeg REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml Sql Network Designer PrintSupport OpenGL Svg Concurrent Charts Positioning Test TextToSpeech Multimedia MultimediaWidgets RemoteObjects Gamepad AxContainer WebSockets)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/lib_ly_ccip)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/ffmpeg)

if (UNIX)
  find_package(Qt5 REQUIRED COMPONENTS X11Extras)
else()
  find_package(Qt5 REQUIRED COMPONENTS AxContainer)
endif()

macro(clear_vars)
  foreach(_var ${ARGN})
    unset(${_var})
    unset(${_var} CACHE)
  endforeach()
endmacro()

macro(add_subdir)
  foreach(_var ${ARGN})
  string(TOUPPER ${_var} varU)
	clear_vars(${varU}_LIBRARY ${varU}_LIBRARIES ${varU}_INCLUDE_DIR)
	set(${varU}_LIBRARY ${_var} CACHE INTERNAL "")
	add_subdirectory(${_var})
	set(${varU}_INCLUDE_DIR "${${${varU}_LIBRARY}_SOURCE_DIR}" "${${${varU}_LIBRARY}_BINARY_DIR}" CACHE INTERNAL "")
	set(${varU}_INCLUDE_DIRS ${${varU}_INCLUDE_DIR})
	set(${varU}_LIBRARIES ${${varU}_LIBRARY})
	set(${varU}_SOURCE_DIR "${${${varU}_LIBRARY}_SOURCE_DIR}")
	set(${varU}_FOUND TRUE)
  endforeach()
endmacro()

macro(source_group_by_dir source_files)
    if(MSVC)
        set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(sgbd_file ${${source_files}})
            string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC)
endmacro(source_group_by_dir)


add_subdir(LY_TinyXml FireProt LyUcisPluginFrameWork Measure lyXyhc
  LY_AmmunitionManageService
  LY_CollaborativeSearchService
  LY_CommandUnitManageService
  LY_ConfigBundleControlService
  LY_PlatformStateMonitoring
  LY_PositionManageService
  LY_ProtocolParse_XygsServer
  LY_SensorManageService
  LY_SystemMaintenanceService
  LY_MarkManageService
  LY_TaskManage
  LY_TaskManageService
  LY_TaskPlatMatch
  LY_UAVFlyControl_Mavlink
  LY_UAVRealTimeStateManageService
  LY_AgricultureHomepageService
  LY_WeaponManageService
  LY_ZoneManagerServive
  LY_ZoneManager
  LY_FireSpreadServive
  LY_AgriMachineManager
  LY_AgriFarmManager
  LY_AgriySprayPrescriptionService
  LY_AgriSoilTestingAnalysis
  LY_AgriFertilizerPrescription
  LY_AgroTaskManage
  LY_AgroTaskManageService
  LY_SensorVedioService
  Ly_VideoRealTimePreview
  LY_AgriVideoPlayer
)
