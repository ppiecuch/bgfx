TEMPLATE = app
QT += \
  gui openglextensions \  # for library
  gui-private widgets     # for demo application
CONFIG += c++11 debug
CONFIG -= app_bundle

INCLUDEPATH += ../examples/common
!bgfx_demo_combined:DEFINES += ENTRY_CONFIG_IMPLEMENT_MAIN=1

# bgfx codes:
macx|ios: SOURCES += ../src/amalgamated.mm
SOURCES += ../src/amalgamated.cpp
# 3rd party libsraries:
SOURCES += ../3rdparty/bx/src/bx-amalgamated.cpp
# demo codes:
macx|ios: SOURCES += ../examples/common/osx_utils.mm
SOURCES += \
    ../examples/common/bounds.cpp \
    ../examples/common/camera.cpp \
    ../examples/common/cube_atlas.cpp \
    ../examples/common/example-glue.cpp \
    ../examples/common/qt_io.cpp \
    ../examples/common/bgfx_utils.cpp \
    \
    ../examples/common/entry/cmd.cpp \
    ../examples/common/entry/input.cpp \
    ../examples/common/entry/entry.cpp \
    ../examples/common/entry/entry_qt.cpp \
    \
    ../examples/common/ps/particle_system.cpp \
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
    ../3rdparty/ib-compress/indexbufferdecompression.cpp

HEADERS += \
    ../examples/common/bounds.h \
    ../examples/common/camera.h \
    ../examples/common/bgfx_utils.h \
    ../examples/common/osx_utils.h \
    \
    ../examples/common/entry/dbg.h \
    \
    ../examples/common/ps/particle_system.h \
    \
    ../examples/common/debugdraw/debugdraw.h \
    \
    ../examples/common/font/font_manager.h \
    ../examples/common/font/text_buffer_manager.h \
    ../examples/common/font/text_metrics.h \
    ../examples/common/font/utf8.h \
    \
    ../examples/common/imgui/droidsans.ttf.h \
    ../examples/common/imgui/roboto_regular.ttf.h \
    ../examples/common/imgui/robotomono_regular.ttf.h \
    ../examples/common/imgui/imgui.h \
    ../examples/common/imgui/scintilla.h \
    \
    ../3rdparty/ib-compress/indexbuffercompression.h \
    ../3rdparty/ib-compress/indexbuffercompressionformat.h \
    ../3rdparty/ib-compress/indexbufferdecompression.h \
    ../3rdparty/ib-compress/indexcompressionconstants.h \
    ../3rdparty/ib-compress/readbitstream.h \
    ../3rdparty/ib-compress/writebitstream.h

# build game resources:
RES_EXT = .rcc
RES_RCC = $$PWD/assets.rcc $$PWD/runtime.rcc
RES_QRC = $$PWD/assets.qrc $$PWD/runtime.qrc
makeResRcc.name = Build rcc resources
makeResRcc.input = RES_QRC
makeResRcc.output = $${PWD}/${QMAKE_FILE_IN_BASE}$${RES_EXT}
makeResRcc.commands =  $$[QT_INSTALL_PREFIX]/bin/rcc -binary "${QMAKE_FILE_IN}" -o "${QMAKE_FILE_OUT}"
makeResRcc.CONFIG += target_predeps
makeResRcc.variable_out = OTHER_FILES
QMAKE_EXTRA_COMPILERS += makeResRcc

mac:CONFIG(app_bundle):equals(TEMPLATE, "app") {
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8

    copydata.commands = $(COPY_DIR) $$RES_RCC $$DESTDIR/$${TARGET}.app/Contents/MacOS
    first.depends = $(first) copydata

    export(first.depends)
    export(copydata.commands)

    QMAKE_EXTRA_TARGETS += first copydata
} else {
    copydata.commands = $(COPY_DIR) $$RES_RCC $$DESTDIR
    first.depends = $(first) copydata

    export(first.depends)
    export(copydata.commands)

    QMAKE_EXTRA_TARGETS += first copydata
}

win32 {
    run_file=$$DESTDIR/$${TARGET}.cmd
    L="SET PATH=$$[QT_INSTALL_LIBS]:%PATH%"
    write_file($$run_file, L)
    L="$${TARGET}.exe"
    write_file($$run_file, L)
}

include("bimg.pri")
