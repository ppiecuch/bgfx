INCLUDEPATH += \
    $$PWD/../3rdparty/bimg/include \
    $$PWD/../3rdparty/bimg/3rdparty \
    $$PWD/../3rdparty/bimg/3rdparty/iqa/include \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt

exists(../3rdparty/bimg/src/bimg-amalgamated.cpp):SOURCES += ../3rdparty/bimg/src/bimg-amalgamated.cpp
else:SOURCES += \
    $$PWD/../3rdparty/bimg/src/image_decode.cpp \
    $$PWD/../3rdparty/bimg/src/image_encode.cpp \
    $$PWD/../3rdparty/bimg/src/image.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/etc1/etc1.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/etc2/ProcessRGB.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/etc2/Tables.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/iqa/source/convolve.c \
    $$PWD/../3rdparty/bimg/3rdparty/iqa/source/decimate.c \
    $$PWD/../3rdparty/bimg/3rdparty/iqa/source/math_utils.c \
    $$PWD/../3rdparty/bimg/3rdparty/iqa/source/ms_ssim.c \
    $$PWD/../3rdparty/bimg/3rdparty/iqa/source/mse.c \
    $$PWD/../3rdparty/bimg/3rdparty/iqa/source/psnr.c \
    $$PWD/../3rdparty/bimg/3rdparty/iqa/source/ssim.c \
    $$PWD/../3rdparty/bimg/3rdparty/libsquish/alpha.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/libsquish/clusterfit.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/libsquish/colourblock.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/libsquish/colourfit.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/libsquish/colourset.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/libsquish/maths.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/libsquish/rangefit.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/libsquish/singlecolourfit.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/libsquish/squish.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc6h/zoh.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc6h/zoh_utils.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc6h/zohone.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc6h/zohtwo.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl_mode0.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl_mode1.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl_mode2.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl_mode3.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl_mode4.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl_mode5.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl_mode6.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl_mode7.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/bc7/avpcl_utils.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/nvmath/fitting.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/nvtt/nvtt.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/pvrtc/BitScale.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/pvrtc/MortonTable.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/pvrtc/PvrTcDecoder.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/pvrtc/PvrTcEncoder.cpp \
    $$PWD/../3rdparty/bimg/3rdparty/pvrtc/PvrTcPacket.cpp
