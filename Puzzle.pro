#-------------------------------------------------
#
# Project created by QtCreator 2015-03-27T12:50:57
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Puzzle
TEMPLATE = app
LIBS += -lOpengl32


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp

HEADERS  += mainwindow.h \
    glwidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
