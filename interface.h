#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include <QTextBrowser>

class Interface
{
public:
    Interface();
    enum CAMERA_TYPES{FREE, CALIBRATED};
    static int camera;
    static bool showBackgroundImage;
    static bool ShowCameras;
    static bool eyeDropper;
    static float alphaBackgroundImage;
    static QTextBrowser* infoBox;
};

#endif // INTERFACE_H
