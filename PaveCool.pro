#-------------------------------------------------
#
# Project created by QtCreator 2014-09-23T15:19:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PaveCool
TEMPLATE = app

SOURCES += main.cpp\
        pavecool.cpp \
    globals.cpp \
    helpdlg.cpp \
    tdlg.cpp \
    about.cpp

HEADERS  += pavecool.h \
    globals.h \
    helpdlg.h \
    tdlg.h \
    about.h

FORMS    += pavecool.ui \
    helpdlg.ui \
    tdlg.ui \
    about.ui

TRANSLATIONS    += locale/pavecool_de.ts

RESOURCES += \
    resource.qrc

RC_ICONS += icons/PaveCool.ico \
            icons/PaveCoolDoc.ico

OTHER_FILES +=

DISTFILES +=
