#include "pixelflow.h"

#include <cube/cube.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numbers>
#include <utility>

namespace {
constexpr float kPi = std::numbers::pi_v<float>;

cube::Vec3f toF(const cube::Vec3i& v) {
    return {static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z)};
}

cube::Vec3i toI(const cube::Vec3f& v) {
    return {static_cast<int>(std::lround(v.x)),
            static_cast<int>(std::lround(v.y)),
            static_cast<int>(std::lround(v.z))};
}
}  // namespace

PixelFlow::PixelFlow() : cube::CubeApp("pixelflow3", 40) {}

bool PixelFlow::loop() {
    static std::vector<std::shared_ptr<Drop>> rdrops;
    static int counter = 0;
    static int counterColChange = 0;
    static cube::Color col1(0, 255 - std::rand() % 100, 255 - std::rand() % 200);
    static bool isPaused = false;

    if (joystick_.justPressed(cube::Btn::A)) {
        counterColChange++;
    }
    if (joystick_.justPressed(cube::Btn::Y)) {
        isPaused = !isPaused;
    }

    if (isPaused) {
        return true;
    }

    fade(params().getFloat("fade"));

    // Create new Raindrops
    int spawnRate = params().getInt("spawnRate");
    float speedMultiplier = params().getFloat("speed");
    for (int foo = 0; foo < spawnRate; foo++) {
        float randAngle = static_cast<float>(std::rand() % 360);
        float vx = speedMultiplier * std::cos(randAngle * kPi / 180.0F);
        float vy = speedMultiplier * std::sin(randAngle * kPi / 180.0F);
        rdrops.push_back(std::make_shared<Drop>(
            cube::Vec3i{cube::VIRTUAL_CUBE_MAX_INDEX, cube::VIRTUAL_CUBE_MAX_INDEX, cube::VIRTUAL_CUBE_MAX_INDEX},
            cube::Vec3f{static_cast<float>(cube::VIRTUAL_CUBE_CENTER),
                        static_cast<float>(cube::VIRTUAL_CUBE_CENTER), 0.0F},
            cube::Vec3f{vx, vy, 0.0F}, cube::Vec3f{0.0F, 0.0F, 0.0F}, col1));
    }

    switch (counterColChange % 6) {
        case 0:
            col1.r(0);
            col1.g(150);
            col1.b(255);
            col1 *= (static_cast<float>(std::rand() % 100) / 100.0F);
            break;
        case 1:
            col1.g(0);
            col1.b(static_cast<std::uint8_t>(255 - std::rand() % 100));
            col1.r(static_cast<std::uint8_t>(255 - std::rand() % 200));
            break;
        case 2:
            col1.b(0);
            col1.r(static_cast<std::uint8_t>(255 - std::rand() % 100));
            col1.g(static_cast<std::uint8_t>(255 - std::rand() % 200));
            break;
        case 3:
            col1.r(0);
            col1.g(0);
            col1.b(static_cast<std::uint8_t>(255 - std::rand() % 200));
            break;
        case 4:
            col1.g(0);
            col1.b(0);
            col1.r(static_cast<std::uint8_t>(255 - std::rand() % 200));
            break;
        case 5:
            col1.b(0);
            col1.r(0);
            col1.g(static_cast<std::uint8_t>(255 - std::rand() % 200));
            break;
    }

    for (const auto& r : rdrops) {
        if (counter % 1 == 0) {
            r->step();
        }
        setPixel3D(r->iPosition(), r->color());
    }

    // Remove drops that are ready
    rdrops.erase(std::remove_if(rdrops.begin(), rdrops.end(),
                                [](const std::shared_ptr<Drop>& r) { return r->getRdyDelete(); }),
                 rdrops.end());

    counter++;
    return true;
}

PixelFlow::Drop::Drop(cube::Vec3i maxPos, cube::Vec3f pos, cube::Vec3f vel, cube::Vec3f accel, cube::Color col)
    : Particle(pos, vel, accel, col), maxPos_(maxPos) {}

