#include "pixelbeat.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

#define PI 3.14159265f

PixelBeat::PixelBeat(std::string serverUri)
    : CubeApplication(40, serverUri, "PixelBeat"),
      hue_(0.0f), prevReset_(false),
      lastParamUpdateCount_(0), paramNotifyFrames_(0) {

    params.registerFloat("speed",        "Particle Speed",      0.1f, 3.0f,  1.0f,  0.01f,  "Motion");
    params.registerFloat("spawnSpeed",   "Spawn Speed",         0.0f, 3.0f,  1.0f,  0.01f,  "Motion");
    params.registerFloat("gravity",      "Gravity",             0.0f, 1.0f,  0.25f, 0.01f,  "Motion");

    params.registerBool ("audioEnabled", "Audio Reactive",      true,                       "Audio");
    params.registerFloat("audioGain",    "Audio Gain",          1.0f, 200.0f, 50.0f, 1.0f,  "Audio");
    params.registerFloat("beatThreshold","Beat Threshold",      1.1f, 2.5f,  1.4f,  0.05f,  "Audio");

    params.registerFloat("intensity",    "Brightness",          0.0f, 1.0f,  0.9f,  0.01f,  "Render");
    params.registerFloat("tailLength",   "Tail Length",         0.5f, 0.99f, 0.9f,  0.005f, "Render");
    params.registerInt  ("particlesPerBeat", "Particles per Beat", 1, 64, 8,                "Render");

    std::vector<std::string> spawnModes = {"randomFace", "topOnly", "randomEdge", "upperEdges"};
    std::vector<std::string> colorModes = {"hueCycle", "audioBands", "random"};
    params.registerEnum("spawnMode", "Spawn Location", spawnModes, "randomFace", "Mode");
    params.registerEnum("colorMode", "Color Scheme",   colorModes, "hueCycle",   "Mode");

    params.registerBool("reset", "Reset Animation", false, "Action");

    audio_.init();
    drops_.reserve(2048);
}

bool PixelBeat::loop() {
    bool resetNow = params.getBool("reset");
    if (resetNow && !prevReset_) {
        drops_.clear();
        params.setBool("reset", false);
        resetNow = false;
    }
    prevReset_ = resetNow;

    audio_.setEnabled(params.getBool("audioEnabled"));
    audio_.setGain(params.getFloat("audioGain"));
    audio_.setBeatThreshold(params.getFloat("beatThreshold"));

    fade(params.getFloat("tailLength"));

    const bool audioOn = params.getBool("audioEnabled") && audio_.isAvailable();
    if (audioOn && audio_.consumeBeat()) {
        handleBeat();
    }

    const float speedMul   = params.getFloat("speed");
    const float gravityMag = params.getFloat("gravity");
    const float intensity  = params.getFloat("intensity");

    for (auto& d : drops_) {
        d->step(speedMul, gravityMag);
        if (!d->dead()) {
            setPixel3D(d->iPos(), d->color(), intensity);
        }
    }
    drops_.erase(std::remove_if(drops_.begin(), drops_.end(),
        [](const std::shared_ptr<Drop>& d){ return d->dead(); }), drops_.end());

    // Fires every time a param update message lands — even if the value is
    // unchanged — so a "Apply" press is always confirmed visually.
    uint64_t cnt = params.getUpdateCounter();
    if (cnt != lastParamUpdateCount_) {
        paramNotifyFrames_ = 20;  // ~0.5 s @ 40 fps
        lastParamUpdateCount_ = cnt;
    }

    if (paramNotifyFrames_ > 0) {
        paramNotifyFrames_--;
        drawText(top, Vector2i(CharacterBitmaps::centered, 22), Color::white(), "params");
        drawText(top, Vector2i(CharacterBitmaps::centered, 32), Color::white(), "updated");
    }

    render();
    return true;
}

void PixelBeat::handleBeat() {
    const std::string spawnMode = params.getString("spawnMode");
    const std::string colorMode = params.getString("colorMode");
    const int n = params.getInt("particlesPerBeat");
    const float spawnSpeed = params.getFloat("spawnSpeed");

    Color beatColor = colorForBeat(colorMode);

    for (int i = 0; i < n; i++) {
        ScreenNumber face;
        Vector3f pos = spawnPointForMode(spawnMode, face);
        Vector3f vel = tangentOutwardVelocity(face, spawnSpeed);
        Color c = (colorMode == "random") ? Color::random() : beatColor;
        drops_.push_back(std::make_shared<Drop>(pos, vel, face, c));
    }
}

ScreenNumber PixelBeat::randomFace() {
    static const ScreenNumber faces[6] = {front, right, back, left, top, bottom};
    return faces[rand() % 6];
}

ScreenNumber PixelBeat::randomSideFace() {
    static const ScreenNumber sides[4] = {front, right, back, left};
    return sides[rand() % 4];
}

