CONFIG(c++11): C11 = -c11
CONFIG(debug, debug|release): DBG = dbg
else: DBG = rel

DESTDIR = $$PWD/build-$$[QMAKE_SPEC]$$C11
OBJECTS_DIR = $$PWD/build-$$[QMAKE_SPEC]$$C11/bgfx-objects.$$DBG
MOC_DIR = $$PWD/build-$$[QMAKE_SPEC]$$C11/bgfx-objects.$$DBG
UI_DIR = $$PWD/build-$$[QMAKE_SPEC]$$C11/bgfx-objects.$$DBG
RCC_DIR = $$PWD/build-$$[QMAKE_SPEC]$$C11/bgfx-objects.$$DBG

DEFINES += BGFX_CONFIG_DEBUG BGFX_CONFIG_RENDERER_OPENGL=1

INCLUDEPATH += \
    $$PWD/../3rdparty/bx/include \
    $$PWD/../3rdparty/khronos \
    $$PWD/../3rdparty \
    $$PWD/../include \
    $$PWD/../examples/common

macx {
    INCLUDEPATH += $$PWD/../3rdparty/bx/include/compat/osx
    LIBS += -framework Foundation -framework AppKit
}
win: INCLUDEPATH += $$PWD/../3rdparty/bx/include/compat/msvc
linux: INCLUDEPATH += $$PWD/../3rdparty/bx/include/compat/freebsd
