#include "Image.h"

#include <cube/cube.h>
#include <iostream>

Image::Image() {}

bool Image::loadImage(std::string filepath) {
    image = Imlib2::imlib_load_image(filepath.data());
    if (image) {
        Imlib2::imlib_context_set_image(image);
        Imlib2::imlib_image_set_changes_on_disk();
        width = Imlib2::imlib_image_get_width();
        height = Imlib2::imlib_image_get_height();
        imageData.resize(width * height, cube::Color::black());

        for (unsigned int cols = 0; cols < width; cols++) {
            for (unsigned int rows = 0; rows < height; rows++) {
                Imlib2::Imlib_Color tempColor;
                Imlib2::imlib_image_query_pixel(cols, rows, &tempColor);
                imageData[rows + cols * height] = cube::Color(tempColor.red, tempColor.green, tempColor.blue);
            }
        }
    } else {
        return false;
    }
    return true;
}

cube::Color Image::at(int col, int row) {
    unsigned int idx = static_cast<unsigned int>(row + col * height);
    if (idx < imageData.size()) {
        return imageData[idx];
    } else {
        return cube::Color::black();
    }
}

int Image::getHeight() {
    return static_cast<int>(height);
}

int Image::getWidth() {
    return static_cast<int>(width);
}
