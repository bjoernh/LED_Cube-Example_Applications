#include "matrixrain.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <numbers>
#include <vector>

using cube::Color;
using cube::Vec3f;
using cube::Vec3i;

namespace {
// Legacy SN30 Pro joystick index -> cube::Btn, per spec §10 cheat sheet.
//   index 0 -> A (§10), index 3 -> Y (§10).
// MatrixRain only uses A (cycle colour) and Y (pause); the legacy shoulder
// (6/7) handlers nudged a `fade_factor` that was never fed to fade(), so they
// were no-ops and are dropped.
constexpr int kMax = cube::VIRTUAL_CUBE_MAX_INDEX;     // 65
constexpr int kCenter = cube::VIRTUAL_CUBE_CENTER;     // 33
}  // namespace

MatrixRain::MatrixRain() : cube::CubeApp("matrixrain", 40) {}

bool MatrixRain::loop() {
    if (joystick_.justPressed(cube::Btn::A)) {
        ++counterColChange_;
    }
    if (joystick_.justPressed(cube::Btn::Y)) {
        isPaused_ = !isPaused_;
    }
    if (isPaused_) {
        return true;
    }

    fade(params().getFloat("fade"));

    int spawnRate = params().getInt("spawnRate");
    float speedMultiplier = params().getFloat("speed");

    // Audio reactivity (cube::Microphone; volume/bands are 0..255 -> 0..1).
    const float audioVol = static_cast<float>(mic_.volume()) / 255.0F;
    const std::vector<std::uint8_t> audioFreqs = mic_.frequencies();
    const float audioThreshold = params().getFloat("audioThreshold");
    const float audioSpeedMult = params().getFloat("audioSpeedMult");
    const bool audioColorShift = params().getBool("audioColorShift");

    if (audioVol > audioThreshold) {
        const float excite = audioVol - audioThreshold;
        spawnRate += static_cast<int>(excite * 30.0F);
        speedMultiplier += excite * audioSpeedMult;
    }

    for (int i = 0; i < spawnRate; ++i) {
        const float randAngle = static_cast<float>(std::rand() % 360);
        const float vx = speedMultiplier * std::cos(randAngle * std::numbers::pi_v<float> / 180.0F);
        const float vy = speedMultiplier * std::sin(randAngle * std::numbers::pi_v<float> / 180.0F);
        drops_.push_back(std::make_shared<Drop>(Vec3i{kMax, kMax, kMax},
                                                Vec3f{static_cast<float>(kCenter),
                                                      static_cast<float>(kCenter), 0.0F},
                                                Vec3f{vx, vy, 0.0F}, Vec3f{0.0F, 0.0F, 0.0F},
                                                col1_));
    }

    // Colour palette cycles between two greens/cyans (legacy switch was %2).
    if (counterColChange_ % 2 == 0) {
        col1_ = Color{0, 255, 150};
        col1_ *= static_cast<float>(std::rand() % 100) / 100.0F;
    } else {
        col1_ = Color{static_cast<std::uint8_t>(255 - std::rand() % 200), 0,
                      static_cast<std::uint8_t>(255 - std::rand() % 100)};
    }

    // Shift colour by audio energy in the low/high bands.
    if (audioColorShift && !audioFreqs.empty()) {
        const float bass = static_cast<float>(audioFreqs.front()) / 255.0F;
        const float treble =
            static_cast<float>(audioFreqs.size() > 16 ? audioFreqs[16] : audioFreqs.back()) /
            255.0F;
        if (bass > audioThreshold) {
            col1_.r(static_cast<std::uint8_t>(
                std::min(255, col1_.r() + static_cast<int>((bass - audioThreshold) * 255.0F))));
        }
        if (treble > audioThreshold) {
            col1_.b(static_cast<std::uint8_t>(
                std::min(255, col1_.b() + static_cast<int>((treble - audioThreshold) * 255.0F))));
        }
    }

    for (const auto& r : drops_) {
        r->step();
        setPixel3D(r->iPosition(), r->color());
    }
    drops_.erase(std::remove_if(drops_.begin(), drops_.end(),
                                [](const std::shared_ptr<Drop>& r) { return r->getRdyDelete(); }),
                 drops_.end());
    return true;
}

