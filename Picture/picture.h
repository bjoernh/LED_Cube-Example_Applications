#ifndef PICTURE_H
#define PICTURE_H

#include "CubeApplication.h"
#include "Joystick.h"
#include "Image.h"
#include <vector>

class Picture : public CubeApplication {
public:
    Picture(int argc, char *argv[]);

    bool loop();

private:
    bool loadImage(std::string filepath);
    bool error;
    std::string error_msg;
    Image autoload;
    std::vector<Joystick *> joysticks;
};


#endif //PICTURE_H