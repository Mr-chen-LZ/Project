QT       += core gui network multimedia multimediawidgets widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = ESP32CameraControl
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        mjpegstreamer.cpp \
        mjpegstreamer.cpp

HEADERS += \
        mainwindow.h \
        mjpegstreamer.h \
        mjpegstreamer.h

FORMS += \
        mainwindow.ui
