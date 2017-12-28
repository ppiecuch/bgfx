CONFIG(c++11): C11 = -c11
CONFIG(debug, debug|release): DBG = dbg
else: DBG = rel

bgfx_demo_combined:DEMOCOMB = -demo

DESTDIR = $$PWD/build-$$[QMAKE_SPEC]$$C11
OBJECTS_DIR = $$PWD/build-$$[QMAKE_SPEC]$$C11/bgfx-objects$${DEMOCOMB}.$$DBG
MOC_DIR = $$PWD/build-$$[QMAKE_SPEC]$$C11/bgfx-objects.$$DBG
UI_DIR = $$PWD/build-$$[QMAKE_SPEC]$$C11/bgfx-objects.$$DBG
RCC_DIR = $$PWD/build-$$[QMAKE_SPEC]$$C11/bgfx-objects.$$DBG

DEFINES += \
	BGFX_CONFIG_DEBUG \
	BGFX_CONFIG_RENDERER_OPENGL=1

DEFINES += \
	__STDC_LIMIT_MACROS \
	__STDC_FORMAT_MACROS \
	__STDC_CONSTANT_MACROS
 
INCLUDEPATH += \
    $$PWD/../3rdparty/bx/include \
    $$PWD/../3rdparty/bx/3rdparty \
    $$PWD/../3rdparty/khronos \
    $$PWD/../3rdparty/khronos \
    $$PWD/../3rdparty \
    $$PWD/../include

macx {
    INCLUDEPATH += $$PWD/../3rdparty/bx/include/compat/osx
    LIBS += -framework Foundation -framework AppKit
}
ios {
    INCLUDEPATH += $$PWD/../3rdparty/bx/include/compat/ios
    LIBS += -framework Foundation
}
win32 {
	msvc*: INCLUDEPATH += $$PWD/../3rdparty/bx/include/compat/msvc
	gcc*: INCLUDEPATH += $$PWD/../3rdparty/bx/include/compat/mingw
	LIBS += -lgdi32 -luser32
}
linux {
    INCLUDEPATH += $$PWD/../3rdparty/bx/include/compat/freebsd
}
