#-------------------------------------------------
#
# Project created by QtCreator 2015-09-06T00:52:16
#   nosferatu
#-------------------------------------------------

QT       -= core gui
TEMPLATE = lib
VERSION = $$(HOUDINI_VERSION)
CONFIG -= qt
CONFIG += plugin

TARGET = sop_groupDiff
INCLUDEPATH += $$(HFS)/toolkit/include
LIBS += -L$$(HFS)/dsolib \
    -lGL \
    -lGLU \
    -lX11 \
    -lXi \
    -lXmu \
    -lXext \
    -lHoudiniUI \
    -lHoudiniOPZ \
    -lHoudiniOP3 \
    -lHoudiniOP2 \
    -lHoudiniOP1 \
    -lHoudiniSIM \
    -lHoudiniGEO \
    -lHoudiniPRM \
    -lHoudiniUT
QMAKE_CXXFLAGS += -Wall \
    -W \
    -Wno-parentheses \
    -Wno-sign-compare \
    -Wno-reorder \
    -Wno-uninitialized \
    -Wunused \
    -Wno-unused-parameter \
    -Wno-deprecated \
    -DSIZEOF_VOID_P=8 \
    -D_GNU_SOURCE \
    -DLINUX \
    -m64 \
    -DAMD64 \
    -DSESI_LITTLE_ENDIAN \
    -DENABLE_THREADS \
    -DUSE_PTHREADS \
    -DENABLE_UI_THREADS \
    -DGCC3 \
    -DGCC4 \
    -DMAKING_DSO \
    -DDLLEXPORT="" \
    -shared \
#    -O0 \ #debug
    -O2 \
    -g #returns more info when crashes


DEFINES += SOP_GROUPDIFF

SOURCES += \
    sop_groupDiff.cpp

HEADERS += \
    sop_groupDiff.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
