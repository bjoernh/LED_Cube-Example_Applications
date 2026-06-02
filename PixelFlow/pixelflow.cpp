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

template <typename T>
constexpr const T& constrain(const T& v, const T& lo, const T& hi) {
    return std::clamp(v, lo, hi);
}

cube::EdgeNumber getEdgeNumberThis(cube::Vec3i point) {
    cube::EdgeNumber result = cube::EdgeNumber::anyEdge;
    if (point.x == cube::VIRTUAL_CUBE_MAX_INDEX && point.y == 0) {
        result = cube::EdgeNumber::frontRight;
    } else if (point.x == cube::VIRTUAL_CUBE_MAX_INDEX && point.y == cube::VIRTUAL_CUBE_MAX_INDEX) {
        result = cube::EdgeNumber::rightBack;
    } else if (point.x == 0 && point.y == cube::VIRTUAL_CUBE_MAX_INDEX) {
        result = cube::EdgeNumber::backLeft;
    } else if (point.x == 0 && point.y == 0) {
        result = cube::EdgeNumber::leftFront;
    } else if (point.y == 0 && point.z == 0) {
        result = cube::EdgeNumber::topFront;
    } else if (point.x == cube::VIRTUAL_CUBE_MAX_INDEX && point.z == 0) {
        result = cube::EdgeNumber::topRight;
    } else if (point.y == cube::VIRTUAL_CUBE_MAX_INDEX && point.z == 0) {
        result = cube::EdgeNumber::topBack;
    } else if (point.x == 0 && point.z == 0) {
        result = cube::EdgeNumber::topLeft;
    } else if (point.y == 0 && point.z == cube::VIRTUAL_CUBE_MAX_INDEX) {
        result = cube::EdgeNumber::bottomFront;
    } else if (point.x == cube::VIRTUAL_CUBE_MAX_INDEX && point.z == cube::VIRTUAL_CUBE_MAX_INDEX) {
        result = cube::EdgeNumber::bottomRight;
    } else if (point.y == cube::VIRTUAL_CUBE_MAX_INDEX && point.z == cube::VIRTUAL_CUBE_MAX_INDEX) {
        result = cube::EdgeNumber::bottomBack;
    } else if (point.x == 0 && point.z == cube::VIRTUAL_CUBE_MAX_INDEX) {
        result = cube::EdgeNumber::bottomLeft;
    }
    return result;
}

cube::ScreenNumber getScreenNumberThis(cube::Vec3i point) {
    cube::ScreenNumber result = cube::ScreenNumber::anyScreen;
    if (point.x == 0) {
        result = cube::ScreenNumber::left;
    } else if (point.x == cube::VIRTUAL_CUBE_MAX_INDEX) {
        result = cube::ScreenNumber::right;
    } else if (point.y == 0) {
        result = cube::ScreenNumber::front;
    } else if (point.y == cube::VIRTUAL_CUBE_MAX_INDEX) {
        result = cube::ScreenNumber::back;
    } else if (point.z == 0) {
        result = cube::ScreenNumber::top;
    } else if (point.z == cube::VIRTUAL_CUBE_MAX_INDEX) {
        result = cube::ScreenNumber::bottom;
    }
    return result;
}
}  // namespace

PixelFlow::PixelFlow() : cube::CubeApp("pixelflow", 40) {}

