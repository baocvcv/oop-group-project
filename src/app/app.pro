#-------------------------------------------------
#
# Project created by QtCreator 2019-07-01T20:00:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = app
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
INCLUDEPATH += . ./include /opt/gurobi811/linux64/include
LIBS += -lz3 -L/opt/gurobi811/linux64/lib -lgurobi_c++ -lgurobi81

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        Graph.cc \
        Architecture.cc \
        Solver.cc \
        MixDisplayer.cpp \
        Show.cc

HEADERS += \
        include/displaydialog.h \
        include/mainwindow.h \
        include/Architecture.h \
        include/ComMethod.h \
        include/Graph.h \
        include/GraphCreator.h \
        include/MixDisplayer.h \
        include/Module.h \
        include/OnePassSynth.h \
        include/Show.h \
        include/Solver.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
