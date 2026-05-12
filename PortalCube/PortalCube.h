#ifndef PORTAL_CUBE_H
#define PORTAL_CUBE_H

#include <CubeApplication.h>
#include <Joystick.h>
#include <string>

class PortalCube : public CubeApplication {
public:
    explicit PortalCube(std::string serverUri);
    bool loop() override;

private:
    void drawFilledCircle(ScreenNumber screen, int cx, int cy, int radius, Color color);
    void drawHeart(ScreenNumber screen, int cx, int cy, int size, Color color);
    void drawCompanionFace(ScreenNumber screen, Color base, Color frame, Color cross, Color heart);
    void drawStorageFace(ScreenNumber screen, Color base, Color frame, Color cross, Color ring);
    void drawReflectionFace(ScreenNumber screen, Color base, Color frame, Color lens);

    enum class CubeType           { Companion = 0, Storage = 1, Reflection = 2, COUNT = 3 };
    enum class PulseMode          { Static = 0, Portal = 1, Neon = 2, COUNT = 3 };
    enum class StoragePulseMode   { Static = 0, Pulse = 1, COUNT = 2 };
    enum class ReflectionPulseMode{ Static = 0, Pulse = 1, COUNT = 2 };

    Joystick              joystick_               { 0 };
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
