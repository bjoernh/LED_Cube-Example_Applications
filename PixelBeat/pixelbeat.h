#ifndef PIXELBEAT_H
#define PIXELBEAT_H

#include "CubeApplication.h"
#include "AudioInput.h"
#include <vector>
#include <memory>

class PixelBeat : public CubeApplication {
public:
    PixelBeat(std::string serverUri = DEFAULTSERVERURI);
    bool loop() override;

private:
    class Drop;

    void handleBeat();
    Vector3f spawnPointForMode(const std::string& mode, ScreenNumber& outFace);
    Vector3f tangentOutwardVelocity(ScreenNumber face, float magnitude);
    Color    colorForBeat(const std::string& mode);
    ScreenNumber randomFace();
    ScreenNumber randomSideFace();

    AudioInput audio_;
    std::vector<std::shared_ptr<Drop>> drops_;
    float hue_;
    bool  prevReset_;

    uint64_t lastParamUpdateCount_;
    int      paramNotifyFrames_;
};

class PixelBeat::Drop {
public:
    Drop(Vector3f pos, Vector3f vel, ScreenNumber spawnFace, Color col);
    void step(float speedMul, float gravityMag);
    bool dead() const { return dead_; }
    Vector3i iPos() const;
    Color color() const { return color_; }
private:
    Vector3f pos_;
    Vector3f vel_;
    Color    color_;
    ScreenNumber spawnFace_;
    bool falling_;
    int  age_;
    bool dead_;
};

#endif // PIXELBEAT_H
