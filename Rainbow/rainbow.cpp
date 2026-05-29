#include "rainbow.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <numbers>
#include <vector>

using cube::Color;
using cube::Vec3f;
using cube::Vec3i;

namespace {
constexpr int OVERSAMPLING = 1;

// Fade between two colours by `progress` percent (0..100).
Color ColorFade(Color col1, Color col2, float progress) {
    Color returnColor;
    returnColor.r(static_cast<std::uint8_t>(col1.r() + (col2.r() - col1.r()) * progress / 100));
    returnColor.g(static_cast<std::uint8_t>(col1.g() + (col2.g() - col1.g()) * progress / 100));
    returnColor.b(static_cast<std::uint8_t>(col1.b() + (col2.b() - col1.b()) * progress / 100));
    return returnColor;
}
}  // namespace

Rainbow::Rainbow() : cube::CubeApp("rainbow", 40) {
    allTheColors_.push_back(Color(static_cast<std::uint8_t>(255 - std::rand() % 100), 0, 0));
    allTheColors_.push_back(Color(255, 0, 0));
    allTheColors_.push_back(Color(255, 255, 0));
    allTheColors_.push_back(Color(0, 255, 0));
    allTheColors_.push_back(Color(0, 255, 255));
    allTheColors_.push_back(Color(0, 0, 255));
    allTheColors_.push_back(Color(255, 0, 255));
    allTheColors_.push_back(Color(255, 255, 255));
    allTheColors_.push_back(Color(0, 0, 0));

    allTheColorsRainbow_.push_back(Color(255, 0, 0));
    allTheColorsRainbow_.push_back(Color(255, 255, 0));
    allTheColorsRainbow_.push_back(Color(0, 255, 0));
    allTheColorsRainbow_.push_back(Color(0, 255, 255));
    allTheColorsRainbow_.push_back(Color(0, 0, 255));
    allTheColorsRainbow_.push_back(Color(255, 0, 255));

    allTheColorsRandom_.push_back(Color(static_cast<std::uint8_t>(std::rand() % 127 + 127), 0, 0));
    allTheColorsRandom_.push_back(Color(0, static_cast<std::uint8_t>(std::rand() % 127 + 127), 0));
    allTheColorsRandom_.push_back(Color(0, 0, static_cast<std::uint8_t>(std::rand() % 127 + 127)));
    allTheColorsRandom_.push_back(Color(static_cast<std::uint8_t>(std::rand() % 127 + 127),
                                        static_cast<std::uint8_t>(std::rand() % 127 + 127), 0));
    allTheColorsRandom_.push_back(Color(0, static_cast<std::uint8_t>(std::rand() % 127 + 127),
                                        static_cast<std::uint8_t>(std::rand() % 127 + 127)));
    allTheColorsRandom_.push_back(Color(static_cast<std::uint8_t>(std::rand() % 127 + 127), 0,
                                        static_cast<std::uint8_t>(std::rand() % 127 + 127)));
    allTheColorsRandom_.push_back(Color(static_cast<std::uint8_t>(std::rand() % 127 + 127),
                                        static_cast<std::uint8_t>(std::rand() % 127 + 127),
                                        static_cast<std::uint8_t>(std::rand() % 127 + 127)));
}

