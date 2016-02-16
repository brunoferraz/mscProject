#ifndef INTERFACE_H
#define INTERFACE_H


class Interface
{
public:
    Interface();
    enum CAMERA_TYPES{FREE, CALIBRATED};
    static int camera;
    static bool showBackgroundImage;
    static float alphaBackgroundImage;

};

#endif // INTERFACE_H
