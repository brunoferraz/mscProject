#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GL/glew.h>
#include <QWidget>

#include <utils/qttrackballwidget.hpp>
#include <photo/multiTextureManagerObj.h>
#include <rendertexture.hpp>
#include <phongshader.hpp>
#include <depthMap.hpp>
#include <multi.hpp>
#include <multiMask.hpp>
#include <interface.h>
#include <QDebug>
#include <QDate>
#include <iostream>
#include <QDateTime>

class GLWidget : public Tucano::QtTrackballWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    void initialize();
    void paintGL();
    void reload();
    void changeCam();

    void saveImage();

    MultiTextureManagerObj mTextManagerObj;
    Tucano::Camera calibrationCamera;
    Tucano::Camera *currentCamera;

    inline void nextCamera(){ mTextManagerObj.nextPhoto();
                              mTextManagerObj.calibrateCamera(calibrationCamera);
                              update();
                            }
    inline void prevCamera(){ mTextManagerObj.prevPhoto();
                              mTextManagerObj.calibrateCamera(calibrationCamera);
                              update();
                            }


signals:
private:
    Effects::Phong phong;
    Effects::DepthMap depthMap;
    Effects::Multi multi;
    Effects::MultiMask multiMask;
    Effects::RenderTexture renderTexture;
    Texture image_Texture;
protected:
    void mousePressEvent(QMouseEvent *ev);

public slots:

};

#endif // GLWIDGET_H
