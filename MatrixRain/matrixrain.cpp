#include "matrixrain.h"
#include <cmath>

#include <iostream>
#include <algorithm>
#include <cctype>
#include <memory>

#define PI 3.14159265

MatrixRain::MatrixRain(std::string serverUri, float fade) : CubeApplication(40, serverUri) {
    joysticks.push_back(new Joystick(0));
    joysticks.push_back(new Joystick(1));
    joysticks.push_back(new Joystick(2));
    joysticks.push_back(new Joystick(3));
    fade_factor = fade;
}

bool MatrixRain::loop(){
    static std::vector<std::shared_ptr<Drop>> rdrops;
    static int counter = 0;
    static int counterColChange = 0;
    static Color col1(0,255-rand()%100,255-rand()%200);
    static bool isPaused = false;


    for (auto joystick : joysticks) {
        if (joystick->getButtonPress(0)) {
            counterColChange++;
        }
        if (joystick->getButtonPress(3)) {
            isPaused = !isPaused;
        }
        /* decrease particle fade on left shoulder button press */
        if (joystick->getButtonPress(6)) {
            if (fade_factor >= 0.05)
                fade_factor -= 0.2;
        }
        /* increase particle fade on right shoulder button press */
        if (joystick->getButtonPress(7)) {
            if (fade_factor <= 1.0)
                fade_factor += 0.2;
        }

        joystick->clearAllButtonPresses();
    }

    if(isPaused)
        return true;


    fade(fade_factor);
    //create new Raindrops
    for (int foo = 0; foo < 4; foo++){
        float randAngle = rand()%360;
        float vx = 0.5 * cos(randAngle*PI/180);
        float vy = 0.5 * sin(randAngle*PI/180);
        rdrops.push_back(std::make_shared<Drop>(Vector3i(VIRTUALCUBEMAXINDEX,VIRTUALCUBEMAXINDEX,VIRTUALCUBEMAXINDEX), Vector3f(VIRTUALCUBECENTER,VIRTUALCUBECENTER,0), Vector3f(vx,vy,0), Vector3f(0,0,0),col1));
    }

    switch (counterColChange%2) {
        case 0:
            col1.r((uint8_t)0);
            col1.g((uint8_t)255);
            col1.b((uint8_t)150);
            col1 *= (float)((rand()%100))/100.0f;
            break;
        case 1:
            col1.g((uint8_t)(0));
            col1.b((uint8_t)(255-rand()%100));
            col1.r((uint8_t)(255-rand()%200));
            break;
        case 2:
            col1.b((uint8_t)(0));
            col1.r((uint8_t)(255-rand()%100));
            col1.g((uint8_t)(255-rand()%200));
            break;
        case 3:
            col1.r((uint8_t)(0));
            col1.g((uint8_t)(0));
            col1.b((uint8_t)(255-rand()%200));
            break;
        case 4:
            col1.g((uint8_t)(0));
            col1.b((uint8_t)(0));
            col1.r((uint8_t)(255-rand()%200));
            break;
        case 5:
            col1.b((uint8_t)(0));
            col1.r((uint8_t)(0));
            col1.g((uint8_t)(255-rand()%200));
            break;
    }

    for(auto r : rdrops) {
        if (counter%1 == 0) {
            r->step();
        }
        //setPixelSmooth3D(r->position(), r->color());
        setPixel3D(r->iPosition(), r->color());
    }

    //remove drops from the bottom
    rdrops.erase(std::remove_if(rdrops.begin(),rdrops.end(),[](std::shared_ptr<Drop> r){return (r->getRdyDelete());}),rdrops.end());

    render();
    counter++;

    return true;
}


MatrixRain::Drop::Drop(Vector3i maxPos, Vector3f pos, Vector3f vel, Vector3f accel, Color col)
        : Particle(pos, vel, accel, col){
    maxPos_ = maxPos;
    vxOld_ = 0.0f;
    vyOld_ = 0.0f;
    rdyDelete_ = false;
}

