#include "interface.h"

Interface::Interface()
{

}
bool    Interface::showBackgroundImage  = true;
bool    Interface::eyeDropper = false;
float   Interface::alphaBackgroundImage = 0.0;
int     Interface::camera   = Interface::CAMERA_TYPES::FREE;
std::string Interface::infoBox = "InfoBox";
