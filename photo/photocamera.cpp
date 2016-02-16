#include "photocamera.h"
#include <QDebug>
#include <QStringList>
#include <math.h>
#include <cmath>

PhotoCamera::PhotoCamera()
{
    intrinsicMatrix     = Eigen::Matrix4f::Identity();
    extrinsicMatrix     = Eigen::Matrix4f::Identity();
    rotationMatrix      = Eigen::Matrix4f::Identity();
    translationMatrix   = Eigen::Matrix4f::Identity();

    focalLength         = 47.3606;
    pixelSize           << 0.0319598, 0.0319598;
    oneOverDx           = 1/pixelSize(0);
    oneOverDy           = 1/pixelSize(1);

//    buildIntrinsic();
//    buildExtrinsic();
}

bool PhotoCamera::initializeFromMeshLab(QDomElement &element)
{
    QStringList attTemp;
    //Get Translation
    attTemp = element.attribute("TranslationVector").split(" ");
    Eigen::Vector4f translation;
    for(int i = 0; i < attTemp.count(); i++){
        translation(i) = attTemp[i].toFloat();
    }
    translationMatrix.translation() = translation.head(3);
    //translationMatrix.col(3) = translation;

    //Get Center;
    attTemp = element.attribute("CenterPx").split(" ");
    principalPoint << attTemp.at(0).toFloat(), attTemp.at(1).toFloat();


    //Get RotationMatrix;
    attTemp = element.attribute("RotationMatrix").split(" ");
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            rotationMatrix(i, j) = attTemp[i*4 + j].toFloat();
        }
    }

    //Get Viewport;
    attTemp = element.attribute("ViewportPx").split(" ");
    viewport << attTemp.at(0).toFloat(), attTemp.at(1).toFloat();

    //Lens Distortion;
    attTemp = element.attribute("LensDistortion").split(" ");
    distortion << attTemp.at(0).toFloat(), attTemp.at(1).toFloat();
    //std::cout << distortion.transpose() << std::endl;

    //PixelSize;
    attTemp = element.attribute("PixelSizeMm").split(" ");
    pixelSize << attTemp.at(0).toFloat(), attTemp.at(1).toFloat();
    oneOverDx           = 1/pixelSize(0);
    oneOverDy           = 1/pixelSize(1);
    //std::cout << pixelSize.transpose() << std::endl;

    //Focal
    focalLength = element.attribute("FocalMm").toFloat();

    buildExtrinsic();
    buildIntrinsic();
    buildProjection();

//    cameraCenter = center(intrinsicMatrix.topLeftCorner(3,4)*extrinsicMatrix);
    cameraCenter = center(intrinsicMatrix.matrix().topLeftCorner(3,4)*extrinsicMatrix.matrix());
    return true;
}

void PhotoCamera::buildIntrinsic()
{
    //Projection Matrix
    Eigen::Matrix3f Mi;
//    cout << "focal length: " << focalLength << endl;
//    cout << "1/dx: " << oneOverDx << endl;
//    cout << "1/dy: " << oneOverDy << endl;
    Mi << -focalLength*oneOverDx, 0, principalPoint(0),
            0, -focalLength*oneOverDy, principalPoint(1),
            0, 0, 1;
    intrinsicMatrix.topLeftCorner(3, 3) = Mi;
}

void PhotoCamera::buildExtrinsic()
{
    //View Matrix
    extrinsicMatrix = rotationMatrix*translationMatrix;
}

void PhotoCamera::buildProjection()
{
      float fov     = (2 * std::atan((getHeight()/2.0)/-intrinsicMatrix(1,1)));
      float ratio   = getWidth()/(getHeight() * pixelSize(0)/pixelSize(1));
      float f       =  1/ tan(fov/2);
      float near    = 0.1;
      float far     = 10000;
//      cout << "fov: " << fov << endl;
//      cout << "ratio: " << ratio << endl;

      Eigen::Matrix4f perspective = Eigen::Matrix4f::Zero();
      perspective(0,0) = f/ratio;
      perspective(1,1) = f;
      perspective(2,2) = (far + near)/(near - far);
      perspective(3,2) = -1;
      perspective(2,3) = (2 * far * near)/(near - far);

      projectionMatrix = perspective;
}

float PhotoCamera::getHeight()
{
    return viewport(1);
}

float PhotoCamera::getWidth()
{
    return viewport(0);
}

Eigen::Vector4f PhotoCamera::center(const Eigen::MatrixXf &camera)
{
    Eigen::Matrix3f C1, C2, C3, C4;
    C1 << camera.col(1), camera.col(2), camera.col(3);
    C2 << camera.col(0), camera.col(2), camera.col(3);
    C3 << camera.col(0), camera.col(1), camera.col(3);
    C4 << camera.col(0), camera.col(1), camera.col(2);

    Eigen::Vector4f C;
    C << C1.determinant(), -C2.determinant(), C3.determinant(), -C4.determinant();
    return C;
}

Eigen::Vector2f PhotoCamera::project(const Eigen::Vector3f &point)
{
    Eigen::MatrixXf P = intrinsicMatrix*extrinsicMatrix.matrix();
    Eigen::Vector4f pointH;
    pointH << point(0), point(1), point(2), 1;
    Eigen::Vector3f pixelH = P*pointH;
    return pixelH.hnormalized();
}

Eigen::Vector3f PhotoCamera::unproject(const Eigen::Vector2f &pixel)
{
    Eigen::Vector3f pixelH;
    pixelH << pixel(0), pixel(1), 1;
    Eigen::MatrixXf pseudoInverse = getPseudoInverse();
    Eigen::Vector4f XH = pseudoInverse*pixelH;
    Eigen::Vector3f p = XH.hnormalized();
    Eigen::Vector3f ray = p - cameraCenter.hnormalized();
    ray /= ray.norm();

    return ray;
}

Eigen::MatrixXf PhotoCamera::getPseudoInverse()
{
    Eigen::MatrixXf P = intrinsicMatrix*extrinsicMatrix.matrix();
    Eigen::MatrixXf Pt = P.transpose();
    Eigen::MatrixXf pseudoInverse = Pt*(P*Pt).inverse();
    return pseudoInverse;
}

