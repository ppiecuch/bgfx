TEMPLATE = lib
TARGET = bgfx
CONFIG += staticlib
QT += opengl openglextensions

SOURCES += \
    ../src/amalgamated.mm \
    ../src/amalgamated.cpp

include("common.pri")
