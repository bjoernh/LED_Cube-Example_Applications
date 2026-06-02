#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <cube/cube.h>

#include <string>
#include <vector>

// Ported to libcube (Cube 2.0). Highscore round-trips through
// /var/lib/cube/apps/snake/highscore.txt; driven by cube::run().
#define DEFAULTHIGHSCOREFILE "/var/lib/cube/apps/snake/highscore.txt"

class Snake : public cube::CubeApp {
public:
    Snake();
    bool loop() override;

private:
    bool updateHighScoreFromToFile(int score = 0, const std::string& filename = DEFAULTHIGHSCOREFILE);

    class Player;
    class Food;

    // Spawn `count` snakes (clamped to the 8 colour slots). Slot 0 reads the
    // shared controller; the rest are AI — Snake fills the no-controller slots
    // with AI, exactly as the legacy game did. When libcube grows multi-pad
    // support, additional slots can bind to their own controllers.
    void spawnPlayers(int count);

    // Startup "HOW MANY PLAYERS?" menu, driven from loop() before gameplay.
    // 2..8 total snakes; AI fills every slot without a controller.
    cube::GameStartMenu startMenu_{2, 8, true};

    std::vector<Player*> players;
    std::vector<Food*> food;
    int currentHighScore{1000};
};

class Snake::Player {
public:
    Player(cube::CubeApp* renderCube, int joysticknumber, cube::Vec3f position,
           cube::Vec3f velocity, cube::Color color, unsigned int length);

    void reset();
    void step();
    void handleJoystick();
    void render();
    void turnLeft();
    void turnRight();
    [[nodiscard]] bool collidesWith(cube::Vec3i point);
    void grow(unsigned int howMuch);
    void speedUp(float factor);
    void die();
    [[nodiscard]] bool getIsDying() const { return isDying; }
    [[nodiscard]] bool getIsDead() const { return isDead; }
    [[nodiscard]] cube::Vec3i iPosition() const;
    [[nodiscard]] int getSnakeLength() const { return static_cast<int>(snakeLength); }
    [[nodiscard]] cube::Color getDefaultColor() const { return defaultColor; }

private:
    void accelerate();
    void move();
    void warp();
    void doKiMove();

    std::vector<cube::Vec3f> tail;
    cube::Vec3f position;
    cube::Vec3f velocity;
    cube::Vec3f acceleration;
    cube::Color color;
    cube::Vec3f defaultPosition;
    cube::Vec3f defaultVelocity;
    cube::Color defaultColor;
    bool isDying{false};
    bool isDead{false};
    int dieCounter{0};
    unsigned int snakeLength;
    unsigned int defaultSnakeLength;
    cube::EdgeNumber lastEdge{cube::EdgeNumber::anyEdge};
    cube::Vec3i lastIPosition;
    int joystickNumber;
    cube::Joystick joystick;
    float lastAxis0{0.0F};
    cube::CubeApp* ca;
};

class Snake::Food {
public:
    Food(cube::CubeApp* renderCube, cube::Vec3i position, cube::Color color = cube::Color::red());
    [[nodiscard]] cube::Vec3i getPosition() const { return position; }
    [[nodiscard]] bool getIsEaten() const { return isEaten; }
    void eat() { isEaten = true; }
    void render();

private:
    bool isEaten{false};
    cube::Vec3i position;
    cube::Color color;
    cube::CubeApp* ca;
};

#endif  // __SNAKE_H__
