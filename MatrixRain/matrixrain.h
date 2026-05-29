#ifndef MATRIX_RAIN_H
#define MATRIX_RAIN_H

#include <cube/cube.h>

#include <memory>
#include <vector>

// Ported to libcube (Cube 2.0). Parameters live in schema.toml; audio comes
// from cube::Microphone; the app is driven by cube::run().
class MatrixRain : public cube::CubeApp {
public:
    MatrixRain();
    bool loop() override;

private:
    class Particle;
    class Drop;

    cube::Joystick joystick_;
    cube::Microphone mic_;
    std::vector<std::shared_ptr<Drop>> drops_;
    cube::Color col1_{0, 200, 150};
    int counterColChange_{0};
    bool isPaused_{false};
};

class MatrixRain::Particle {
public:
    Particle(cube::Vec3f pos, cube::Vec3f vel, cube::Vec3f accel, cube::Color col);
    void step();

    [[nodiscard]] cube::Vec3i iPosition() const;
    [[nodiscard]] cube::Color color() const { return color_; }

protected:
    void accelerate();
    void move();

    cube::Vec3f position_;
    cube::Vec3f velocity_;
    cube::Vec3f acceleration_;
    cube::Color color_;
};

class MatrixRain::Drop : public Particle {
public:
    Drop(cube::Vec3i maxPos, cube::Vec3f pos, cube::Vec3f vel, cube::Vec3f accel, cube::Color col);
    void step();
    [[nodiscard]] bool getRdyDelete() const { return rdyDelete_; }

private:
    float vxOld_{0.0F};
    float vyOld_{0.0F};
    cube::Vec3i maxPos_;
    bool rdyDelete_{false};
};

#endif  // MATRIX_RAIN_H
