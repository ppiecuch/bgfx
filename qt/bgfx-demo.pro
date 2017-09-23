CONFIG += bgfx_demo_combined

SOURCES += \
    ../examples/00-helloworld/helloworld.cpp \
    ../examples/01-cubes/cubes.cpp \
    ../examples/02-metaballs/metaballs.cpp \
    ../examples/03-raymarch/raymarch.cpp \
    ../examples/04-mesh/mesh.cpp \
    ../examples/05-instancing/instancing.cpp \
    ../examples/06-bump/bump.cpp \
    ../examples/07-callback/callback.cpp \
    ../examples/08-update/update.cpp \
    ../examples/09-hdr/hdr.cpp \
    ../examples/10-font/font.cpp \
    ../examples/11-fontsdf/fontsdf.cpp \
    ../examples/12-lod/lod.cpp \
    ../examples/13-stencil/stencil.cpp \
    ../examples/14-shadowvolumes/shadowvolumes.cpp \
    ../examples/15-shadowmaps-simple/shadowmaps_simple.cpp \
    ../examples/16-shadowmaps/shadowmaps.cpp \
    ../examples/17-drawstress/drawstress.cpp \
    ../examples/18-ibl/ibl.cpp \
    ../examples/19-oit/oit.cpp \
    ../examples/20-nanovg/nanovg.cpp \
    ../examples/22-windows/windows.cpp \
    ../examples/21-deferred/deferred.cpp \
    ../examples/23-vectordisplay/main.cpp ../examples/23-vectordisplay/vectordisplay.cpp \
    ../examples/24-nbody/nbody.cpp \
    ../examples/25-c99/helloworld-c.c \
    ../examples/26-occlusion/occlusion.cpp \
    ../examples/27-terrain/terrain.cpp \
    ../examples/28-wireframe/wireframe.cpp \
    ../examples/29-debugdraw/debugdrawtest.cpp \
    ../examples/30-picking/picking.cpp \
    ../examples/31-rsm/reflectiveshadowmap.cpp \
    ../examples/32-particles/particles.cpp
HEADERS += \
    ../examples/00-helloworld/logo.h \
    ../examples/02-metaballs/fs_metaballs.bin.h ../examples/02-metaballs/vs_metaballs.bin.h \
    ../examples/17-drawstress/fs_drawstress.bin.h ../examples/17-drawstress/vs_drawstress.bin.h \
    ../examples/20-nanovg/blendish.h \
    ../examples/23-vectordisplay/vectordisplay.h

include("common.pri")
include("common-demo.pri")
include("QtBgfx/QtBgfx.pri")
