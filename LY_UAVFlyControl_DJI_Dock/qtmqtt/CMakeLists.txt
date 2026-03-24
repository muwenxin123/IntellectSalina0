# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)


# 设置默认 Qt 版本（可由用户通过命令行修改） C:\Qt\Qt5.11.2\5.11.2\msvc2015_64\lib\cmake\Qt5
set(QT_VERSION 6 CACHE STRING "Set to 5 or 6 for Qt5 or Qt6")

if (QT_VERSION EQUAL 5)

    set(CMAKE_CXX_STANDARD 11)

    project(QtMqtt
        VERSION "${QT_REPO_MODULE_VERSION}"
        DESCRIPTION "Qt Mqtt Libraries"
        HOMEPAGE_URL "https://qt.io/"
        LANGUAGES CXX C
    )
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

     find_package(Qt5 ${PROJECT_VERSION} REQUIRED COMPONENTS Core Network Gui Widgets)
     find_package(Qt5 ${PROJECT_VERSION} REQUIRED OPTIONAL_COMPONENTS Quick WebSockets)
     
     add_subdirectory(src)
     add_subdirectory(examples/mqtt/subscriptionsdjicloude)

     
elseif (QT_VERSION EQUAL 6)

    include(.cmake.conf)
    
    project(QtMqtt
        VERSION "${QT_REPO_MODULE_VERSION}"
        DESCRIPTION "Qt Mqtt Libraries"
        HOMEPAGE_URL "https://qt.io/"
        LANGUAGES CXX C
    )

    find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals)

    # This should be called as early as possible, just after find_package(BuildInternals) where it is
    # defined.
    qt_internal_project_setup()

    find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS Core Network)
    find_package(Qt6 ${PROJECT_VERSION} CONFIG OPTIONAL_COMPONENTS Quick  # For tests
                                                                   Gui Widgets WebSockets) # For examples

    qt_build_repo()

endif()
