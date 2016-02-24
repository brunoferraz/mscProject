#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>

class Interface
{
public:
    Interface();
    enum CAMERA_TYPES{FREE, CALIBRATED};
    static int camera;
    static bool showBackgroundImage;
    static bool eyeDropper;
    static float alphaBackgroundImage;
    static std::string infoBox;

};

#endif // INTERFACE_H
