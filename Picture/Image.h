#ifndef IMAGE_H
#define IMAGE_H

#include <cube/cube.h>
#include <vector>
#include <string>

namespace Imlib2 {
#include <Imlib2.h>
}

class Image {
public:
    Image();
    bool loadImage(std::string filepath);
    cube::Color at(int col, int row);

    int getWidth();
    int getHeight();
private:
    Imlib2::Imlib_Image image{nullptr};
    std::vector<cube::Color> imageData;
    unsigned int width{0};
    unsigned int height{0};
};

#endif //IMAGE_H
