#-------------------------------------------------
#
# Project created by QtCreator 2014-12-27 T 18:34:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Arabic_OCR
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    recognition.cpp \
    segmentation.cpp \
    preprocess.cpp

HEADERS  += mainwindow.h \
    recognition.h \
    segmentation.h \
    preprocess.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc

INCLUDEPATH += /usr/local/include/opencv2

LIBS += -L /usr/local/lib \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_ml \
    -lopencv_video \
    -lopencv_features2d \
    -lopencv_calib3d \
    -lopencv_objdetect\
    -lopencv_contrib \
    -lopencv_legacy \
    -lopencv_flann

INCLUDEPATH  += /usr/include/poppler
LIBS         += /usr/lib/x86_64-linux-gnu/libpoppler-qt5.so

OTHER_FILES += \
    NOTES.txt
