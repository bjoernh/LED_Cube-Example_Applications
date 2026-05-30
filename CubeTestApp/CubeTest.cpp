#include "CubeTest.h"

#include <cube/cube.h>

using cube::Color;
using cube::Font;
using cube::ScreenNumber;
using cube::Vec2i;
using cube::Vec3i;

CubeTest::CubeTest() : cube::CubeApp("cubetestapp", 30) {}

bool CubeTest::loop() {
    static int loopcount = 0;
    clear();

    drawLine3D(Vec3i{0, 0, cube::CUBE_SIZE - loopcount % cube::CUBE_SIZE},
               Vec3i{cube::CUBE_SIZE, 0, cube::CUBE_SIZE - loopcount % cube::CUBE_SIZE},
               Color::red());
    drawLine3D(Vec3i{loopcount % cube::CUBE_SIZE, 0, cube::CUBE_SIZE},
               Vec3i{loopcount % cube::CUBE_SIZE, 0, 0},
               Color::blue());

    const Vec2i centered{Font::centered, Font::centered};
    drawText(ScreenNumber::front, centered, Color::white(), "Screen 0 front");
    drawText(ScreenNumber::right, centered, Color::white(), "Screen 1 right");
    drawText(ScreenNumber::back, centered, Color::white(), "Screen 2 back");
    drawText(ScreenNumber::left, centered, Color::white(), "Screen 3 left");
    drawText(ScreenNumber::top, centered, Color::white(), "Screen 4 top");
    drawText(ScreenNumber::bottom, centered, Color::white(), "Screen 5 bottom");

    loopcount++;
    return true;
}