#include "PortalCube.h"
#include <cmath>

PortalCube::PortalCube(std::string serverUri)
    : CubeApplication(30, serverUri, "PortalCube") {}

bool PortalCube::loop() {
    glowPhase_  = fmodf(glowPhase_  + 0.04f, 2.0f * M_PI);
    crossPhase_ = fmodf(crossPhase_ + 0.025f, 2.0f * M_PI);

    if (joystick_.isFound()) {
        float ay = joystick_.getAxis(7);
        float ax = joystick_.getAxis(6);

        bool dpadUp    = (ay <= -0.5f);
        bool dpadDown  = (ay >=  0.5f);
        bool dpadLeft  = (ax <= -0.5f);
        bool dpadRight = (ax >=  0.5f);

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
        Color base, frame, cross, heart;

        if (pulseMode_ == PulseMode::Static) {
            base.fromHSV(20.0f,  0.18f, 0.30f);
            frame.fromHSV(0.0f,  0.00f, 0.95f);
            heart.fromHSV(338.0f, 0.72f, 0.75f);
            cross.fromHSV(338.0f, 0.55f, 0.65f);

        } else if (pulseMode_ == PulseMode::Portal) {
            base.fromHSV(20.0f,  0.18f, 0.30f);
            frame.fromHSV(0.0f,  0.00f, 0.95f);
            float heartV = 0.60f + 0.30f * sinf(glowPhase_);
            float crossV = 0.55f + 0.20f * sinf(crossPhase_ + 0.8f);
            heart.fromHSV(338.0f, 0.72f, heartV);
            cross.fromHSV(338.0f, 0.55f, crossV);

        } else {
            base.fromHSV(20.0f,  0.10f, 0.08f);
            frame.fromHSV(210.0f, 0.15f, 0.98f);
            float pulse  = sinf(glowPhase_);
            float heartV = 0.75f + 0.25f * pulse;
            float heartS = 0.95f - 0.30f * (pulse * 0.5f + 0.5f);
            float crossV = 0.70f + 0.20f * sinf(crossPhase_ + 0.8f);
            float crossS = 0.90f - 0.25f * (sinf(crossPhase_ + 0.8f) * 0.5f + 0.5f);
            heart.fromHSV(338.0f, heartS, heartV);
            cross.fromHSV(338.0f, crossS, crossV);
        }

        fillAll(base);
        for (auto screen : {front, back, left, right, top, bottom})
            drawCompanionFace(screen, base, frame, cross, heart);

    } else if (cubeType_ == CubeType::Storage) {
        Color base, frame, cross, ring;

        base.fromHSV(210.0f, 0.05f, 0.55f);
        frame.fromHSV(0.0f,  0.00f, 0.95f);
        cross.fromHSV(0.0f,  0.00f, 0.90f);

        if (storagePulseMode_ == StoragePulseMode::Pulse) {
            float ringV = 0.70f + 0.30f * sinf(glowPhase_);
            float ringS = 0.85f - 0.25f * (sinf(glowPhase_) * 0.5f + 0.5f);
            ring.fromHSV(210.0f, ringS, ringV);
        } else {
            ring.fromHSV(210.0f, 0.85f, 0.90f);
        }

        fillAll(base);
        for (auto screen : {front, back, left, right, top, bottom})
            drawStorageFace(screen, base, frame, cross, ring);

    } else {
        Color base, frame, lens;

        base.fromHSV(210.0f, 0.05f, 0.50f);
        frame.fromHSV(0.0f,  0.00f, 0.90f);

        if (reflectionPulseMode_ == ReflectionPulseMode::Pulse) {
            float lensV = 0.55f + 0.35f * sinf(glowPhase_);
            lens.fromHSV(145.0f, 0.85f, lensV);
        } else {
            lens.fromHSV(145.0f, 0.85f, 0.70f);
        }

        fillAll(base);
        for (auto screen : {front, back, left, right, top, bottom})
            drawReflectionFace(screen, base, frame, lens);
    }

    render();
    return true;
}

// ─── Companion Cube helpers ───────────────────────────────────────────────────

void PortalCube::drawFilledCircle(ScreenNumber screen, int cx, int cy, int radius, Color color) {
    for (int dy = -radius; dy <= radius; dy++) {
        int dx = (int)sqrtf((float)(radius * radius - dy * dy) - 0.5f);
        drawLine2D(screen, cx - dx, cy + dy, cx + dx, cy + dy, color);
    }
}

void PortalCube::drawHeart(ScreenNumber screen, int cx, int cy, int size, Color color) {
    float fHr = size * 0.5f;
    for (int dy = -size; dy <= size; dy++) {
        int iDrawY = dy - 2;
        bool bInSegment = false;
        int iSegStart = 0;

        for (int dx = -size; dx <= size; dx++) {
            float x   = (float)dx / size;
            float y   = -(float)dy / size;
            float val = powf(x*x + y*y - 1.0f, 3.0f) - x*x * y*y*y;
            bool bInside = (val <= 0.0f);

            if (bInside && !bInSegment) {
                iSegStart  = dx;
                bInSegment = true;
                if (dy <= 0) {
                    int dxL = (int)sqrtf(fHr*fHr - (float)(dy*dy) - 0.5f);
                    drawLine2D(screen, cx - (int)fHr - dxL, cy + iDrawY,
                                       cx - (int)fHr + dxL, cy + iDrawY, color);
                    drawLine2D(screen, cx + (int)fHr - dxL, cy + iDrawY,
                                       cx + (int)fHr + dxL, cy + iDrawY, color);
                    break;
                }
            } else if (!bInside && bInSegment) {
                drawLine2D(screen, cx + iSegStart, cy + iDrawY,
                                   cx + (dx - 1),  cy + iDrawY, color);
                bInSegment = false;
            }
        }
    }
}