bool Rainbow::loop() {
    static std::vector<std::shared_ptr<Drop>> rdrops;
    static int counterColChange = 0;
    static Color col1(255, 0, 0);
    static Color col1RainbowOld = col1;
    static Color col1RainbowNew = col1;
    static bool isPaused = false;
    static int countRainbow = 0;
    static std::uint8_t currentRedValue = 127;
    static std::uint8_t currentGreenValue = 127;
    static std::uint8_t currentBlueValue = 127;
    static int valueColorChangePerLoop = 2;
    static int rainbowSpeedFactorMax = 10;
    static float colorChangeSpeedFactorMax = 2.0f;
    static int colorFadeNumber = 0;
    static int counterStepRainbow = 0;
    static int tempCounterPulseTime = 2000;  // mS
    static int lengthPulseTimeMax = 5000;     // mS

    int colorMode = params().getInt("colorMode");
    int rainbowSpeedFactor = params().getInt("rainbowSpeed");
    float colorChangeSpeedFactor = params().getFloat("colorChangeSpeed");
    int counterPulseTime = params().getInt("pulseInterval");
    int lengthPulseTime = params().getInt("pulseLength");
    static int counterPulseTimeMax = 50000;  // milli Seconds
    static int counterPulse = 0;             // milli Seconds
    static int counterPulse2 = 0;            // milli Seconds
    static int counterBackColorPulse = 0;    // milli Seconds

    // Colormode 4 Pulse 2
    static int counterPulse3 = 0;
    static int counterPulseLong = 1000;       //  in ms
    static int counterPulseLongStart = 0;     //  in ms
    static bool trigerAxsis0 = false;
    auto imuPoint = imu_.acceleration();
    auto imuPointOld = imu_.acceleration();

    // Button X -> Pause
    if (joystick_.justPressed(cube::Btn::Y)) {
        isPaused = !isPaused;
    }
    if (isPaused) {
        return true;
    }

    int colorModeOld = colorMode;
    // Button Y -> Mode
    if (joystick_.justPressed(cube::Btn::B)) {  // VERIFY ON HARDWARE
        colorMode++;
    }

    // Rainbow Color
    counterStepRainbow++;
    if (counterStepRainbow % rainbowSpeedFactor == 0) {
        countRainbow++;
    }

    if (countRainbow >= 100) {
        countRainbow = 0;
        counterStepRainbow = 0;
        colorFadeNumber++;
        col1RainbowOld = col1;
    }
    if (colorFadeNumber >= static_cast<int>(allTheColorsRainbow_.size())) {
        colorFadeNumber = 0;
    }
    col1RainbowNew = allTheColorsRainbow_.at(colorFadeNumber);

    switch (colorMode) {
        // Color Mode
        // Button 0 (A) for Color out of Array
        // Axis 0 (Up Down) for Red Value in-/decrease
        // Axis 1 (Left Right) for Green Value in-/decrease
        // Button 6 (left Shoulder) for Blue Value decrease
        // Button 7 (right Shoulder) for Blue Value increase
        case 0:
            if (joystick_.justPressed(cube::Btn::A)) {
                counterColChange++;
                if (counterColChange >= static_cast<int>(allTheColors_.size())) {
                    counterColChange = 0;
                }
                currentRedValue = allTheColors_.at(counterColChange).r();
                currentGreenValue = allTheColors_.at(counterColChange).g();
                currentBlueValue = allTheColors_.at(counterColChange).b();
            }
            if (joystick_.axis(cube::Axis::LeftX) > 0.5f &&
                currentRedValue < 256 - valueColorChangePerLoop) {
                currentRedValue += valueColorChangePerLoop;
            } else if (joystick_.axis(cube::Axis::LeftX) < -0.5f &&
                       currentRedValue > -1 + valueColorChangePerLoop) {
                currentRedValue -= valueColorChangePerLoop;
            }
            if (joystick_.axis(cube::Axis::LeftY) < -0.5f &&
                currentGreenValue < 256 - valueColorChangePerLoop) {
                currentGreenValue += valueColorChangePerLoop;
            } else if (joystick_.axis(cube::Axis::LeftY) > 0.5f &&
                       currentGreenValue > -1 + valueColorChangePerLoop) {
                currentGreenValue -= valueColorChangePerLoop;
            }
            if (joystick_.isHeld(cube::Btn::ShoulderRight) &&
                currentBlueValue < 256 - valueColorChangePerLoop) {
                currentBlueValue += valueColorChangePerLoop;
            }
            if (joystick_.isHeld(cube::Btn::ShoulderLeft) &&
                currentBlueValue > -1 + valueColorChangePerLoop) {
                currentBlueValue -= valueColorChangePerLoop;
            }

            col1.r(currentRedValue);
            col1.g(currentGreenValue);
            col1.b(currentBlueValue);

            break;
        case 1:
            // Rainbow Mode
            // Button 6 (left Shoulder) for velocity decrease
            // Button 7 (right Shoulder) for velocity increase
            if (joystick_.justPressed(cube::Btn::ShoulderRight) &&
                rainbowSpeedFactor < rainbowSpeedFactorMax) {
                rainbowSpeedFactor++;
            }
            if (joystick_.justPressed(cube::Btn::ShoulderLeft) && rainbowSpeedFactor > 1) {
                rainbowSpeedFactor--;
            }
            if (joystick_.axis(cube::Axis::LeftX) > 0.5f &&
                colorChangeSpeedFactor < colorChangeSpeedFactorMax) {
                colorChangeSpeedFactor += 0.1f;
            } else if (joystick_.axis(cube::Axis::LeftX) < -0.5f && colorChangeSpeedFactor > 0.0f) {
                colorChangeSpeedFactor -= 0.1f;
            }
            if (colorChangeSpeedFactor < 0.1f) {
                colorChangeSpeedFactor = 0.0f;
            }

            col1 = ColorFade(col1RainbowOld, col1RainbowNew, static_cast<float>(countRainbow));
            break;
        case 2:
            // Pulse -> Trial stuff
            if (joystick_.justPressed(cube::Btn::ShoulderRight) &&
                counterPulseTime < counterPulseTimeMax) {
                counterPulseTime += 100;
            }
            if (joystick_.justPressed(cube::Btn::ShoulderLeft) && counterPulseTime > 0) {
                counterPulseTime -= 100;
            }

            if (joystick_.axis(cube::Axis::LeftX) > 0.5f && lengthPulseTime < lengthPulseTimeMax) {
                lengthPulseTime += 100;
            } else if (joystick_.axis(cube::Axis::LeftX) < -0.5f && lengthPulseTime > 0) {
                lengthPulseTime -= 100;
            }

            if (colorChangeSpeedFactor <= 0.1f) {
                counterPulse++;
            }
            tempCounterPulseTime = static_cast<int>(
                counterPulseTime * (static_cast<float>(std::rand() % 70) / 100.0f + 0.7f));
            if (static_cast<float>(counterPulse / getFps()) >=
                static_cast<float>(tempCounterPulseTime)) {
                colorChangeSpeedFactor = 0.4f;
                colorChangeSpeedFactor *= static_cast<float>(std::rand() % 70) / 100.0f + 0.7f;
                counterPulse = 0;
            }
            if (colorChangeSpeedFactor > 0.1f) {
                counterPulse2++;
            }
            if ((counterPulse2 * 1000 / getFps()) >= lengthPulseTime) {
                colorChangeSpeedFactor = 0.0f;
                counterPulse2 = 0;
            }
            col1 = ColorFade(col1RainbowOld, col1RainbowNew, static_cast<float>(countRainbow));
            break;
        case 3:
            if (colorModeOld != colorMode) {
                counterBackColorPulse = 0;
                colorChangeSpeedFactor = 0.4f;
            }

            if (joystick_.justPressed(cube::Btn::ShoulderRight)) {
                counterBackColorPulse++;
            }
            switch (counterBackColorPulse) {
                case 0:
                    col1 = Color::black();
                    break;
                case 1:
                    col1 = ColorFade(col1RainbowOld, col1RainbowNew,
                                     static_cast<float>(countRainbow));
                    break;
                case 2:
                    col1 = Color::white();
                    break;
                case 3:
                    col1 = allTheColorsRandom_.at(std::rand() % allTheColorsRandom_.size());
                    break;
                default:
                    counterBackColorPulse = 0;
                    break;
            }

            if (joystick_.isHeld(cube::Btn::A)) {
                colorChangeSpeedFactor = 0.4f;
                colorChangeSpeedFactor *= static_cast<float>(std::rand() % 100) / 100.0f + 1.0f;
                col1 = allTheColorsRandom_.at(std::rand() % allTheColorsRandom_.size());
            }
            break;
        case 4:
            if (colorModeOld != colorMode) {
                colorChangeSpeedFactor = 1.0f;
                col1 = Color::black();
            }

            if (joystick_.justPressed(cube::Btn::ShoulderRight) && counterPulseLong < 10000) {
                counterPulseLong += 10;
            }
            if (joystick_.justPressed(cube::Btn::ShoulderLeft) && counterPulseLong > 0) {
                counterPulseLong -= 10;
            }

            if (joystick_.axis(cube::Axis::LeftX) > 0.5f && colorChangeSpeedFactor < 10.0f &&
                !trigerAxsis0) {
                colorChangeSpeedFactor += 0.1f;
                trigerAxsis0 = true;
            }
            if (std::abs(joystick_.axis(cube::Axis::LeftX)) < 0.5f && trigerAxsis0) {
                trigerAxsis0 = false;
            }
            if (joystick_.axis(cube::Axis::LeftX) < -0.5f && colorChangeSpeedFactor > 0.0f &&
                !trigerAxsis0) {
                colorChangeSpeedFactor -= 0.1f;
                trigerAxsis0 = true;
            }
            if (std::abs(joystick_.axis(cube::Axis::LeftX)) < 0.5f && trigerAxsis0) {
                trigerAxsis0 = false;
            }
            imuPointOld = imuPoint;
            imuPoint = imu_.acceleration();

            counterPulse3++;
            if (joystick_.justPressed(cube::Btn::A) || imuPointOld != imuPoint) {
                counterPulseLongStart = counterPulse3;
                col1 = allTheColorsRandom_.at(std::rand() % allTheColorsRandom_.size());
            }

            if ((counterPulse3 - counterPulseLongStart) * 1000 / getFps() >= counterPulseLong) {
                col1 = Color::black();
            }
            if (col1 == Color::black()) {
                counterPulse3 = 0;
            }
            break;
        default:
            colorMode = 0;
            break;
    }

    col1 *= static_cast<float>(std::rand() % 70) / 100.0f + 0.7f;

    fade(0.85f);
    // create new Raindrops
    if (colorChangeSpeedFactor > 0.0f) {
        for (int foo = 0; foo < 30; foo++) {
            float randAngle = static_cast<float>(std::rand() % 360);
            float vx = (colorChangeSpeedFactor / OVERSAMPLING) *
                       std::cos(randAngle * std::numbers::pi_v<float> / 180);
            float vy = (colorChangeSpeedFactor / OVERSAMPLING) *
                       std::sin(randAngle * std::numbers::pi_v<float> / 180);
            rdrops.push_back(std::make_shared<Drop>(
                Vec3i{cube::VIRTUAL_CUBE_MAX_INDEX, cube::VIRTUAL_CUBE_MAX_INDEX,
                      cube::VIRTUAL_CUBE_MAX_INDEX},
                Vec3f{static_cast<float>(cube::VIRTUAL_CUBE_CENTER),
                      static_cast<float>(cube::VIRTUAL_CUBE_CENTER), 0.0f},
                Vec3f{vx, vy, 0.0f}, Vec3f{0.0f, 0.0f, 0.0f}, col1));
        }
    }

    for (const auto& r : rdrops) {
        for (int overSamplingCounter = 0; overSamplingCounter < OVERSAMPLING; overSamplingCounter++) {
            r->step();
            setPixel3D(r->iPosition(), r->color());
        }
    }

    // remove drops from the bottom
    rdrops.erase(std::remove_if(rdrops.begin(), rdrops.end(),
                                [](const std::shared_ptr<Drop>& r) { return r->getRdyDelete(); }),
                 rdrops.end());

    return true;
}

