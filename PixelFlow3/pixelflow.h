#ifndef SNAKE_PIXELFLOW_H
#define SNAKE_PIXELFLOW_H

#include <cube/cube.h>
#include <memory>
#include <vector>

class PixelFlow : public cube::CubeApp {
public:
    PixelFlow();
    bool loop() override;
private:
    class Particle;
    class Drop;
    cube::Joystick joystick_;
};

class PixelFlow::Particle {
public:
    Particle(cube::Vec3f pos, cube::Vec3f vel, cube::Vec3f accel, cube::Color col);
    void step();
    void accelerate();
    void move();

    cube::Vec3f position();
    cube::Vec3f velocity();
    cube::Vec3f acceleration();

    cube::Vec3i iPosition();
    cube::Vec3i iVelocity();
    cube::Vec3i iAcceleration();

    void position(cube::Vec3f pos);
    void velocity(cube::Vec3f vel);
    void acceleration(cube::Vec3f accel);

    cube::Color color();
    void color(cube::Color Col);
protected:
    cube::Vec3f position_;
    cube::Vec3f velocity_;
    cube::Vec3f acceleration_;
    cube::Color color_;
};

class PixelFlow::Drop : public Particle {
public:
    Drop(cube::Vec3i maxPos, cube::Vec3f pos, cube::Vec3f vel, cube::Vec3f accel, cube::Color col);
    void step();
    bool getRdyDelete();
private:
    float vxOld_{0.0F};
    float vyOld_{0.0F};
    cube::Vec3i maxPos_;
    bool rdyDelete_{false};
};

#endif //SNAKE_PIXELFLOW_H