void MatrixRain::Drop::step(){
    Particle::step(); //Do the physics

    if(position_[0] < 0 || position_[1] < 0 || position_[0] > maxPos_[0] || position_[1] > maxPos_[1]) {
        velocity_[2] = 0.2;
        acceleration_[2] = 0.001+((float)(rand()%10)/200.0f);
        acceleration_[1] = 0;
        acceleration_[0] = 0;
        if (vxOld_ == 0 && vyOld_ == 0){
            vxOld_ = velocity_[0];
            vyOld_ = velocity_[1];
        }
        velocity_[0] = 0;
        velocity_[1] = 0;
    }

    if(position_[0] < 0) {
        position_[0] = 0;
        position_[2] = 0;
    }

    if(position_[1] < 0){
        position_[1] = 0;
        position_[2] = 0;
    }

    if(position_[0] > maxPos_[0]){
        position_[0] = maxPos_[0];
        position_[2] = 0;
    }

    if(position_[1] > maxPos_[1]){
        position_[1] = maxPos_[1];
        position_[2] = 0;
    }

    if(position_[2] < 0){
        position_[2] = 0;
        velocity_[2] *= -1;
    }

    if(position_[2] > maxPos_[2]){
        position_[2] = maxPos_[2];
        velocity_[0] = vxOld_ * -1;
        velocity_[1] = vyOld_ * -1;
        velocity_[2] = 0;
        acceleration_[1] = 0;
        acceleration_[0] = 0;
        acceleration_[2] = 0;
    }
    if(((velocity_[0] > 0 && position_[0] >VIRTUALCUBECENTER) || (velocity_[0]<0 && position_[0] < VIRTUALCUBECENTER)) && position_[2] == maxPos_[2]){
        velocity_[0] = 0;
        vxOld_ = 0;
    }
    if(((velocity_[1] > 0 && position_[1] >VIRTUALCUBECENTER) || (velocity_[1]<0 && position_[1] < VIRTUALCUBECENTER)) && position_[2] == maxPos_[2]){
        velocity_[1] = 0;
        vyOld_ = 0;
    }
    if (velocity_[0] == 0 && velocity_[1] == 0  && position_[2] == maxPos_[2]){
        rdyDelete_ = true;
    }

}

bool MatrixRain::Drop::getRdyDelete(){
    return rdyDelete_;
}


MatrixRain::Particle::Particle(Vector3f pos, Vector3f vel, Vector3f accel, Color col)
        :position_(pos),
         velocity_(vel),
         acceleration_(accel),
         color_(col){}

void MatrixRain::Particle::step(){
    accelerate();
    move();
}

void MatrixRain::Particle::move(){
    position_ += velocity_;
}

void MatrixRain::Particle::accelerate(){
    velocity_ += acceleration_;
}

Vector3f MatrixRain::Particle::position(){
    return position_;
}

Vector3f MatrixRain::Particle::velocity(){
    return velocity_;
}

Vector3f MatrixRain::Particle::acceleration(){
    return acceleration_;
}

Vector3i MatrixRain::Particle::iPosition(){
    return Vector3i(round(position()[0]),round(position()[1]),round(position()[2]));
}

Vector3i MatrixRain::Particle::iVelocity(){
    return Vector3i(round(velocity()[0]),round(velocity()[1]),round(position()[2]));
}

Vector3i MatrixRain::Particle::iAcceleration(){
    return Vector3i(round(acceleration()[0]),round(acceleration()[1]),round(acceleration()[2]));
}

void MatrixRain::Particle::position(Vector3f pos){
    position_ = pos;
}
void MatrixRain::Particle::velocity(Vector3f vel){
    velocity_ = vel;
}
void MatrixRain::Particle::acceleration(Vector3f accel){
    acceleration_ = accel;
}

Color MatrixRain::Particle::color(){
    return color_;
}
void MatrixRain::Particle::color(Color Col){
    color_ = Col;
}

