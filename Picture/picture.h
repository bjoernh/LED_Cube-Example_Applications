#ifndef PICTURE_H
#define PICTURE_H

#include <cube/cube.h>
#include "Image.h"
#include <string>

class Picture : public cube::CubeApp {
public:
    Picture(int argc, char *argv[]);
    bool loop() override;

private:
    void drawImage(cube::ScreenNumber screenNr, cube::Vec2i topLeftPoint, Image &image, cube::Vec2i imageStartPoint);
    bool loadImage(std::string filepath);

    bool error{false};
    std::string error_msg;
    Image autoload;
    cube::Joystick joystick_;
};

#endif //PICTURE_H