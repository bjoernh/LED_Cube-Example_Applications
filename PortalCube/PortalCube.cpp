#include "PortalCube.h"

#include <cube/cube.h>
#include <algorithm>
#include <cmath>
#include <numbers>

namespace {
constexpr float kPi = std::numbers::pi_v<float>;
}  // namespace

PortalCube::PortalCube()
    : cube::CubeApp("portalcube", 30) {}

bool PortalCube::loop() {
    glowPhase_  = std::fmod(glowPhase_  + 0.04F, 2.0F * kPi);
    crossPhase_ = std::fmod(crossPhase_ + 0.025F, 2.0F * kPi);

    if (joystick_.isConnected()) {
        float ay = joystick_.axis(cube::Axis::LeftY);
        float ax = joystick_.axis(cube::Axis::LeftX);

        bool dpadUp    = (ay <= -0.5F);
        bool dpadDown  = (ay >=  0.5F);
        bool dpadLeft  = (ax <= -0.5F);
        bool dpadRight = (ax >=  0.5F);

        if (dpadRight && !dpadWasRight_) {
            int next = (static_cast<int>(cubeType_) + 1) % static_cast<int>(CubeType::COUNT);
            cubeType_ = static_cast<CubeType>(next);
        }
        if (dpadLeft && !dpadWasLeft_) {
            int prev = (static_cast<int>(cubeType_) - 1 + static_cast<int>(CubeType::COUNT)) % static_cast<int>(CubeType::COUNT);
            cubeType_ = static_cast<CubeType>(prev);
        }

        if (cubeType_ == CubeType::Companion) {
            if (dpadUp && !dpadWasUp_) {
                int next = (static_cast<int>(pulseMode_) + 1) % static_cast<int>(PulseMode::COUNT);
                pulseMode_ = static_cast<PulseMode>(next);
            }
            if (dpadDown && !dpadWasDown_) {
                int prev = (static_cast<int>(pulseMode_) - 1 + static_cast<int>(PulseMode::COUNT))
                           % static_cast<int>(PulseMode::COUNT);
                pulseMode_ = static_cast<PulseMode>(prev);
            }
        } else if (cubeType_ == CubeType::Storage) {
            if (dpadUp && !dpadWasUp_) {
                int next = (static_cast<int>(storagePulseMode_) + 1) % static_cast<int>(StoragePulseMode::COUNT);
                storagePulseMode_ = static_cast<StoragePulseMode>(next);
            }
            if (dpadDown && !dpadWasDown_) {
                int prev = (static_cast<int>(storagePulseMode_) - 1 + static_cast<int>(StoragePulseMode::COUNT))
                           % static_cast<int>(StoragePulseMode::COUNT);
                storagePulseMode_ = static_cast<StoragePulseMode>(prev);
            }
        } else {
            if (dpadUp && !dpadWasUp_) {
                int next = (static_cast<int>(reflectionPulseMode_) + 1) % static_cast<int>(ReflectionPulseMode::COUNT);
                reflectionPulseMode_ = static_cast<ReflectionPulseMode>(next);
            }
            if (dpadDown && !dpadWasDown_) {
                int prev = (static_cast<int>(reflectionPulseMode_) - 1 + static_cast<int>(ReflectionPulseMode::COUNT))
                           % static_cast<int>(ReflectionPulseMode::COUNT);
                reflectionPulseMode_ = static_cast<ReflectionPulseMode>(prev);
            }
        }

        dpadWasUp_    = dpadUp;
        dpadWasDown_  = dpadDown;
        dpadWasLeft_  = dpadLeft;
        dpadWasRight_ = dpadRight;
    }

    clear();

    if (cubeType_ == CubeType::Companion) {
        cube::Color base, frame, cross, heart;

        if (pulseMode_ == PulseMode::Static) {
            base = cube::Color::fromHSV(20.0F,  0.18F, 0.30F);
            frame = cube::Color::fromHSV(0.0F,  0.00F, 0.95F);
            heart = cube::Color::fromHSV(338.0F, 0.72F, 0.75F);
            cross = cube::Color::fromHSV(338.0F, 0.55F, 0.65F);

        } else if (pulseMode_ == PulseMode::Portal) {
            base = cube::Color::fromHSV(20.0F,  0.18F, 0.30F);
            frame = cube::Color::fromHSV(0.0F,  0.00F, 0.95F);
            float heartV = 0.60F + 0.30F * std::sin(glowPhase_);
            float crossV = 0.55F + 0.20F * std::sin(crossPhase_ + 0.8F);
            heart = cube::Color::fromHSV(338.0F, 0.72F, heartV);
            cross = cube::Color::fromHSV(338.0F, 0.55F, crossV);

        } else {
            base = cube::Color::fromHSV(20.0F,  0.10F, 0.08F);
            frame = cube::Color::fromHSV(210.0F, 0.15F, 0.98F);
            float pulse  = std::sin(glowPhase_);
            float heartV = 0.75F + 0.25F * pulse;
            float heartS = 0.95F - 0.30F * (pulse * 0.5F + 0.5F);
            float crossV = 0.70F + 0.20F * std::sin(crossPhase_ + 0.8F);
            float crossS = 0.90F - 0.25F * (std::sin(crossPhase_ + 0.8F) * 0.5F + 0.5F);
            heart = cube::Color::fromHSV(338.0F, heartS, heartV);
            cross = cube::Color::fromHSV(338.0F, crossS, crossV);
        }

        fillAll(base);
        for (auto screen : {cube::ScreenNumber::front, cube::ScreenNumber::back,
                            cube::ScreenNumber::left, cube::ScreenNumber::right,
                            cube::ScreenNumber::top, cube::ScreenNumber::bottom}) {
            drawCompanionFace(screen, base, frame, cross, heart);
        }

    } else if (cubeType_ == CubeType::Storage) {
        cube::Color base, frame, cross, ring;

        base = cube::Color::fromHSV(210.0F, 0.05F, 0.55F);
        frame = cube::Color::fromHSV(0.0F,  0.00F, 0.95F);
        cross = cube::Color::fromHSV(0.0F,  0.00F, 0.90F);

        if (storagePulseMode_ == StoragePulseMode::Pulse) {
            float ringV = 0.70F + 0.30F * std::sin(glowPhase_);
            float ringS = 0.85F - 0.25F * (std::sin(glowPhase_) * 0.5F + 0.5F);
            ring = cube::Color::fromHSV(210.0F, ringS, ringV);
        } else {
            ring = cube::Color::fromHSV(210.0F, 0.85F, 0.90F);
        }

        fillAll(base);
        for (auto screen : {cube::ScreenNumber::front, cube::ScreenNumber::back,
                            cube::ScreenNumber::left, cube::ScreenNumber::right,
                            cube::ScreenNumber::top, cube::ScreenNumber::bottom}) {
            drawStorageFace(screen, base, frame, cross, ring);
        }

    } else {
        cube::Color base, frame, lens;

        base = cube::Color::fromHSV(210.0F, 0.05F, 0.50F);
        frame = cube::Color::fromHSV(0.0F,  0.00F, 0.90F);

        if (reflectionPulseMode_ == ReflectionPulseMode::Pulse) {
            float lensV = 0.55F + 0.35F * std::sin(glowPhase_);
            lens = cube::Color::fromHSV(145.0F, 0.85F, lensV);
        } else {
            lens = cube::Color::fromHSV(145.0F, 0.85F, 0.70F);
        }

        fillAll(base);
        for (auto screen : {cube::ScreenNumber::front, cube::ScreenNumber::back,
                            cube::ScreenNumber::left, cube::ScreenNumber::right,
                            cube::ScreenNumber::top, cube::ScreenNumber::bottom}) {
            drawReflectionFace(screen, base, frame, lens);
        }
    }

    return true;
}

