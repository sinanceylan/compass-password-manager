#-------------------------------------------------
#
# Project created by QtCreator 2011-09-09T15:25:03
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = compass
TEMPLATE = app

LIBS += -lcryptopp

TRANSLATIONS = compass_tr.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    commonlib.cpp \
    passworddialog.cpp \
    opendbpassworddialog.cpp \
    proxysqlrelationaldelegate.cpp \
    categorydialog.cpp \
    entrydialog.cpp \
    exportdialog.cpp \
    importdialog.cpp \
    optionsdialog.cpp \
    filesystemmodel.cpp \
    generatepassworddialog.cpp

HEADERS  += mainwindow.h \
    commonlib.h \
    passworddialog.h \
    opendbpassworddialog.h \
    proxysqlrelationaldelegate.h \
    categorydialog.h \
    entrydialog.h \
    exportdialog.h \
    importdialog.h \
    optionsdialog.h \
    filesystemmodel.h \
    generatepassworddialog.h

FORMS    += mainwindow.ui \
    passworddialog.ui \
    opendbpassworddialog.ui \
    entrydialog.ui \
    categorydialog.ui \
    exportdialog.ui \
    importdialog.ui \
    optionsdialog.ui \
    generatepassworddialog.ui

RESOURCES += \
    compass.qrc
