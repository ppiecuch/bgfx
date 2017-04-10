TEMPLATE = lib
TARGET = bgfx
CONFIG += staticlib
QT += opengl openglextensions

macx|ios: SOURCES += ../src/amalgamated.mm

SOURCES += \
    ../src/amalgamated.cpp \
    \
    ../3rdparty/bx/src/commandline.cpp \
    ../3rdparty/bx/src/crtimpl.cpp \
    ../3rdparty/bx/src/fpumath.cpp \
    ../3rdparty/bx/src/debug.cpp \
    ../3rdparty/bx/src/os.cpp \
    ../3rdparty/bx/src/string.cpp \
    ../3rdparty/bx/src/thread.cpp \
    ../3rdparty/bx/src/mutex.cpp \
    ../3rdparty/bx/src/sem.cpp
    \

include("common.pri")