Rainbow::Drop::Drop(Vec3i maxPos, Vec3f pos, Vec3f vel, Vec3f accel, Color col)
    : Particle(pos, vel, accel, col), maxPos_(maxPos) {}

void Rainbow::Drop::step() {
    static float oversamplingFactor = 1.0f / OVERSAMPLING;
    Particle::step();  // Do the physics

    if (position_[0] < 0 || position_[1] < 0 || position_[0] > static_cast<float>(maxPos_[0]) ||
        position_[1] > static_cast<float>(maxPos_[1])) {
        velocity_[2] = 0.3f * oversamplingFactor;
        acceleration_[2] =
            (0.02f + (static_cast<float>(std::rand() % 10) / 400.0f)) * oversamplingFactor;
        acceleration_[1] = 0;
        acceleration_[0] = 0;
        if (vxOld_ == 0 && vyOld_ == 0) {
            vxOld_ = velocity_[0];
            vyOld_ = velocity_[1];
        }
        velocity_[0] = 0;
        velocity_[1] = 0;
    }

    if (position_[0] < 0) {
        position_[0] = 0;
        position_[2] = 0;
    }

    if (position_[1] < 0) {
        position_[1] = 0;
        position_[2] = 0;
    }

    if (position_[0] > static_cast<float>(maxPos_[0])) {
        position_[0] = static_cast<float>(maxPos_[0]);
        position_[2] = 0;
    }

    if (position_[1] > static_cast<float>(maxPos_[1])) {
        position_[1] = static_cast<float>(maxPos_[1]);
        position_[2] = 0;
    }

    if (position_[2] < 0) {
        position_[2] = 0;
        velocity_[2] *= -1;
    }

    if (position_[2] > static_cast<float>(maxPos_[2])) {
        position_[2] = static_cast<float>(maxPos_[2]);
        velocity_[0] = vxOld_ * -1;
        velocity_[1] = vyOld_ * -1;
        velocity_[2] = 0;
        acceleration_[1] = 0;
        acceleration_[0] = 0;
        acceleration_[2] = 0;
    }
    if (((velocity_[0] > 0 && position_[0] > cube::VIRTUAL_CUBE_CENTER) ||
         (velocity_[0] < 0 && position_[0] < cube::VIRTUAL_CUBE_CENTER)) &&
        position_[2] == static_cast<float>(maxPos_[2])) {
        velocity_[0] = 0;
        vxOld_ = 0;
    }
    if (((velocity_[1] > 0 && position_[1] > cube::VIRTUAL_CUBE_CENTER) ||
         (velocity_[1] < 0 && position_[1] < cube::VIRTUAL_CUBE_CENTER)) &&
        position_[2] == static_cast<float>(maxPos_[2])) {
        velocity_[1] = 0;
        vyOld_ = 0;
    }
    if (velocity_[0] == 0 && velocity_[1] == 0 && position_[2] == static_cast<float>(maxPos_[2])) {
        rdyDelete_ = true;
    }
}

