CONFIG(c++11): C11 = -c11
CONFIG(debug, debug|release): DBG = dbg
else: DBG = rel

DESTDIR = build-$$[QMAKE_SPEC]$$C11
OBJECTS_DIR = build-$$[QMAKE_SPEC]$$C11/$${TARGET}.$$DBG
MOC_DIR = build-$$[QMAKE_SPEC]$$C11/$${TARGET}.$$DBG
UI_DIR = build-$$[QMAKE_SPEC]$$C11/$${TARGET}.$$DBG
RCC_DIR = build-$$[QMAKE_SPEC]$$C11/$${TARGET}.$$DBG
