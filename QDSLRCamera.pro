#-------------------------------------------------
#
# Project created by QtCreator 2014-04-21T17:40:56
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia

TARGET = QDSLRCamera
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$[QT_INSTALL_PLUGINS]/mediaservice

SOURCES += \
    qgphotocaptureserviceplugin.cpp \
    qgphotocaptureservice.cpp \
    qgphotocameracontrol.cpp \
    qgphotoimagecapturecontrol.cpp \
    qgphotovideoinputdevicecontrol.cpp \
    qgphotocapturesession.cpp

HEADERS += \
    qgphotocaptureserviceplugin.h \
    qgphotocaptureservice.h \
    qgphotocameracontrol.h \
    qgphotoimagecapturecontrol.h \
    qgphotovideoinputdevicecontrol.h \
    qgphotocapturesession.h
OTHER_FILES += QDSLRCamera.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

LIBS     += -lgphoto2_port -lgphoto2
