TEMPLATE = app
QT += \
  gui openglextensions \  # for library
  widgets                 # for demo application
CONFIG += c++11 debug
CONFIG -= app_bundle

DEFINES += BGFX_CONFIG_DEBUG

INCLUDEPATH += ../3rdparty/bx/include ../3rdparty ../include ../examples/common

macx: INCLUDEPATH += ../3rdparty/bx/include/compat/osx
win: INCLUDEPATH += ../3rdparty/bx/include/compat/msvc
linux: INCLUDEPATH += ../3rdparty/bx/include/compat/freebsd

macx: LIBS += -framework Foundation

QMAKE_MOC_OPTIONS += -DBGFX_CONFIG_RENDERER_OPENGL

HEADERS += \
    ../include/bgfx.qt.h
SOURCES += \
    ../src/amalgamated.cpp \
    ../3rdparty/ocornut-imgui/imgui.cpp \
    ../3rdparty/ocornut-imgui/imgui_wm.cpp \
    ../3rdparty/ocornut-imgui/imgui_draw.cpp \
    ../examples/common/imgui/imgui-amalgamated.cpp \
    ../examples/common/nanovg/nanovg-amalgamated.cpp \
    ../examples/common/entry/dbg.cpp \
    ../examples/common/entry/cmd.cpp \
    ../examples/common/entry/input.cpp \
    ../examples/common/entry/entry.cpp \
    ../examples/common/entry/entry_qt.cpp \
    ../examples/20-nanovg/nanovg.cpp

include("common.pri")