#include "multiTextureManagerObj.h"
#include <sstream>
#include <util/util.h>

MultiTextureManagerObj::MultiTextureManagerObj()
{
    MultiTextureManagerObj::currentPhotoIndex = 0;
}

bool MultiTextureManagerObj::initializeFromMeshLab(QString path, QString photoPath)
{
    QDomDocument doc;
    QFile file(path);
    qDebug() << file.exists();

    cout << "mlp : " << path.toStdString().c_str() << endl;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "failed to open";
        return -1;
    }else{
        if(!doc.setContent(&file))
        {
            qDebug() << "failed to load document";
            return -1;
        }
        file.close();
    }
    QDomElement root = doc.firstChildElement();
    QDomNodeList meshTree = root.elementsByTagName("MeshGroup");
    QDomNodeList rasterTree = root.elementsByTagName("MLRaster");
    //QNodeList percorre por indice porem vc precisa saber o nome da tag
    filename = root.firstChildElement().firstChildElement().attribute("filename").toStdString();
    label    = root.firstChildElement().firstChildElement().attribute("label").toStdString();

    cout << filename.c_str() << endl;

    openMesh(Util::debugOpenPath+filename);
    //mesh.resetModelMatrix();

    std::stringstream m(root.firstChildElement().firstChildElement().firstChildElement().text().toStdString());
    int i = 0;
    float a, b, c, d;
    while(m >> a >> b >> c >> d){
        Eigen::Vector4f v;
        v << a, b, c, d;
        modelMatrix.row(i) = v.transpose();
        i++;
    }
    //get raster info
    int total = rasterTree.count();
    for(int i =0; i < total; i++){
        cout << "raster " << i << " / " << total << endl;
        RasterInfo *info = new RasterInfo();
        info->label = rasterTree.at(i).toElement().attribute("label").toStdString();
        if(photoPath==""){
            //use defautl path from mesh lab file
            info->filename = rasterTree.at(i).firstChild().nextSibling().toElement().attribute("fileName").toStdString();
        }else{
            //set your own path
            info->filename = photoPath.toStdString() + rasterTree.at(i).firstChild().nextSibling().toElement().attribute("fileName").toStdString();
        }
        cout << "loading " << info->filename.c_str() << endl;
        QImage img(QString::fromStdString(info->filename));
        QImage glImage = QGLWidget::convertToGLFormat(img);
        info->baseTexture.create(glImage.width(), glImage.height(), glImage.bits());

        PhotoCamera cam;
        QDomElement camDom  = rasterTree.at(i).firstChildElement();
        cam.initializeFromMeshLab(camDom);
        info->photoCamera    = cam;
        rasterGroup.push_back(info);
    }

    return true;
}
Eigen::Matrix4f *MultiTextureManagerObj::getProjectionMatrix()
{
    return &rasterGroup.at(currentPhotoIndex)->photoCamera.projectionMatrix;
}
Eigen::Affine3f *MultiTextureManagerObj::getViewMatrix()
{
    return &rasterGroup.at(currentPhotoIndex)->photoCamera.extrinsicMatrix;
}
Eigen::Matrix4f *MultiTextureManagerObj::getModelMatrix()
{
    return &modelMatrix;
}

Tucano::Texture *MultiTextureManagerObj::getBaseTexture()
{
    return &rasterGroup.at(currentPhotoIndex)->baseTexture;
}

Tucano::Texture *MultiTextureManagerObj::getBaseTextureAt(int i)
{
    return &rasterGroup.at(i)->baseTexture;
}

Tucano::Mesh *MultiTextureManagerObj::getMesh()
{
    return &mesh;
}

void MultiTextureManagerObj::calibrateCamera(Tucano::Camera &c)
{
    //Receive the camera and calibrate it using info from de current raster image
    //VIEW PORT MUST BE SET BASED ON YOUR WIDGET SIZE FROM OUTSIDE
    PhotoCamera &cam = rasterGroup.at(currentPhotoIndex)->photoCamera;
    RasterInfo *info = rasterGroup.at(currentPhotoIndex);

    Eigen::Affine3f view = cam.extrinsicMatrix;
    c.setProjectionMatrix(cam.projectionMatrix);
    c.setViewMatrix(view);
}

Eigen::Vector3f MultiTextureManagerObj::getCenterCamera()
{
    Tucano::Camera c;
    PhotoCamera &cam = rasterGroup.at(currentPhotoIndex)->photoCamera;

    Eigen::Affine3f view = cam.extrinsicMatrix;
    c.setProjectionMatrix(cam.projectionMatrix);
    c.setViewMatrix(view);
    return c.getCenter();
}

void MultiTextureManagerObj::changePhotoReferenceTo(int n)
{
    int next = n;
    if(next >= rasterGroup.count()) next = 0;
    if(next < 0) next = rasterGroup.count()-1;
    currentPhotoIndex = next;
}

void MultiTextureManagerObj::changePhotoReferenceIn(int n)
{
    int next = currentPhotoIndex + n;
    if(next >= rasterGroup.count()) next = 0;
    if(next < 0) next = rasterGroup.count()-1;
    currentPhotoIndex = next;
}

void MultiTextureManagerObj::openMesh(string filename)
{
    QString str (filename.c_str());
    QStringList strlist = str.split(".");
    QString extension = strlist[strlist.size()-1];

    if (extension.compare("ply") != 0 && extension.compare("obj") != 0)
    {
        cerr << "file format [" << extension.toStdString() << "] not supported" << endl;
        return;
    }

    mesh.reset();

    if (extension.compare("ply") == 0)
    {
        Tucano::MeshImporter::loadPlyFile(&mesh, filename);
    }
    if (extension.compare("obj") == 0)
    {
        Tucano::MeshImporter::loadObjFile(&mesh, filename);
    }
}