Vector3f PixelBeat::spawnPointForMode(const std::string& mode, ScreenNumber& outFace) {
    if (mode == "topOnly") {
        outFace = top;
        Vector3i p = getRandomPointOnScreen(top);
        return Vector3f(p[0], p[1], p[2]);
    }
    // Face-local coords passed to getPointOnScreen are 0..CUBEMAXINDEX (=63).
    if (mode == "upperEdges") {
        outFace = randomSideFace();
        int along = rand() % CUBESIZE;
        Vector2i p2(along, 0);
        Vector3i p = getPointOnScreen(outFace, p2);
        return Vector3f(p[0], p[1], p[2]);
    }
    if (mode == "randomEdge") {
        outFace = randomFace();
        int along = rand() % CUBESIZE;
        int edgePick = rand() % 4;
        Vector2i p2;
        switch (edgePick) {
            case 0: p2 = Vector2i(along, 0); break;
            case 1: p2 = Vector2i(along, CUBEMAXINDEX); break;
            case 2: p2 = Vector2i(0, along); break;
            default:p2 = Vector2i(CUBEMAXINDEX, along); break;
        }
        Vector3i p = getPointOnScreen(outFace, p2);
        return Vector3f(p[0], p[1], p[2]);
    }
    // randomFace (default)
    outFace = randomFace();
    Vector3i p = getRandomPointOnScreen(outFace);
    return Vector3f(p[0], p[1], p[2]);
}

Vector3f PixelBeat::tangentOutwardVelocity(ScreenNumber face, float magnitude) {
    float angle = (rand() % 360) * PI / 180.0f;
    float a = magnitude * std::cos(angle);
    float b = magnitude * std::sin(angle);
    switch (face) {
        case top:
        case bottom:
            return Vector3f(a, b, 0.0f);
        case front:
        case back:
            return Vector3f(a, 0.0f, b);
        case left:
        case right:
            return Vector3f(0.0f, a, b);
        default:
            return Vector3f(a, b, 0.0f);
    }
}

Color PixelBeat::colorForBeat(const std::string& mode) {
    if (mode == "audioBands") {
        Eigen::Vector3f bands = Eigen::Vector3f::Zero();
        // Note: AudioInput::getBands() returns (bass, mid, treble) in [0,1].
        bands = const_cast<AudioInput&>(audio_).getBands();
        float sum = bands.sum();
        if (sum < 1e-4f) return Color::white();
        Color c;
        c.r((uint8_t)std::min(255.0f, bands[0] / sum * 255.0f * 1.5f));
        c.g((uint8_t)std::min(255.0f, bands[1] / sum * 255.0f * 1.5f));
        c.b((uint8_t)std::min(255.0f, bands[2] / sum * 255.0f * 1.5f));
        return c;
    }
    if (mode == "random") {
        // Each particle gets its own random in handleBeat(); this base is unused.
        return Color::random();
    }
    // hueCycle (default)
    hue_ += 47.0f; // ~13% of 360
    if (hue_ >= 360.0f) hue_ -= 360.0f;
    Color c;
    c.fromHSV(hue_, 1.0f, 1.0f);
    return c;
}

// ---------------- Drop ----------------

PixelBeat::Drop::Drop(Vector3f pos, Vector3f vel, ScreenNumber spawnFace, Color col)
    : pos_(pos), vel_(vel), color_(col), spawnFace_(spawnFace),
      falling_(spawnFace != top), age_(0), dead_(false) {}

void PixelBeat::Drop::step(float speedMul, float gravityMag) {
    if (falling_) {
        vel_[2] += gravityMag;
    }

    pos_ += vel_ * speedMul;

    const float lo = 0.0f;
    const float hi = (float)VIRTUALCUBEMAXINDEX;

    bool hitXYBoundary = false;
    if (pos_[0] < lo) { pos_[0] = lo; hitXYBoundary = true; }
    if (pos_[0] > hi) { pos_[0] = hi; hitXYBoundary = true; }
    if (pos_[1] < lo) { pos_[1] = lo; hitXYBoundary = true; }
    if (pos_[1] > hi) { pos_[1] = hi; hitXYBoundary = true; }

    if (hitXYBoundary && !falling_) {
        // Transitioned off the top face onto a side wall — gravity takes over.
        vel_[0] = 0.0f;
        vel_[1] = 0.0f;
        vel_[2] = std::max(vel_[2], 0.3f);
        falling_ = true;
    }

    if (pos_[2] < lo) {
        pos_[2] = lo;
        if (vel_[2] < 0.0f) vel_[2] = 0.0f;
    }
    if (pos_[2] >= hi) {
        pos_[2] = hi;
        vel_[2] = 0.0f;
        // On the bottom face — let any in-plane velocity dissipate, then die.
        vel_[0] *= 0.85f;
        vel_[1] *= 0.85f;
        if (std::abs(vel_[0]) < 0.05f && std::abs(vel_[1]) < 0.05f) {
            dead_ = true;
        }
    }

    age_++;
    if (age_ > 600) dead_ = true; // safety cap (~15s @ 40 fps)
}

Vector3i PixelBeat::Drop::iPos() const {
    return Vector3i((int)std::round(pos_[0]),
                    (int)std::round(pos_[1]),
                    (int)std::round(pos_[2]));
}
