TEMPLATE = app
QT += \
  gui openglextensions \  # for library
  widgets                 # for demo application
CONFIG += c++11 debug
CONFIG -= app_bundle

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
