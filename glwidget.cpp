#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : Tucano::QtTrackballWidget(parent)
{

}

void GLWidget::initialize()
{
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mTextManagerObj.initializeFromMeshLab(QString("urna/urna.mlp"), QString("./urna/fotos/"));

    Tucano::QtTrackballWidget::initialize();
//    Tucano::QtTrackballWidget::openMesh("./models/toy.obj");

    string shaders_dir("./effects/shaders/");
    phong.setShadersDir(shaders_dir);
    phong.initialize();

    depthMap.setShadersDir(shaders_dir);
    depthMap.initialize();

    renderTexture.setShadersDir(shaders_dir);
    renderTexture.initialize();

    multi.setShadersDir(shaders_dir);
    multi.initialize();

    calibrationCamera.setViewport(Eigen::Vector2f(this->size().width(), this->size().height()));
    mTextManagerObj.calibrateCamera(calibrationCamera);
//    mTextManagerObj.getMesh()->normalizeModelMatrix();
    Interface::camera = Interface::CAMERA_TYPES::CALIBRATED;
    changeCam();
}

void GLWidget::paintGL()
{
    makeCurrent();

    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);




//    phong.render(*mTextManagerObj.getMesh(), *currentCamera, light_trackball);
      multi.render(mTextManagerObj, *currentCamera, light_trackball);
//    depthMap.render(*mTextManagerObj.getMesh(), calibrationCamera, light_trackball);

    if(Interface::showBackgroundImage)
    {
        renderTexture.imageAlpha = Interface::alphaBackgroundImage;
        renderTexture.renderTexture(*mTextManagerObj.getBaseTexture(), Eigen::Vector2i(this->width(), this->height()));
    }

    if(Interface::camera == Interface::CAMERA_TYPES::FREE)
    {
        camera.render();
    }
}

void GLWidget::reload()
{
    phong.reloadShaders();
    depthMap.reloadShaders();
    renderTexture.reloadShaders();
    multi.reloadShaders();
}

void GLWidget::changeCam()
{
    if(Interface::camera == Interface::CAMERA_TYPES::FREE)
    {
        mTextManagerObj.getMesh()->setModelMatrix(Eigen::Affine3f::Identity());
        mTextManagerObj.getMesh()->normalizeModelMatrix();
        currentCamera = &camera;
    }else if(Interface::camera == Interface::CAMERA_TYPES::CALIBRATED){
        mTextManagerObj.getMesh()->setModelMatrix(Eigen::Affine3f::Identity());
        currentCamera = &calibrationCamera;
    }
}

void GLWidget::mousePressEvent(QMouseEvent *ev)
{
    //cout << ev->x() << ev->y() << endl;
    cout << multi.getColorAt(Eigen::Vector2i(ev->x(), ev->y())).transpose() << endl;
    update();
}
