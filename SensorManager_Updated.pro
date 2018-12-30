#-------------------------------------------------
#
# Project created by QtCreator 2018-12-11T16:18:18
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT       += serialport
QT       += sql
QT       +=charts
TARGET = SensorManager_Updated
TEMPLATE = app


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwidget.cpp \
    globeobject.cpp \
    portdialog.cpp \
    treeviewmodel.cpp \
    serialportdialog.cpp \
    datathread.cpp \
    dataconfigdialog.cpp \
    renamedialog.cpp \
    environmentconfig.cpp \
    sensorconfig_dialog.cpp

HEADERS += \
        mainwidget.h \
    globeobject.h \
    portdialog.h \
    treeviewmodel.h \
    serialportdialog.h \
    datathread.h \
    dataconfigdialog.h \
    renamedialog.h \
    environmentconfig.h \
    sensorconfig_dialog.h

FORMS += \
        mainwidget.ui \
    portdialog.ui \
    portdialog.ui \
    serialportdialog.ui \
    dataconfigdialog.ui \
    renamedialog.ui \
    environmentconfig.ui \
    sensorconfig_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    rsc.qrc


