#ifndef RAINBOW_H
#define RAINBOW_H

#include <cube/cube.h>

#include <memory>
#include <vector>

// Ported to libcube (Cube 2.0). Parameters live in schema.toml; the IMU comes
// from cube::Imu; the app is driven by cube::run().
class Rainbow : public cube::CubeApp {
public:
    Rainbow();
    bool loop() override;

private:
    class Particle;
    class Drop;

    cube::Imu imu_;
    cube::Joystick joystick_;
    std::vector<cube::Color> allTheColors_;
    std::vector<cube::Color> allTheColorsRainbow_;
    std::vector<cube::Color> allTheColorsRandom_;
};

class Rainbow::Particle {
public:
    Particle(cube::Vec3f pos, cube::Vec3f vel, cube::Vec3f accel, cube::Color col);
    void step();
    void accelerate();
    void move();

    [[nodiscard]] cube::Vec3f position() const;
    [[nodiscard]] cube::Vec3f velocity() const;
    [[nodiscard]] cube::Vec3f acceleration() const;

    [[nodiscard]] cube::Vec3i iPosition() const;
    [[nodiscard]] cube::Vec3i iVelocity() const;
    [[nodiscard]] cube::Vec3i iAcceleration() const;

    void position(cube::Vec3f pos);
    void velocity(cube::Vec3f vel);
    void acceleration(cube::Vec3f accel);

    [[nodiscard]] cube::Color color() const;
    void color(cube::Color col);

protected:
    cube::Vec3f position_;
    cube::Vec3f velocity_;
    cube::Vec3f acceleration_;
    cube::Color color_;
};

class Rainbow::Drop : public Particle {
public:
    Drop(cube::Vec3i maxPos, cube::Vec3f pos, cube::Vec3f vel, cube::Vec3f accel, cube::Color col);
    void step();
    [[nodiscard]] bool getRdyDelete() const;

private:
    float vxOld_{0.0F};
    float vyOld_{0.0F};
    cube::Vec3i maxPos_;
    bool rdyDelete_{false};
};

#endif  // RAINBOW_H
