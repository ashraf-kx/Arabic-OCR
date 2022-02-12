#-------------------------------------------------
#
# Project created by QtCreator 2014-12-27 T 18:34:54
#
#-------------------------------------------------

cmake_minimum_required(VERSION 3.5)

project(Arabic_OCR VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core5Compat REQUIRED)
find_package(OpenCV 3.4.16 EXACT REQUIRED PATHS $ENV{HOME}/github/opencv)
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        recognition.cpp
        segmentation.cpp
        preprocess.cpp
        recognition.h
        segmentation.h
        preprocess.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Arabic_OCR
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Arabic_OCR APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Arabic_OCR SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Arabic_OCR
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Arabic_OCR PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Arabic_OCR PRIVATE Qt${QT_VERSION_MAJOR}::Core5Compat)
target_link_libraries(Arabic_OCR PRIVATE ${OpenCV_LIBS})

set_target_properties(Arabic_OCR PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Arabic_OCR)
endif()
