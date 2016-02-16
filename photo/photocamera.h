#ifndef PHOTOCAMERA_H
#define PHOTOCAMERA_H

#include <camera.hpp>
#include <QDomElement>
#include <QDomNode>

class PhotoCamera
{
public:
    PhotoCamera();

    Eigen::Vector4f cameraCenter;
    Eigen::Matrix4f intrinsicMatrix;
    Eigen::Affine3f extrinsicMatrix;

    Eigen::Affine3f translationMatrix;
    Eigen::Affine3f rotationMatrix;
    Eigen::Matrix4f projectionMatrix;

    Eigen::Vector2f principalPoint;
    Eigen::Vector2f viewport;
    Eigen::Vector2f pixelSize;
    Eigen::Vector2f distortion;

    float radialDistortion;
    float focalLength;
    float oneOverDx;
    float oneOverDy;

    bool initializeFromMeshLab(QDomElement& element);
    void buildIntrinsic();
    void buildExtrinsic();
    void buildProjection();

    float getHeight();
    float getWidth();

    void setDistortion(Eigen::Vector2f& d);
    void setRadialDistortion(Eigen::Vector2f& d);

    Eigen::Vector4f center(const Eigen::MatrixXf& camera);
    Eigen::Vector2f project(const Eigen::Vector3f& point);
    Eigen::Vector3f unproject(const Eigen::Vector2f& pixel);
    Eigen::MatrixXf getPseudoInverse();
};

#endif // PHOTOCAMERA_H
