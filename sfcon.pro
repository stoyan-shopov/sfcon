#-------------------------------------------------
#
# Project created by QtCreator 2013-06-20T16:27:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += PETIT_FS
DEFINES += ENGINE_32BIT
DEFINES += CORE_CELLS_COUNT="1024*128"
DEFINES += STACK_DEPTH=32

TARGET = thread-test
TEMPLATE = app


SOURCES += main.cxx\
        mainwindow.cxx \
    ./sforth/sf-opt-string.c \
    ./sforth/sf-opt-prog-tools.c \
    ./sforth/sf-opt-file.c \
    ./sforth/engine.c \
    qsfdict.c \
    qsfgraph.cxx \
    petite-fatfs/src/pff.c \
    petite-fatfs/src/diskio.cxx

HEADERS  += mainwindow.hxx \
    qsfdict.h \
    petite-fatfs/src/pff.h

FORMS    += mainwindow.ui

INCLUDEPATH += ./sforth/
INCLUDEPATH += ./petite-fatfs/src/

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    draw.fs
