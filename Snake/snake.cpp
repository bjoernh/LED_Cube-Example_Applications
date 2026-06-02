#include "snake.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using cube::Color;
using cube::EdgeNumber;
using cube::ScreenNumber;
using cube::Vec3f;
using cube::Vec3i;

namespace {
// §10 cheat sheet: getAxis(0) -> Axis::LeftX. Snake uses only the left-stick X
// to steer; players 1..7 are AI (no second controller — multi-controller is
// out of scope for libcube v1).
constexpr int kMax = cube::VIRTUAL_CUBE_MAX_INDEX;  // 65

[[nodiscard]] Vec3f toF(Vec3i v) {
    return Vec3f{static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z)};
}
[[nodiscard]] Vec3i toI(Vec3f v) {
    return Vec3i{static_cast<int>(std::lround(v.x)), static_cast<int>(std::lround(v.y)),
                 static_cast<int>(std::lround(v.z))};
}
[[nodiscard]] float norm(Vec3f v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
}  // namespace

Snake::Snake() : cube::CubeApp("snake", 40) {
    // Players are spawned once the start menu confirms a count (see loop());
    // only the food and high score are set up here.
    for (int i = 0; i < 20; i++) {
        food.push_back(new Food(this, getRandomPointOnScreen(ScreenNumber::front), Color::randomBlue() * 2.0F));
        food.push_back(new Food(this, getRandomPointOnScreen(ScreenNumber::right), Color::randomBlue() * 2.0F));
        food.push_back(new Food(this, getRandomPointOnScreen(ScreenNumber::back), Color::randomBlue() * 2.0F));
        food.push_back(new Food(this, getRandomPointOnScreen(ScreenNumber::left), Color::randomBlue() * 2.0F));
        food.push_back(new Food(this, getRandomPointOnScreen(ScreenNumber::top), Color::randomBlue() * 2.0F));
        food.push_back(new Food(this, getRandomPointOnScreen(ScreenNumber::bottom), Color::randomBlue() * 2.0F));
    }
    updateHighScoreFromToFile();
}

void Snake::spawnPlayers(int count) {
    // The eight distinct snake colours; slot 0 is the human (shared controller).
    static const std::array<Color, 8> kColors = {
        Color::green(),
        Color::green() + Color::red(),
        Color::blue() + Color::red(),
        Color::red(),
        Color::blue() * 0.5F,
        Color::blue() + Color::red() * 0.3F,
        Color::green() * 0.4F + Color::blue() * 0.2F,
        Color::white() * 0.6F,
    };
    const float startSpeed = 0.2F;
    const int n = std::clamp(count, 1, static_cast<int>(kColors.size()));
    for (int i = 0; i < n; i++) {
        players.push_back(new Player(this, i, toF(getRandomPointOnScreen(ScreenNumber::top)),
                                     Vec3f{0.0F, startSpeed, 0.0F}, kColors[static_cast<std::size_t>(i)], 10));
    }
}

bool Snake::loop() {
    // Startup menu runs first, as a loop() mode: tick it until the user picks a
    // player count (then spawn the snakes) or backs out with B (then quit).
    if (startMenu_.active()) {
        startMenu_.tick(*this);
        if (!startMenu_.active()) {
            if (startMenu_.cancelled()) {
                return false;
            }
            spawnPlayers(startMenu_.playerCount());
        }
        return true;
    }

    static bool highScoreTime = false;
    static int highScoreTimer = 120;
    static Color highScoreColor = Color::white();

    clear();

    if (highScoreTime) {
        const Color fontColor = (highScoreTimer / 5 % 2 == 0) ? Color::black() : highScoreColor;
        const std::string text = std::format("HIGHSCORE {}", currentHighScore);
        for (const auto face : {ScreenNumber::top, ScreenNumber::left, ScreenNumber::front,
                                ScreenNumber::right, ScreenNumber::back, ScreenNumber::bottom}) {
            drawText(face, {cube::Font::centered, cube::Font::centered}, fontColor, text);
        }
        if (--highScoreTimer == 0) {
            highScoreTimer = 120;
            highScoreTime = false;
        }
    }

    for (int oversampling = 8; oversampling > 0; oversampling--) {
        for (auto* player : players) {
            player->handleJoystick();
            player->step();
            for (auto* player2 : players) {
                if (player->collidesWith(player2->iPosition()) && player != player2 &&
                    !player->getIsDying() && !player2->getIsDying()) {
                    player2->die();
                    player->grow(static_cast<unsigned int>(player2->getSnakeLength() / 4));
                    player->speedUp(1.10F);
                    if (updateHighScoreFromToFile(player2->getSnakeLength())) {
                        highScoreTime = true;
                        highScoreColor = player2->getDefaultColor();
                    }
                }
            }
            if (player->getIsDead()) {
                player->reset();
            }
            player->render();
        }

        for (auto* f : food) {
            for (auto* p : players) {
                if (p->iPosition() == f->getPosition()) {
                    p->grow(2);
                    p->speedUp(1.05F);
                    f->eat();
                    food.push_back(new Food(this, getRandomPointOnScreen(ScreenNumber::anyScreen),
                                            Color::randomBlue() * 2.0F));
                }
            }
            f->render();
        }
        food.erase(std::remove_if(food.begin(), food.end(),
                                  [](Food* f) { return f->getIsEaten(); }),
                   food.end());
    }

    drawText(ScreenNumber::top, {cube::Font::right, 58}, highScoreColor * 0.5F,
             std::to_string(currentHighScore));
    return true;
}