void PixelFlow::Drop::step() {
    Particle::step(); // Do the physics

    if (position_.x < 0.0F || position_.y < 0.0F ||
        position_.x > static_cast<float>(maxPos_.x) || position_.y > static_cast<float>(maxPos_.y)) {
        velocity_.z = 0.3F;
        acceleration_.z = 0.02F + (static_cast<float>(std::rand() % 10) / 400.0F);
        acceleration_.y = 0.0F;
        acceleration_.x = 0.0F;
        if (vxOld_ == 0.0F && vyOld_ == 0.0F) {
            vxOld_ = velocity_.x;
            vyOld_ = velocity_.y;
        }
        velocity_.x = 0.0F;
        velocity_.y = 0.0F;
    }

    if (position_.x < 0.0F) {
        position_.x = 0.0F;
        position_.z = 0.0F;
    }
    if (position_.y < 0.0F) {
        position_.y = 0.0F;
        position_.z = 0.0F;
    }
    if (position_.x > static_cast<float>(maxPos_.x)) {
        position_.x = static_cast<float>(maxPos_.x);
        position_.z = 0.0F;
    }
    if (position_.y > static_cast<float>(maxPos_.y)) {
        position_.y = static_cast<float>(maxPos_.y);
        position_.z = 0.0F;
    }
    if (position_.z < 0.0F) {
        position_.z = 0.0F;
        velocity_.z *= -1.0F;
    }
    if (position_.z > static_cast<float>(maxPos_.z)) {
        position_.z = static_cast<float>(maxPos_.z);
        velocity_.x = vxOld_ * -1.0F;
        velocity_.y = vyOld_ * -1.0F;
        velocity_.z = 0.0F;
        acceleration_.y = 0.0F;
        acceleration_.x = 0.0F;
        acceleration_.z = 0.0F;
    }

    if (((velocity_.x > 0.0F && position_.x > static_cast<float>(cube::VIRTUAL_CUBE_CENTER)) ||
         (velocity_.x < 0.0F && position_.x < static_cast<float>(cube::VIRTUAL_CUBE_CENTER))) &&
        position_.z == static_cast<float>(maxPos_.z)) {
        velocity_.x = 0.0F;
        vxOld_ = 0.0F;
    }
    if (((velocity_.y > 0.0F && position_.y > static_cast<float>(cube::VIRTUAL_CUBE_CENTER)) ||
         (velocity_.y < 0.0F && position_.y < static_cast<float>(cube::VIRTUAL_CUBE_CENTER))) &&
        position_.z == static_cast<float>(maxPos_.z)) {
        velocity_.y = 0.0F;
        vyOld_ = 0.0F;
    }
    if (velocity_.x == 0.0F && velocity_.y == 0.0F && position_.z == static_cast<float>(maxPos_.z)) {
        rdyDelete_ = true;
    }
}

bool PixelFlow::Drop::getRdyDelete() {
    return rdyDelete_;
}

PixelFlow::Particle::Particle(cube::Vec3f pos, cube::Vec3f vel, cube::Vec3f accel, cube::Color col)
    : position_(pos), velocity_(vel), acceleration_(accel), color_(col) {}

void PixelFlow::Particle::step() {
    accelerate();
    move();
}

void PixelFlow::Particle::move() {
    position_ += velocity_;
}

void PixelFlow::Particle::accelerate() {
    velocity_ += acceleration_;
}

cube::Vec3f PixelFlow::Particle::position() {
    return position_;
}

cube::Vec3f PixelFlow::Particle::velocity() {
    return velocity_;
}

cube::Vec3f PixelFlow::Particle::acceleration() {
    return acceleration_;
}

cube::Vec3i PixelFlow::Particle::iPosition() {
    return {static_cast<int>(std::round(position_.x)),
            static_cast<int>(std::round(position_.y)),
            static_cast<int>(std::round(position_.z))};
}

cube::Vec3i PixelFlow::Particle::iVelocity() {
    return {static_cast<int>(std::round(velocity_.x)),
            static_cast<int>(std::round(velocity_.y)),
            static_cast<int>(std::round(velocity_.z))};
}

cube::Vec3i PixelFlow::Particle::iAcceleration() {
    return {static_cast<int>(std::round(acceleration_.x)),
            static_cast<int>(std::round(acceleration_.y)),
            static_cast<int>(std::round(acceleration_.z))};
}

void PixelFlow::Particle::position(cube::Vec3f pos) {
    position_ = pos;
}

void PixelFlow::Particle::velocity(cube::Vec3f vel) {
    velocity_ = vel;
}

void PixelFlow::Particle::acceleration(cube::Vec3f accel) {
    acceleration_ = accel;
}

cube::Color PixelFlow::Particle::color() {
    return color_;
}

void PixelFlow::Particle::color(cube::Color Col) {
    color_ = Col;
}