void PortalCube::drawFilledCircle(cube::ScreenNumber screen, int cx, int cy, int radius, cube::Color color) {
    for (int dy = -radius; dy <= radius; dy++) {
        int dx = static_cast<int>(std::sqrt(static_cast<float>(radius * radius - dy * dy) - 0.5F));
        drawLine2D(screen, cube::Vec2i{cx - dx, cy + dy}, cube::Vec2i{cx + dx, cy + dy}, color);
    }
}

void PortalCube::drawHeart(cube::ScreenNumber screen, int cx, int cy, int size, cube::Color color) {
    float fHr = static_cast<float>(size) * 0.5F;
    for (int dy = -size; dy <= size; dy++) {
        int iDrawY = dy - 2;
        bool bInSegment = false;
        int iSegStart = 0;

        for (int dx = -size; dx <= size; dx++) {
            float x   = static_cast<float>(dx) / static_cast<float>(size);
            float y   = -static_cast<float>(dy) / static_cast<float>(size);
            float val = std::pow(x*x + y*y - 1.0F, 3.0F) - x*x * y*y*y;
            bool bInside = (val <= 0.0F);

            if (bInside && !bInSegment) {
                iSegStart  = dx;
                bInSegment = true;
                if (dy <= 0) {
                    int dxL = static_cast<int>(std::sqrt(fHr*fHr - static_cast<float>(dy*dy) - 0.5F));
                    drawLine2D(screen, cube::Vec2i{cx - static_cast<int>(fHr) - dxL, cy + iDrawY},
                                       cube::Vec2i{cx - static_cast<int>(fHr) + dxL, cy + iDrawY}, color);
                    drawLine2D(screen, cube::Vec2i{cx + static_cast<int>(fHr) - dxL, cy + iDrawY},
                                       cube::Vec2i{cx + static_cast<int>(fHr) + dxL, cy + iDrawY}, color);
                    break;
                }
            } else if (!bInside && bInSegment) {
                drawLine2D(screen, cube::Vec2i{cx + iSegStart, cy + iDrawY},
                                   cube::Vec2i{cx + (dx - 1),  cy + iDrawY}, color);
                bInSegment = false;
            }
        }
    }
}