bool Snake::updateHighScoreFromToFile(int score, const std::string& filename) {
    try {
        std::filesystem::path p(filename);
        if (p.has_parent_path()) {
            std::filesystem::create_directories(p.parent_path());
        }
    } catch (...) {}

    bool returnValue = false;
    std::fstream highScoreFile;
    highScoreFile.open(filename, std::fstream::binary | std::fstream::in);
    if (highScoreFile) {
        highScoreFile >> currentHighScore;
    } else {
        highScoreFile.clear();
        highScoreFile.open(filename, std::fstream::binary | std::fstream::in |
                                         std::fstream::out | std::fstream::trunc);
        highScoreFile << 0;
    }

    if (score > currentHighScore) {
        std::cout << "NEW HIGHSCORE: " << score << std::endl;
        currentHighScore = score;
        returnValue = true;
        highScoreFile.close();
        highScoreFile.open(filename, std::fstream::binary | std::fstream::in |
                                         std::fstream::out | std::fstream::trunc);
        highScoreFile << currentHighScore;
    }
    highScoreFile.close();
    return returnValue;
}

// ── Player ──────────────────────────────────────────────────────────────────
Snake::Player::Player(cube::CubeApp* renderCube, int joysticknumber, Vec3f setPosition,
                      Vec3f setVelocity, Color setColor, unsigned int length)
    : ca(renderCube) {
    joystickNumber = joysticknumber;
    tail.push_back(position);
    position = setPosition;
    velocity = setVelocity;
    acceleration = Vec3f{0.0F, 0.0F, 0.0F};
    color = setColor;
    defaultPosition = position;
    defaultVelocity = velocity;
    snakeLength = length;
    defaultSnakeLength = snakeLength;
    defaultColor = color;
}

void Snake::Player::reset() {
    position = defaultPosition;
    velocity = defaultVelocity;
    color = defaultColor;
    snakeLength = defaultSnakeLength;
    isDying = false;
    isDead = false;
    tail.clear();
}

void Snake::Player::step() {
    if (!isDying) {
        accelerate();
        move();
        warp();

        for (int i = 0; i < 3; i++) {
            if (position[i] < 0.01F && position[i] > 0) {
                position[i] = 0;
            }
            if (position[i] > static_cast<float>(kMax) - 0.01F) {
                position[i] = static_cast<float>(kMax);
            }
        }

        if (iPosition() != toI(tail.back())) {
            tail.push_back(toF(iPosition()));
        }

        const Vec3i head = iPosition();
        int colCounter = 0;
        for (const auto& t : tail) {
            if (head == toI(t)) {
                colCounter++;
            }
        }
        if (colCounter > 1) {
            die();
        }

        if (tail.size() > snakeLength) {
            tail.erase(tail.begin(), tail.end() - static_cast<long>(snakeLength));
        }
    } else {
        color = (dieCounter / 40 % 2) ? Color::black() : Color::white();
        dieCounter++;
        if (dieCounter >= 200) {
            isDead = true;
        }
    }
}

void Snake::Player::accelerate() { velocity += acceleration; }
void Snake::Player::move() { position += velocity; }

