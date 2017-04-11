TEMPLATE = lib
TARGET = bgfx
CONFIG += staticlib
QT += opengl openglextensions

macx|ios: SOURCES += ../src/amalgamated.mm
SOURCES += ../src/amalgamated.cpp
SOURCES += ../3rdparty/bx/src/bx-amalgamated.cpp

include("bimg.pri")
include("common.pri")