void PortalCube::drawCompanionFace(cube::ScreenNumber screen,
                                   cube::Color base, cube::Color frame,
                                   cube::Color cross, cube::Color heart)
{
    const int size   = 63;
    const int center = size / 2;
    const int wEnd   = center - 10;
    const int wBegin = center + 11;
    const int iSpace = 3;
    const int iRows  = 8;

    drawLine2D(screen, cube::Vec2i{center, 0},    cube::Vec2i{center, size},  cross);
    drawLine2D(screen, cube::Vec2i{0,    center}, cube::Vec2i{size,   center}, cross);

    for (int i = 0; i < iRows; ++i) {
        drawLine2D(screen, cube::Vec2i{0,            i},     cube::Vec2i{wEnd,           i},     frame);
        drawLine2D(screen, cube::Vec2i{wEnd+iSpace,  i},     cube::Vec2i{wBegin-iSpace,  i},     frame);
        drawLine2D(screen, cube::Vec2i{wBegin,       i},     cube::Vec2i{size,           i},     frame);

        drawLine2D(screen, cube::Vec2i{size-i, 0},            cube::Vec2i{size-i, wEnd},           frame);
        drawLine2D(screen, cube::Vec2i{size-i, wEnd+iSpace},  cube::Vec2i{size-i, wBegin-iSpace},  frame);
        drawLine2D(screen, cube::Vec2i{size-i, wBegin},       cube::Vec2i{size-i, size},           frame);

        drawLine2D(screen, cube::Vec2i{size,         size-i}, cube::Vec2i{wBegin,         size-i}, frame);
        drawLine2D(screen, cube::Vec2i{wBegin-iSpace,size-i}, cube::Vec2i{wEnd+iSpace,    size-i}, frame);
        drawLine2D(screen, cube::Vec2i{wEnd,         size-i}, cube::Vec2i{0,              size-i}, frame);

        drawLine2D(screen, cube::Vec2i{i, size},          cube::Vec2i{i, wBegin},          frame);
        drawLine2D(screen, cube::Vec2i{i, wBegin-iSpace}, cube::Vec2i{i, wEnd+iSpace},     frame);
        drawLine2D(screen, cube::Vec2i{i, wEnd},          cube::Vec2i{i, 0},               frame);
    }

    for (int i = iRows, dec = 1; i < wEnd; ++i, ++dec) {
        drawLine2D(screen, cube::Vec2i{0,          i},     cube::Vec2i{wEnd-dec,   i},     frame);
        drawLine2D(screen, cube::Vec2i{wBegin+dec, i},     cube::Vec2i{size,       i},     frame);
        drawLine2D(screen, cube::Vec2i{size,       size-i}, cube::Vec2i{wBegin+dec, size-i}, frame);
        drawLine2D(screen, cube::Vec2i{wEnd-dec,   size-i}, cube::Vec2i{0,          size-i}, frame);
    }

    drawFilledCircle(screen, center, center, 15, frame);
    drawHeart(screen, center, center, 9, heart);
}

