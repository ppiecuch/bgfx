TEMPLATE = lib
CONFIG += static
QT += opengl openglextensions

INCLUDEPATH += ../3rdparty/bx/include ../include
macx: INCLUDEPATH += ../3rdparty/bx/include/compat/osx
win: INCLUDEPATH += ../3rdparty/bx/include/compat/msvc
linux: INCLUDEPATH += ../3rdparty/bx/include/compat/freebsd

SOURCES += ../src/amalgamated.cpp
