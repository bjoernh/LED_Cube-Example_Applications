#include "picture.h"

#include <cube/cube.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

namespace {
std::string filepath;
fs::file_time_type lastModificationTime;
int animationPrescale = 2;
}  // namespace

Picture::Picture(int argc, char *argv[]) : cube::CubeApp("picture", 40) {
    const char* home = std::getenv("HOME");
    filepath = (home ? std::string(home) : std::string("/tmp")) + "/pictures/autoload.png";

    if (argc > 1) {
        filepath = std::string(argv[argc - 1]);
        if (argc == 4) {
            if (std::string(argv[1]) == "-s") { // speed
                int temp = std::stoi(argv[2]);
                if (temp >= 0 && temp <= getFps() * 4) {
                    animationPrescale = temp;
                }
            }
        }
    }

    if (!loadImage(filepath)) {
        error = true;
    }
}

bool Picture::loadImage(std::string path) {
    if (!fs::exists(path)) {
        std::string msg = "no image";
        const cube::Vec2i centered{cube::Font::centered, cube::Font::centered};
        drawText(cube::ScreenNumber::front, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::right, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::back, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::left, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::top, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::bottom, centered, cube::Color::white(), msg);
        return false;
    }

    if (autoload.loadImage(path)) {
        lastModificationTime = fs::last_write_time(fs::path(path));
        if (autoload.getWidth() == 384 && autoload.getHeight() % 64 == 0) {
            return true;
        } else {
            error_msg = "wrong format";
        }
    } else {
        std::string msg = "no image";
        const cube::Vec2i centered{cube::Font::centered, cube::Font::centered};
        drawText(cube::ScreenNumber::front, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::right, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::back, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::left, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::top, centered, cube::Color::white(), msg);
        drawText(cube::ScreenNumber::bottom, centered, cube::Color::white(), msg);
    }
    return false;
}

bool Picture::loop() {
    static int loopcount = 0;
    static int verticalPos = 0;

    /* If file has wrong format or does not exist, then only display an error msg on all screens */
    if (error) {
        drawText(cube::ScreenNumber::front, {cube::Font::centered, cube::Font::centered}, cube::Color::white(), error_msg);
        drawText(cube::ScreenNumber::right, {cube::Font::centered, cube::Font::centered}, cube::Color::white(), error_msg);
        drawText(cube::ScreenNumber::back, {cube::Font::centered, cube::Font::centered}, cube::Color::white(), error_msg);
        drawText(cube::ScreenNumber::left, {cube::Font::centered, cube::Font::centered}, cube::Color::white(), error_msg);
        drawText(cube::ScreenNumber::top, {cube::Font::centered, cube::Font::centered}, cube::Color::white(), error_msg);
        drawText(cube::ScreenNumber::bottom, {cube::Font::centered, cube::Font::centered}, cube::Color::white(), error_msg);
        return true;
    }

    if (fs::exists(filepath) && fs::last_write_time(fs::path(filepath)) > lastModificationTime) {
        loadImage(filepath);
        lastModificationTime = fs::last_write_time(fs::path(filepath));
    }

    clear();

    if (joystick_.justPressed(cube::Btn::A)) {
        verticalPos += 64;
        if (verticalPos > autoload.getHeight() - 64) {
            verticalPos = 0;
        }
    }

    if (animationPrescale > 0) {
        if ((loopcount % animationPrescale) == 0) {
            verticalPos += 64;
            if (verticalPos > autoload.getHeight() - 64) {
                verticalPos = 0;
            }
        }
    }

    drawImage(cube::ScreenNumber::top, cube::Vec2i{0, 0}, autoload, cube::Vec2i{0, verticalPos});
    drawImage(cube::ScreenNumber::left, cube::Vec2i{0, 0}, autoload, cube::Vec2i{64, verticalPos});
    drawImage(cube::ScreenNumber::front, cube::Vec2i{0, 0}, autoload, cube::Vec2i{128, verticalPos});
    drawImage(cube::ScreenNumber::right, cube::Vec2i{0, 0}, autoload, cube::Vec2i{192, verticalPos});
    drawImage(cube::ScreenNumber::back, cube::Vec2i{0, 0}, autoload, cube::Vec2i{256, verticalPos});
    drawImage(cube::ScreenNumber::bottom, cube::Vec2i{0, 0}, autoload, cube::Vec2i{320, verticalPos});

    loopcount++;
    return true;
}

void Picture::drawImage(cube::ScreenNumber screenNr, cube::Vec2i topLeftPoint,
                        Image &image, cube::Vec2i imageStartPoint) {
    for (int cols = 0; cols < 64; cols++) {
        if (cols > cube::CUBE_MAX_INDEX || cols < 0) {
            break;
        }
        for (int rows = 0; rows < 64; rows++) {
            if (rows > cube::CUBE_MAX_INDEX || rows < 0) {
                break;
            }
            setPixel3D(
                getPointOnScreen(screenNr, cube::Vec2i{cols + topLeftPoint.x,
                                                      rows + topLeftPoint.y}),
                image.at(cols + imageStartPoint.x, rows + imageStartPoint.y));
        }
    }
}