bool PixelFlow::loop() {
    static std::vector<std::shared_ptr<Drop>> rdrops;
    static int counter = 0;
    static int counterColChange = 0;
    static cube::Color col1(0, 255 - std::rand() % 100, 255 - std::rand() % 200);

    fade(0.85F);

    // Snapshot the IMU once per frame. In libcube each Imu accessor performs a
    // synchronous sysfs read, so calling them inside the spawn loop (60x) and
    // the per-drop update loop (thousands of times) issued tens of thousands of
    // blocking reads per frame — the cause of the post-port slowdown. The IMU
    // value is constant within a frame, so one snapshot is behaviour-identical.
    const cube::Vec3i imuPoint = imu_.cubeAccIntersect();
    const cube::Vec3f imuAccel = imu_.acceleration();

    // Create new Raindrops
    for (int foo = 0; foo < 60; foo++) {
        float randAngle = static_cast<float>(std::rand() % 360);
        float speed = 0.0F;
        float vx = speed * std::cos(randAngle * kPi / 180.0F);
        float vy = speed * std::sin(randAngle * kPi / 180.0F);
        cube::Vec3f startSpeed(0.0F, 0.0F, 0.0F);

        switch (cube::CubeApp::getScreenNumber(imuPoint)) {
            case cube::ScreenNumber::top:
            case cube::ScreenNumber::bottom:
                startSpeed.x = vx;
                startSpeed.y = vy;
                break;
            case cube::ScreenNumber::front:
            case cube::ScreenNumber::back:
                startSpeed.x = vx;
                startSpeed.z = vy;
                break;
            case cube::ScreenNumber::left:
            case cube::ScreenNumber::right:
                startSpeed.y = vx;
                startSpeed.z = vy;
                break;
            case cube::ScreenNumber::anyScreen:
            default:
                break;
        }
        cube::Vec3f startPoint = toF(imuPoint);
        rdrops.push_back(std::make_shared<Drop>(
            cube::Vec3i{cube::VIRTUAL_CUBE_MAX_INDEX, cube::VIRTUAL_CUBE_MAX_INDEX, cube::VIRTUAL_CUBE_MAX_INDEX},
            startPoint, startSpeed, cube::Vec3f{0.0F, 0.0F, 0.0F}, col1));
    }

    if (counter % 50 == 0) {
        counterColChange++;
    }
    switch (counterColChange % 6) {
        case 0:
            col1.r(0);
            col1.g(static_cast<std::uint8_t>(255 - std::rand() % 100));
            col1.b(static_cast<std::uint8_t>(255 - std::rand() % 200));
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
            r->acceleration(imuAccel * (-0.1F + (static_cast<float>(std::rand() % 100) / 2000.0F)));
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
    : SurfaceParticle(maxPos, pos, vel, accel, col), maxPos_(maxPos) {}

void PixelFlow::Drop::step() {
    SurfaceParticle::step(); // Do the physics
    if (stepCount > 260) {
        rdyDelete_ = true;
    }
    stepCount++;
}

bool PixelFlow::Drop::getRdyDelete() {
    return rdyDelete_;
}

PixelFlow::SurfaceParticle::SurfaceParticle(cube::Vec3i maxPos, cube::Vec3f pos, cube::Vec3f vel, cube::Vec3f accel, cube::Color col)
    : Particle(pos, vel, accel, col), maxPosition(maxPos) {}

void PixelFlow::SurfaceParticle::step() {
    accelerateOnSurface();
    move();
    warp();
}

void PixelFlow::SurfaceParticle::accelerateOnSurface() {
    switch (getScreenNumberThis(iPosition())) {
        case cube::ScreenNumber::top:
        case cube::ScreenNumber::bottom:
            velocity_.x += acceleration_.x;
            velocity_.y += acceleration_.y;
            break;
        case cube::ScreenNumber::front:
        case cube::ScreenNumber::back:
            velocity_.x += acceleration_.x;
            velocity_.z += acceleration_.z;
            break;
        case cube::ScreenNumber::left:
        case cube::ScreenNumber::right:
            velocity_.y += acceleration_.y;
            velocity_.z += acceleration_.z;
            break;
        case cube::ScreenNumber::anyScreen:
        default:
            break;
    }
}

void PixelFlow::SurfaceParticle::warp() {
    // Constrain position values
    position_.x = constrain(position_.x, 0.0F, static_cast<float>(cube::VIRTUAL_CUBE_MAX_INDEX));
    position_.y = constrain(position_.y, 0.0F, static_cast<float>(cube::VIRTUAL_CUBE_MAX_INDEX));
    position_.z = constrain(position_.z, 0.0F, static_cast<float>(cube::VIRTUAL_CUBE_MAX_INDEX));

    cube::Vec3i currentPosition = iPosition();
    cube::EdgeNumber currentEdge = getEdgeNumberThis(currentPosition);

    if (currentEdge != cube::EdgeNumber::anyEdge) {
        if (currentEdge != lastEdge) {
            switch (currentEdge) {
                case cube::EdgeNumber::topLeft:
                case cube::EdgeNumber::topRight:
                case cube::EdgeNumber::bottomRight:
                case cube::EdgeNumber::bottomLeft:
                    std::swap(velocity_.z, velocity_.x);
                    break;
                case cube::EdgeNumber::topFront:
                case cube::EdgeNumber::topBack:
                case cube::EdgeNumber::bottomBack:
                case cube::EdgeNumber::bottomFront:
                    std::swap(velocity_.z, velocity_.y);
                    break;
                case cube::EdgeNumber::frontRight:
                case cube::EdgeNumber::backLeft:
                case cube::EdgeNumber::leftFront:
                case cube::EdgeNumber::rightBack:
                    std::swap(velocity_.x, velocity_.y);
                    break;
                case cube::EdgeNumber::anyEdge:
                default:
                    break;
            }
            // Set position to rounded position
            position_ = toF(currentPosition);

            // Constrain velocity directions, reflect if necessary
            if ((currentPosition.x == 0 && velocity_.x < 0.0F) ||
                (currentPosition.x == cube::VIRTUAL_CUBE_MAX_INDEX && velocity_.x > 0.0F)) {
                velocity_.x *= -1.0F;
            }
            if ((currentPosition.y == 0 && velocity_.y < 0.0F) ||
                (currentPosition.y == cube::VIRTUAL_CUBE_MAX_INDEX && velocity_.y > 0.0F)) {
                velocity_.y *= -1.0F;
            }
            if ((currentPosition.z == 0 && velocity_.z < 0.0F) ||
                (currentPosition.z == cube::VIRTUAL_CUBE_MAX_INDEX && velocity_.z > 0.0F)) {
                velocity_.z *= -1.0F;
            }
        }
    }
    lastIPosition = currentPosition;
    lastEdge = currentEdge;
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
