#ifndef __BREAKOUTGAME_H__
#define __BREAKOUTGAME_H__

#include <cube/cube.h>
#include <string>
#include <vector>

#define DEFAULTGAMEDURATION 120

class BreakoutGame : public cube::CubeApp {
protected:
    class Player;
    class Ball;
    class Block;

    enum GameState {
        pregame, ingame, postgame
    };
public:
    BreakoutGame();

    void playerLoop();
    void ballLoop();
    void blockLoop();
    bool loop() override;

    bool isBlockAtPoint(cube::Vec3f point);
    void spawnBallForPlayer(int playerId);
    void reset(int gameDuration = DEFAULTGAMEDURATION);

    Player *getLeadingPlayer();

private:
    bool updateHighScoreFromToFile(int score = 0, std::string filename = "/var/lib/cube/apps/breakout3d/highscore.txt");

    std::vector<Player *> players_;
    std::vector<Ball *> balls_;
    std::vector<Block *> blocks_;
    cube::Joystick joystick_;
    int remainingSeconds_;
    GameState gameState_;
    int currentHighScore{0};
    int postgameCounter{0};
};

class BreakoutGame::Player {
public:
    Player(cube::CubeApp *renderCube, int id, int joystickId);

    void render();
    void step();
    void doKIMove();
    void generatePaddle();

    cube::Vec3i centerPosition();
    bool collidesWith(cube::Vec3i pos);
    void blink(cube::Color color);

    int getId();
    cube::Color color();
    bool addToScore(float value);
    int score();

    void setLastBall(Ball *ball);
    Ball *lastBall();

private:
    cube::Vec3i centerPosition_;
    std::vector<cube::Vec3i> paddlePixels_;
    int score_;
    int id_;
    int width_;
    int height_;
    float vel_;
    float pos_;
    int maxPos_;
    int minPos_;
    int blinkCount_;
    cube::Color blinkColor_;
    cube::Color color_;
    cube::CubeApp *ca_;
    cube::Joystick joystick_;
    int joystickId_;
    Ball *lastBall_;
};

class BreakoutGame::Ball {
public:
    Ball(cube::CubeApp *renderCube, cube::Vec3f startPosition, cube::Vec3f startVelocity, float speed);

    void render();
    void reflect(cube::Vec3f reflectionVector);
    void step();
    void accelerate();
    void move();
    void revertStep();
    void die();
    bool isDead();
    void reset();
    void setSpeed(float speed);
    void resetSpeed();
    void setLastPlayer(Player *player);
    Player *lastPlayer();

    cube::Vec3f position();
    cube::Vec3f velocity();
    cube::Vec3f acceleration();
    cube::Vec3i iPosition();

    void position(cube::Vec3f pos);
    void velocity(cube::Vec3f vel);
    void acceleration(cube::Vec3f accel);

    cube::Color color();
    void color(cube::Color Col);

private:
    cube::Vec3f position_;
    cube::Vec3f velocity_;
    cube::Vec3f acceleration_;
    cube::Vec3f defaultPosition_;
    cube::Vec3f defaultVelocity_;
    cube::Color color_;
    bool isDead_;
    int respawnTimer_;
    float speed_;
    float defaultSpeed_;
    cube::EdgeNumber lastEdge_;
    cube::Vec3i lastIPosition_;
    cube::CubeApp *ca_;
    Player *lastPlayer_;
};

class BreakoutGame::Block {
public:
    Block(cube::CubeApp *renderCube, cube::ScreenNumber screenNr, cube::Vec2i topLeftCorner, int size, int score, cube::Color color);

    void render();
    bool collidesWith(cube::Vec3f pos);
    cube::Vec3f centerPosition();
    void die();
    bool isDead();
    int score();
    int size();

private:
    std::vector<cube::Vec3i> blockPixels_;
    cube::Vec2i topLeftCorner_;
    cube::ScreenNumber screenNr_;
    int size_;
    int score_;
    cube::Color color_;
    cube::CubeApp *ca_;
    bool isDead_;
};

#endif
