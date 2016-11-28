TEMPLATE = app
QT += \
  gui openglextensions \  # for library
  widgets                 # for demo application
CONFIG += c++11 debug
CONFIG -= app_bundle

macx|ios: SOURCES += \
    ../src/amalgamated.mm \
    ../examples/common/osx_utils.mm

SOURCES += \
    ../src/amalgamated.cpp \
    \
    ../examples/common/bounds.cpp \
    ../examples/common/camera.cpp \
    ../examples/common/cube_atlas.cpp \
    ../examples/common/qt_io.cpp \
    ../examples/common/bgfx_utils.cpp \
    \
    ../examples/common/entry/dbg.cpp \
    ../examples/common/entry/cmd.cpp \
    ../examples/common/entry/input.cpp \
    ../examples/common/entry/entry.cpp \
    ../examples/common/entry/entry_qt.cpp \
    \
    ../examples/common/debugdraw/debugdraw.cpp \
    \
    ../examples/common/imgui/imgui-amalgamated.cpp \
    ../examples/common/nanovg/nanovg-amalgamated.cpp \
    \
    ../examples/common/font/text_buffer_manager.cpp \
    ../examples/common/font/font_manager.cpp \
    ../examples/common/font/text_metrics.cpp \
    ../examples/common/font/utf8.cpp \
    \
    ../3rdparty/ocornut-imgui/imgui.cpp \
    ../3rdparty/ocornut-imgui/imgui_draw.cpp \
    ../3rdparty/ocornut-imgui/imgui_demo.cpp \
    \
    ../3rdparty/ib-compress/indexbuffercompression.cpp \
    ../3rdparty/ib-compress/indexbufferdecompression.cpp \

HEADERS += \
    ../examples/common/bounds.h \
    ../examples/common/camera.h \
    ../examples/common/bgfx_utils.h \
    ../examples/common/osx_utils.h \
    \
    ../examples/common/font/font_manager.h \
    ../examples/common/font/text_buffer_manager.h \
    ../examples/common/font/text_metrics.h \
    ../examples/common/font/utf8.h \
    \
    ../examples/common/imgui/droidsans.ttf.h \
    ../examples/common/imgui/roboto_regular.ttf.h \
    ../examples/common/imgui/robotomono_regular.ttf.h \
    ../examples/common/imgui/ocornut_imgui.h \
    ../examples/common/imgui/imgui.h \
    ../examples/common/imgui/scintilla.h \
    \
    ../examples/common/debugdraw/debugdraw.cpp \
    \
    ../3rdparty/ib-compress/indexbuffercompression.h \
    ../3rdparty/ib-compress/indexbuffercompressionformat.h \
    ../3rdparty/ib-compress/indexbufferdecompression.h \
    ../3rdparty/ib-compress/indexcompressionconstants.h \
    ../3rdparty/ib-compress/readbitstream.h \
    ../3rdparty/ib-compress/writebitstream.h

RESOURCES += assets.qrc runtime.qrc
