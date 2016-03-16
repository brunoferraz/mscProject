#-------------------------------------------------
#
# Project created by QtCreator 2015-11-25T13:34:11
#
#-------------------------------------------------

system(./preparebuild.sh)

QT       += core gui opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mscProjectClean
TEMPLATE = app

EIGEN_PATH  =  /usr/include/eigen3

TUCANO_PATH = $$PWD/../tucano

BUILDDIR = ./build/

LIBS += -lGLEW -lGLU

INCLUDEPATH +=  $$TUCANO_PATH/src $$EIGEN_PATH $$BUILDDIR/bin/effects $$TUCANO_PATH

OBJECTS_DIR =   $$BUILDDIR/obj
MOC_DIR =       $$BUILDDIR/moc
RCC_DIR =       $$BUILDDIR/rcc
UI_DIR =        $$BUILDDIR/ui
DESTDIR =       $$BUILDDIR/bin

QMAKE_CXXFLAGS += -DTUCANODEBUG

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    util/util.cpp \
    photo/multiTextureManagerObj.cpp \
    photo/photocamera.cpp \
    interface.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    util/util.h \
    photo/multiTextureManagerObj.h \
    photo/photocamera.h \
    $$TUCANO_PATH/src/utils/qttrackballwidget.hpp \
    ./effects/phongshader.hpp \
    ./effects/rendertexture.hpp \
    ./effects/multi.hpp \
    ./effects/multiMask.hpp \
    ./effects/depthMap.hpp \
    interface.h

FORMS    += mainwindow.ui

OTHER_FILES += ./effects/shaders/rendertexture.vert \
               ./effects/shaders/rendertexture.frag \
               ./effects/shaders/depthmap.vert \
               ./effects/shaders/depthmap.frag \
               ./effects/shaders/nonnormdepthmap.vert \
               ./effects/shaders/nonnormdepthmap.frag \
               ./effects/shaders/maskpass.vert \
               ./effects/shaders/maskpass.frag \
               ./effects/shaders/maskfusepass.vert \
               ./effects/shaders/maskfusepass.frag \
               ./effects/shaders/maskanglepass.vert \
               ./effects/shaders/maskanglepass.frag \
               ./effects/shaders/maskborderpass.vert \
               ./effects/shaders/maskborderpass.frag \
               ./effects/shaders/maskdepthpass.vert \
               ./effects/shaders/maskdepthpass.frag \
               ./effects/shaders/coordtf.vert \
               ./effects/shaders/coordtf.frag \
               ./effects/shaders/showFbo.vert \
               ./effects/shaders/showFbo.frag \
               ./effects/shaders/multipassLoop.vert \
               ./effects/shaders/multipassLoop.frag \
               ./effects/shaders/multipassLoop.geom \
               ./effects/shaders/multipass.vert \
               ./effects/shaders/multipass.frag \
               ./effects/shaders/multipass.geom \
               ./effects/shaders/simplepass.vert \
               ./effects/shaders/simplepass.geom \
               ./effects/shaders/simplepass.frag \
               ./effects/shaders/phongshader.vert \
               ./effects/shaders/phongshader.frag \
               ./effects/shaders/jumpflood.vert \
               ./effects/shaders/jumpflood.frag \