bool Rainbow::Drop::getRdyDelete() const {
    return rdyDelete_;
}

Rainbow::Particle::Particle(Vec3f pos, Vec3f vel, Vec3f accel, Color col)
    : position_(pos), velocity_(vel), acceleration_(accel), color_(col) {}

void Rainbow::Particle::step() {
    accelerate();
    move();
}

void Rainbow::Particle::move() {
    position_ += velocity_;
}

void Rainbow::Particle::accelerate() {
    velocity_ += acceleration_;
}

Vec3f Rainbow::Particle::position() const {
    return position_;
}

Vec3f Rainbow::Particle::velocity() const {
    return velocity_;
}

Vec3f Rainbow::Particle::acceleration() const {
    return acceleration_;
}

Vec3i Rainbow::Particle::iPosition() const {
    return Vec3i{static_cast<int>(std::lround(position_[0])),
                 static_cast<int>(std::lround(position_[1])),
                 static_cast<int>(std::lround(position_[2]))};
}

Vec3i Rainbow::Particle::iVelocity() const {
    return Vec3i{static_cast<int>(std::lround(velocity_[0])),
                 static_cast<int>(std::lround(velocity_[1])),
                 static_cast<int>(std::lround(position_[2]))};
}

Vec3i Rainbow::Particle::iAcceleration() const {
    return Vec3i{static_cast<int>(std::lround(acceleration_[0])),
                 static_cast<int>(std::lround(acceleration_[1])),
                 static_cast<int>(std::lround(acceleration_[2]))};
}

void Rainbow::Particle::position(Vec3f pos) {
    position_ = pos;
}

void Rainbow::Particle::velocity(Vec3f vel) {
    velocity_ = vel;
}

void Rainbow::Particle::acceleration(Vec3f accel) {
    acceleration_ = accel;
}

Color Rainbow::Particle::color() const {
    return color_;
}

void Rainbow::Particle::color(Color col) {
    color_ = col;
}