// ── Particle ────────────────────────────────────────────────────────────────
MatrixRain::Particle::Particle(Vec3f pos, Vec3f vel, Vec3f accel, Color col)
    : position_(pos), velocity_(vel), acceleration_(accel), color_(col) {}

void MatrixRain::Particle::step() {
    accelerate();
    move();
}
void MatrixRain::Particle::move() { position_ += velocity_; }
void MatrixRain::Particle::accelerate() { velocity_ += acceleration_; }

Vec3i MatrixRain::Particle::iPosition() const {
    return Vec3i{static_cast<int>(std::lround(position_.x)),
                 static_cast<int>(std::lround(position_.y)),
                 static_cast<int>(std::lround(position_.z))};
}

// ── Drop ────────────────────────────────────────────────────────────────────
MatrixRain::Drop::Drop(Vec3i maxPos, Vec3f pos, Vec3f vel, Vec3f accel, Color col)
    : Particle(pos, vel, accel, col), maxPos_(maxPos) {}

void MatrixRain::Drop::step() {
    Particle::step();

    // Lateral boundary: transition to rising up the side.
    if (position_[0] < 0 || position_[1] < 0 || position_[0] > static_cast<float>(maxPos_[0]) ||
        position_[1] > static_cast<float>(maxPos_[1])) {
        velocity_[2] = 0.2F;
        acceleration_[2] = 0.001F + (static_cast<float>(std::rand() % 10) / 200.0F);
        acceleration_[1] = 0.0F;
        acceleration_[0] = 0.0F;
        if (vxOld_ == 0.0F && vyOld_ == 0.0F) {
            vxOld_ = velocity_[0];
            vyOld_ = velocity_[1];
        }
        velocity_[0] = 0.0F;
        velocity_[1] = 0.0F;
    }

    if (position_[0] < 0) {
        position_[0] = 0.0F;
        position_[2] = 0.0F;
    }
    if (position_[1] < 0) {
        position_[1] = 0.0F;
        position_[2] = 0.0F;
    }
    if (position_[0] > static_cast<float>(maxPos_[0])) {
        position_[0] = static_cast<float>(maxPos_[0]);
        position_[2] = 0.0F;
    }
    if (position_[1] > static_cast<float>(maxPos_[1])) {
        position_[1] = static_cast<float>(maxPos_[1]);
        position_[2] = 0.0F;
    }

    if (position_[2] < 0) {  // ground bounce
        position_[2] = 0.0F;
        velocity_[2] *= -1.0F;
    }

    if (position_[2] > static_cast<float>(maxPos_[2])) {  // ceiling: return to centre
        position_[2] = static_cast<float>(maxPos_[2]);
        velocity_[0] = vxOld_ * -1.0F;
        velocity_[1] = vyOld_ * -1.0F;
        velocity_[2] = 0.0F;
        acceleration_[0] = 0.0F;
        acceleration_[1] = 0.0F;
        acceleration_[2] = 0.0F;
    }

    const float ceil = static_cast<float>(maxPos_[2]);
    const float center = static_cast<float>(kCenter);
    if (((velocity_[0] > 0 && position_[0] > center) ||
         (velocity_[0] < 0 && position_[0] < center)) &&
        position_[2] == ceil) {
        velocity_[0] = 0.0F;
        vxOld_ = 0.0F;
    }
    if (((velocity_[1] > 0 && position_[1] > center) ||
         (velocity_[1] < 0 && position_[1] < center)) &&
        position_[2] == ceil) {
        velocity_[1] = 0.0F;
        vyOld_ = 0.0F;
    }
    if (velocity_[0] == 0.0F && velocity_[1] == 0.0F && position_[2] == ceil) {
        rdyDelete_ = true;
    }
}