void Snake::Player::warp() {
    for (int i = 0; i < 3; i++) {
        position[i] = std::clamp(position[i], 0.0F, static_cast<float>(kMax));
    }

    const Vec3i currentPosition = iPosition();
    const EdgeNumber currentEdge = cube::CubeApp::getEdgeNumber(currentPosition);

    if (currentEdge != EdgeNumber::anyEdge && currentEdge != lastEdge) {
        switch (currentEdge) {
        case EdgeNumber::topLeft:
        case EdgeNumber::topRight:
        case EdgeNumber::bottomRight:
        case EdgeNumber::bottomLeft:
            std::swap(velocity[2], velocity[0]);
            break;
        case EdgeNumber::topFront:
        case EdgeNumber::topBack:
        case EdgeNumber::bottomBack:
        case EdgeNumber::bottomFront:
            std::swap(velocity[2], velocity[1]);
            break;
        case EdgeNumber::frontRight:
        case EdgeNumber::backLeft:
        case EdgeNumber::leftFront:
        case EdgeNumber::rightBack:
            std::swap(velocity[0], velocity[1]);
            break;
        case EdgeNumber::anyEdge:
        default:
            break;
        }
        position = toF(currentPosition);
        if ((currentPosition[0] == 0 && velocity[0] < 0) ||
            (currentPosition[0] == kMax && velocity[0] > 0)) {
            velocity[0] *= -1;
        }
        if ((currentPosition[1] == 0 && velocity[1] < 0) ||
            (currentPosition[1] == kMax && velocity[1] > 0)) {
            velocity[1] *= -1;
        }
        if ((currentPosition[2] == 0 && velocity[2] < 0) ||
            (currentPosition[2] == kMax && velocity[2] > 0)) {
            velocity[2] *= -1;
        }
    }
    lastIPosition = currentPosition;
    lastEdge = currentEdge;
}

void Snake::Player::handleJoystick() {
    if (joystickNumber == 0 && joystick.isConnected()) {
        const float newAxis0 = joystick.axis(cube::Axis::LeftX);
        if (newAxis0 < 0 && lastAxis0 == 0) {
            turnLeft();
        } else if (newAxis0 > 0 && lastAxis0 == 0) {
            turnRight();
        }
        lastAxis0 = newAxis0;
    } else {
        doKiMove();
    }
}

void Snake::Player::doKiMove() {
    const int random = std::rand() % 512;
    if (random == 55) {
        turnLeft();
    } else if (random == 66) {
        turnRight();
    }
}

void Snake::Player::render() {
    for (const auto& t : tail) {
        ca->setPixel3D(toI(t), color);
    }
}

void Snake::Player::turnLeft() {
    if (isDying || cube::CubeApp::isOnEdge(iPosition())) {
        return;
    }
    if (position[2] == 0) {
        std::swap(velocity[0], velocity[1]);
        velocity[0] = -velocity[0];
    } else if (position[2] == static_cast<float>(kMax)) {
        std::swap(velocity[0], velocity[1]);
        velocity[1] = -velocity[1];
    } else if (position[1] == 0) {
        std::swap(velocity[0], velocity[2]);
        velocity[2] = -velocity[2];
    } else if (position[1] == static_cast<float>(kMax)) {
        std::swap(velocity[0], velocity[2]);
        velocity[0] = -velocity[0];
    } else if (position[0] == 0) {
        std::swap(velocity[1], velocity[2]);
        velocity[1] = -velocity[1];
    } else if (position[0] == static_cast<float>(kMax)) {
        std::swap(velocity[1], velocity[2]);
        velocity[2] = -velocity[2];
    }
}

void Snake::Player::turnRight() {
    if (isDying || cube::CubeApp::isOnEdge(iPosition())) {
        return;
    }
    if (position[2] == 0) {
        std::swap(velocity[0], velocity[1]);
        velocity[1] = -velocity[1];
    } else if (position[2] == static_cast<float>(kMax)) {
        std::swap(velocity[0], velocity[1]);
        velocity[0] = -velocity[0];
    } else if (position[1] == 0) {
        std::swap(velocity[0], velocity[2]);
        velocity[0] = -velocity[0];
    } else if (position[1] == static_cast<float>(kMax)) {
        std::swap(velocity[0], velocity[2]);
        velocity[2] = -velocity[2];
    } else if (position[0] == 0) {
        std::swap(velocity[1], velocity[2]);
        velocity[2] = -velocity[2];
    } else if (position[0] == static_cast<float>(kMax)) {
        std::swap(velocity[1], velocity[2]);
        velocity[1] = -velocity[1];
    }
}

bool Snake::Player::collidesWith(Vec3i point) {
    for (const auto& t : tail) {
        if (toI(t) == point) {
            return true;
        }
    }
    return false;
}

void Snake::Player::grow(unsigned int howMuch) { snakeLength += howMuch; }

void Snake::Player::speedUp(float factor) {
    if (norm(velocity) < 1.0F) {
        velocity = velocity * std::clamp(factor, 1.0F, 2.0F);
    }
}

void Snake::Player::die() {
    isDying = true;
    color = Color::white();
    dieCounter = 0;
}

cube::Vec3i Snake::Player::iPosition() const { return toI(position); }

// ── Food ────────────────────────────────────────────────────────────────────
Snake::Food::Food(cube::CubeApp* renderCube, Vec3i setPosition, Color setColor)
    : position(setPosition), color(setColor), ca(renderCube) {}

void Snake::Food::render() { ca->setPixel3D(position, color); }
