#ifndef MATRIX_RAIN_H
#define MATRIX_RAIN_H

#include "CubeApplication.h"
#include "Joystick.h"
#include <vector>

class MatrixRain : public CubeApplication{
public:
    MatrixRain(std::string serverUri, float fade);
    bool loop();
private:
    class Particle;
    class Drop;
    std::vector<Joystick *> joysticks;
    float fade_factor;
};

class MatrixRain::Particle{
public:
    Particle(Vector3f pos, Vector3f vel, Vector3f accel, Color col);
    void step();
    void accelerate();
    void move();

    Vector3f position();
    Vector3f velocity();
    Vector3f acceleration();

    Vector3i iPosition();
    Vector3i iVelocity();
    Vector3i iAcceleration();

    void position(Vector3f pos);
    void velocity(Vector3f vel);
    void acceleration(Vector3f accel);

    Color color();
    void color(Color Col);
protected:
    Vector3f position_;
    Vector3f velocity_;
    Vector3f acceleration_;
    Color color_;
};

class MatrixRain::Drop : public Particle {
public:
    Drop(Vector3i maxPos, Vector3f pos, Vector3f vel, Vector3f accel, Color col);
    void step();
    bool getRdyDelete();
private:
    float vxOld_;
    float vyOld_;
    Vector3i maxPos_;
    bool rdyDelete_;
};


#endif //MATRIX_RAIN_H