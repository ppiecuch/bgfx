TEMPLATE = app
QT += \
  gui openglextensions \  # for library
  widgets                 # for demo application
CONFIG += c++11 debug
CONFIG -= app_bundle

DEFINES += BGFX_CONFIG_DEBUG BGFX_CONFIG_RENDERER_OPENGL=1

INCLUDEPATH += \
    ../3rdparty/bx/include \
    ../3rdparty/khronos \
    ../3rdparty \
    ../include \
    ../examples/common

macx {
    INCLUDEPATH += ../3rdparty/bx/include/compat/osx
    LIBS += -framework Foundation -framework AppKit
}
win: INCLUDEPATH += ../3rdparty/bx/include/compat/msvc
linux: INCLUDEPATH += ../3rdparty/bx/include/compat/freebsd

SOURCES += \
    ../src/amalgamated.cpp \
    ../src/amalgamated.mm \
    \
    ../examples/common/imgui/imgui-amalgamated.cpp \
    ../examples/common/nanovg/nanovg-amalgamated.cpp \
    ../examples/common/qt_io.cpp \
    ../examples/common/bgfx_utils.cpp \
    ../examples/common/osx_utils.mm \
    ../examples/common/entry/dbg.cpp \
    ../examples/common/entry/cmd.cpp \
    ../examples/common/entry/input.cpp \
    ../examples/common/entry/entry.cpp \
    ../examples/common/entry/entry_qt.cpp \
    \
    ../examples/20-nanovg/nanovg.cpp \
    \
    ../3rdparty/ocornut-imgui/imgui.cpp \
    ../3rdparty/ocornut-imgui/imgui_draw.cpp \
    ../3rdparty/ocornut-imgui/imgui_demo.cpp \
    \
    ../3rdparty/ib-compress/indexbuffercompression.cpp \
    ../3rdparty/ib-compress/indexbufferdecompression.cpp \

HEADERS += \
    ../examples/common/bgfx_utils.h \
    ../examples/common/osx_utils.h \
    ../3rdparty/ib-compress/indexbuffercompression.h \
    ../3rdparty/ib-compress/indexbuffercompressionformat.h \
    ../3rdparty/ib-compress/indexbufferdecompression.h \
    ../3rdparty/ib-compress/indexcompressionconstants.h \
    ../3rdparty/ib-compress/readbitstream.h \
    ../3rdparty/ib-compress/writebitstream.h

RESOURCES += assets.qrc runtime.qrc

include("common.pri")
include("QtBgfx/QtBgfx.pri")
