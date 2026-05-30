#ifndef PORTAL_CUBE_H
#define PORTAL_CUBE_H

#include <cube/cube.h>
#include <string>

class PortalCube : public cube::CubeApp {
public:
    PortalCube();
    bool loop() override;

private:
    void drawFilledCircle(cube::ScreenNumber screen, int cx, int cy, int radius, cube::Color color);
    void drawHeart(cube::ScreenNumber screen, int cx, int cy, int size, cube::Color color);
    void drawCompanionFace(cube::ScreenNumber screen, cube::Color base, cube::Color frame, cube::Color cross, cube::Color heart);
    void drawStorageFace(cube::ScreenNumber screen, cube::Color base, cube::Color frame, cube::Color cross, cube::Color ring);
    void drawReflectionFace(cube::ScreenNumber screen, cube::Color base, cube::Color frame, cube::Color lens);

    enum class CubeType           { Companion = 0, Storage = 1, Reflection = 2, COUNT = 3 };
    enum class PulseMode          { Static = 0, Portal = 1, Neon = 2, COUNT = 3 };
    enum class StoragePulseMode   { Static = 0, Pulse = 1, COUNT = 2 };
    enum class ReflectionPulseMode{ Static = 0, Pulse = 1, COUNT = 2 };

    cube::Joystick        joystick_;
    CubeType              cubeType_               { CubeType::Companion };
    PulseMode             pulseMode_              { PulseMode::Static };
    StoragePulseMode      storagePulseMode_        { StoragePulseMode::Static };
    ReflectionPulseMode   reflectionPulseMode_    { ReflectionPulseMode::Static };

    bool dpadWasUp_    = false;
    bool dpadWasDown_  = false;
    bool dpadWasLeft_  = false;
    bool dpadWasRight_ = false;

    float glowPhase_  = 0.0f;
    float crossPhase_ = 0.0f;
};

#endif