void PortalCube::drawCompanionFace(ScreenNumber screen,
                                   Color base, Color frame,
                                   Color cross, Color heart)
{
    const int size   = 63;
    const int center = size / 2;
    const int wEnd   = center - 10;
    const int wBegin = center + 11;
    const int iSpace = 3;
    const int iRows  = 8;

    drawLine2D(screen, center, 0,    center, size,  cross);
    drawLine2D(screen, 0,    center, size,   center, cross);

    for (int i = 0; i < iRows; ++i) {
        drawLine2D(screen, 0,            i,     wEnd,           i,     frame);
        drawLine2D(screen, wEnd+iSpace,  i,     wBegin-iSpace,  i,     frame);
        drawLine2D(screen, wBegin,       i,     size,           i,     frame);

        drawLine2D(screen, size-i, 0,            size-i, wEnd,           frame);
        drawLine2D(screen, size-i, wEnd+iSpace,  size-i, wBegin-iSpace,  frame);
        drawLine2D(screen, size-i, wBegin,       size-i, size,           frame);

        drawLine2D(screen, size,         size-i, wBegin,         size-i, frame);
        drawLine2D(screen, wBegin-iSpace,size-i, wEnd+iSpace,    size-i, frame);
        drawLine2D(screen, wEnd,         size-i, 0,              size-i, frame);

        drawLine2D(screen, i, size,          i, wBegin,          frame);
        drawLine2D(screen, i, wBegin-iSpace, i, wEnd+iSpace,     frame);
        drawLine2D(screen, i, wEnd,          i, 0,               frame);
    }

    for (int i = iRows, dec = 1; i < wEnd; ++i, ++dec) {
        drawLine2D(screen, 0,          i,     wEnd-dec,   i,     frame);
        drawLine2D(screen, wBegin+dec, i,     size,       i,     frame);
        drawLine2D(screen, size,       size-i, wBegin+dec, size-i, frame);
        drawLine2D(screen, wEnd-dec,   size-i, 0,          size-i, frame);
    }

    drawFilledCircle(screen, center, center, 15, frame);
    drawHeart(screen, center, center, 9, heart);
}

void PortalCube::drawStorageFace(ScreenNumber screen,
                                 Color base, Color frame,
                                 Color cross, Color ring)
{
    const int size   = 63;
    const int center = size / 2;
    const int wEnd   = center - 10;
    const int wBegin = center + 11;
    const int iSpace = 3;
    const int iRows  = 8;

    drawLine2D(screen, center, 0,    center, size,  cross);
    drawLine2D(screen, 0,    center, size,   center, cross);

    for (int i = 0; i < iRows; ++i) {
        drawLine2D(screen, 0,            i,     wEnd,           i,     frame);
        drawLine2D(screen, wEnd+iSpace,  i,     wBegin-iSpace,  i,     frame);
        drawLine2D(screen, wBegin,       i,     size,           i,     frame);

        drawLine2D(screen, size-i, 0,            size-i, wEnd,           frame);
        drawLine2D(screen, size-i, wEnd+iSpace,  size-i, wBegin-iSpace,  frame);
        drawLine2D(screen, size-i, wBegin,       size-i, size,           frame);

        drawLine2D(screen, size,         size-i, wBegin,         size-i, frame);
        drawLine2D(screen, wBegin-iSpace,size-i, wEnd+iSpace,    size-i, frame);
        drawLine2D(screen, wEnd,         size-i, 0,              size-i, frame);

        drawLine2D(screen, i, size,          i, wBegin,          frame);
        drawLine2D(screen, i, wBegin-iSpace, i, wEnd+iSpace,     frame);
        drawLine2D(screen, i, wEnd,          i, 0,               frame);
    }

    for (int i = iRows, dec = 1; i < wEnd; ++i, ++dec) {
        drawLine2D(screen, 0,          i,     wEnd-dec,   i,     frame);
        drawLine2D(screen, wBegin+dec, i,     size,       i,     frame);
        drawLine2D(screen, size,       size-i, wBegin+dec, size-i, frame);
        drawLine2D(screen, wEnd-dec,   size-i, 0,          size-i, frame);
    }

    drawFilledCircle(screen, center, center, 15, frame);
    drawFilledCircle(screen, center, center, 12, ring);
    drawFilledCircle(screen, center, center,  7, base);
}

void PortalCube::drawReflectionFace(ScreenNumber screen,
                                    Color base, Color frame, Color lens)
{
    const int size    = 63;
    const int center  = size / 2;
    const int corner  = 12;
    const int ringR   = 27;

    Color dark;
    dark.fromHSV(0.0f, 0.0f, 0.20f);

    for (int y = 0; y <= corner; ++y)
        drawLine2D(screen, 0, y, corner, y, base);
    for (int y = 0; y <= corner; ++y)
        drawLine2D(screen, size-corner, y, size, y, base);
    for (int y = size-corner; y <= size; ++y)
        drawLine2D(screen, 0, y, corner, y, base);
    for (int y = size-corner; y <= size; ++y)
        drawLine2D(screen, size-corner, y, size, y, base);

    drawLine2D(screen, center, 0,          center, center - ringR, dark);
    drawLine2D(screen, center, center+ringR, center, size,         dark);
    drawLine2D(screen, 0,          center, center - ringR, center, dark);
    drawLine2D(screen, center+ringR, center, size,         center, dark);

    drawFilledCircle(screen, center, center, ringR, dark);
    drawFilledCircle(screen, center, center, 23,    lens);
    drawFilledCircle(screen, center, center,  5,    dark);
}