void PortalCube::drawStorageFace(cube::ScreenNumber screen,
                                 cube::Color base, cube::Color frame,
                                 cube::Color cross, cube::Color ring)
{
    const int size   = 63;
    const int center = size / 2;
    const int wEnd   = center - 10;
    const int wBegin = center + 11;
    const int iSpace = 3;
    const int iRows  = 8;

    drawLine2D(screen, cube::Vec2i{center, 0},    cube::Vec2i{center, size},  cross);
    drawLine2D(screen, cube::Vec2i{0,    center}, cube::Vec2i{size,   center}, cross);

    for (int i = 0; i < iRows; ++i) {
        drawLine2D(screen, cube::Vec2i{0,            i},     cube::Vec2i{wEnd,           i},     frame);
        drawLine2D(screen, cube::Vec2i{wEnd+iSpace,  i},     cube::Vec2i{wBegin-iSpace,  i},     frame);
        drawLine2D(screen, cube::Vec2i{wBegin,       i},     cube::Vec2i{size,           i},     frame);

        drawLine2D(screen, cube::Vec2i{size-i, 0},            cube::Vec2i{size-i, wEnd},           frame);
        drawLine2D(screen, cube::Vec2i{size-i, wEnd+iSpace},  cube::Vec2i{size-i, wBegin-iSpace},  frame);
        drawLine2D(screen, cube::Vec2i{size-i, wBegin},       cube::Vec2i{size-i, size},           frame);

        drawLine2D(screen, cube::Vec2i{size,         size-i}, cube::Vec2i{wBegin,         size-i}, frame);
        drawLine2D(screen, cube::Vec2i{wBegin-iSpace,size-i}, cube::Vec2i{wEnd+iSpace,    size-i}, frame);
        drawLine2D(screen, cube::Vec2i{wEnd,         size-i}, cube::Vec2i{0,              size-i}, frame);

        drawLine2D(screen, cube::Vec2i{i, size},          cube::Vec2i{i, wBegin},          frame);
        drawLine2D(screen, cube::Vec2i{i, wBegin-iSpace}, cube::Vec2i{i, wEnd+iSpace},     frame);
        drawLine2D(screen, cube::Vec2i{i, wEnd},          cube::Vec2i{i, 0},               frame);
    }

    for (int i = iRows, dec = 1; i < wEnd; ++i, ++dec) {
        drawLine2D(screen, cube::Vec2i{0,          i},     cube::Vec2i{wEnd-dec,   i},     frame);
        drawLine2D(screen, cube::Vec2i{wBegin+dec, i},     cube::Vec2i{size,       i},     frame);
        drawLine2D(screen, cube::Vec2i{size,       size-i}, cube::Vec2i{wBegin+dec, size-i}, frame);
        drawLine2D(screen, cube::Vec2i{wEnd-dec,   size-i}, cube::Vec2i{0,          size-i}, frame);
    }

    drawFilledCircle(screen, center, center, 15, frame);
    drawFilledCircle(screen, center, center, 12, ring);
    drawFilledCircle(screen, center, center,  7, base);
}

void PortalCube::drawReflectionFace(cube::ScreenNumber screen,
                                    cube::Color base, cube::Color frame, cube::Color lens)
{
    const int size    = 63;
    const int center  = size / 2;
    const int corner  = 12;
    const int ringR   = 27;

    cube::Color dark = cube::Color::fromHSV(0.0F, 0.0F, 0.20F);

    for (int y = 0; y <= corner; ++y) {
        drawLine2D(screen, cube::Vec2i{0, y}, cube::Vec2i{corner, y}, base);
    }
    for (int y = 0; y <= corner; ++y) {
        drawLine2D(screen, cube::Vec2i{size-corner, y}, cube::Vec2i{size, y}, base);
    }
    for (int y = size-corner; y <= size; ++y) {
        drawLine2D(screen, cube::Vec2i{0, y}, cube::Vec2i{corner, y}, base);
    }
    for (int y = size-corner; y <= size; ++y) {
        drawLine2D(screen, cube::Vec2i{size-corner, y}, cube::Vec2i{size, y}, base);
    }

    drawLine2D(screen, cube::Vec2i{center, 0},          cube::Vec2i{center, center - ringR}, dark);
    drawLine2D(screen, cube::Vec2i{center, center+ringR}, cube::Vec2i{center, size},         dark);
    drawLine2D(screen, cube::Vec2i{0,          center}, cube::Vec2i{center - ringR, center}, dark);
    drawLine2D(screen, cube::Vec2i{center+ringR, center}, cube::Vec2i{size,         center}, dark);

    drawFilledCircle(screen, center, center, ringR, dark);
    drawFilledCircle(screen, center, center, 23,    lens);
    drawFilledCircle(screen, center, center,  5,    dark);
}
