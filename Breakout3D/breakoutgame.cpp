#include "breakoutgame.h"

#include <cube/cube.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numbers>
#include <string>
#include <vector>

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
}  // namespace

BreakoutGame::BreakoutGame() : cube::CubeApp("breakout3d", 40) {
    updateHighScoreFromToFile(0);
    beginMenu();
}

void BreakoutGame::reset(int numPlayers, int gameDuration) {
    gameState_ = pregame;
    remainingSeconds_ = gameDuration;
    for (auto p : players_) delete p;
    for (auto b : balls_) delete b;
    for (auto blk : blocks_) delete blk;
    players_.clear();
    balls_.clear();
    blocks_.clear();

    const int n = std::clamp(numPlayers, 1, 2);
    for (int i = 0; i < n; i++) {
        players_.push_back(new Player(this, i, i));
        spawnBallForPlayer(i);
    }
    int blockSize = 4;
    int blockScore = 25;
    for (int i = 0; i < cube::CUBE_SIZE; i += blockSize) {
        for (int j = 0; j < cube::CUBE_SIZE; j += blockSize) {
            blocks_.push_back(new Block(this, cube::ScreenNumber::top, cube::Vec2i{i, j}, blockSize, blockScore,
                                        (cube::Color::randomGreen() + cube::Color::blue() * 0.5F + cube::Color::randomBlue()) * 0.7F));
        }
    }
}

void BreakoutGame::beginMenu() {
    for (auto p : players_) delete p;
    for (auto b : balls_) delete b;
    for (auto blk : blocks_) delete blk;
    players_.clear();
    balls_.clear();
    blocks_.clear();
    startMenu_.emplace(1, 2, false);
}

void BreakoutGame::spawnBallForPlayer(int playerId) {
    switch (playerId) {
        case 0:
            balls_.push_back(new Ball(this, cube::Vec3f{static_cast<float>(cube::VIRTUAL_CUBE_CENTER), 0.0F, static_cast<float>(cube::VIRTUAL_CUBE_CENTER)}, cube::Vec3f{0.0F, 0.0F, 1.0F}, 1.7F));
            break;
        case 1:
            balls_.push_back(new Ball(this, cube::Vec3f{static_cast<float>(cube::VIRTUAL_CUBE_CENTER), static_cast<float>(cube::VIRTUAL_CUBE_MAX_INDEX), static_cast<float>(cube::VIRTUAL_CUBE_CENTER)}, cube::Vec3f{0.0F, 0.0F, 1.0F}, 1.7F));
            break;
    }
}

BreakoutGame::Player * BreakoutGame::getLeadingPlayer() {
    Player * leadingPlayer = nullptr;
    int leadScore = 0;
    for (auto player : players_) {
        if (player->score() >= leadScore) {
            leadScore = player->score();
            leadingPlayer = player;
        }
    }
    return leadingPlayer;
}

void BreakoutGame::ballLoop() {
    for (auto ball : balls_) {
        ball->step();
        ball->render();
        if (ball->iPosition().z == cube::VIRTUAL_CUBE_MAX_INDEX && !ball->isDead()) {
            if (ball->lastPlayer() != nullptr) {
                ball->lastPlayer()->addToScore(-10);
            }
            ball->die();
        }
    }
}

void BreakoutGame::blockLoop() {
    for (auto block : blocks_) {
        block->render();
        for (auto ball : balls_) {
            if (block->collidesWith(ball->position())) {
                if (ball->lastPlayer() != nullptr) {
                    ball->lastPlayer()->addToScore(static_cast<float>(block->score()));
                }

                cube::Vec3f incidentVec3 = ball->position() - block->centerPosition();
                std::vector<cube::Vec3f> possibleReflectionVectors;
                float angle = std::atan2(incidentVec3.x, incidentVec3.y) * 180.0F / kPi;
                while (angle < 0.0F) {
                    angle += 360.0F;
                }

                int angleTolerance = 10;
                if ((angle > 315.0F - angleTolerance || angle < 45.0F + angleTolerance)) {
                    possibleReflectionVectors.push_back(cube::Vec3f{0.0F, 1.0F, 0.0F});
                }
                if (angle > 45.0F - angleTolerance && angle < 135.0F + angleTolerance) {
                    possibleReflectionVectors.push_back(cube::Vec3f{1.0F, 0.0F, 0.0F});
                }
                if (angle > 135.0F - angleTolerance && angle < 225.0F + angleTolerance) {
                    possibleReflectionVectors.push_back(cube::Vec3f{0.0F, -1.0F, 0.0F});
                }
                if (angle > 225.0F - angleTolerance && angle < 315.0F + angleTolerance) {
                    possibleReflectionVectors.push_back(cube::Vec3f{-1.0F, 0.0F, 0.0F});
                }

                if (possibleReflectionVectors.empty()) {
                    possibleReflectionVectors.push_back(cube::Vec3f{0.0F, 1.0F, 0.0F});
                }

                cube::Vec3f finalReflectionVector = possibleReflectionVectors.front();
                float maxSimilarity = -2.0F;
                for (auto vec : possibleReflectionVectors) {
                    float dotProduct = vec.x * ball->velocity().x + vec.y * ball->velocity().y + vec.z * ball->velocity().z;
                    if (dotProduct > maxSimilarity) {
                        maxSimilarity = dotProduct;
                        finalReflectionVector = vec;
                    }
                }

                ball->reflect(finalReflectionVector);
                block->die();
            }
        }
    }
    blocks_.erase(std::remove_if(blocks_.begin(), blocks_.end(), [](Block * b) {
        if (b->isDead()) {
            delete b;
            return true;
        }
        return false;
    }), blocks_.end());
}

void BreakoutGame::playerLoop() {
    for (auto player : players_) {
        player->step();
        player->render();
        for (auto ball : balls_) {
            if (player->collidesWith(ball->iPosition())) {
                ball->setLastPlayer(player);
                player->setLastBall(ball);
                ball->velocity(toF(ball->iPosition() - player->centerPosition()));
                ball->accelerate();
            }
        }
    }
}

bool BreakoutGame::isBlockAtPoint(cube::Vec3f point) {
    for (auto block : blocks_) {
        if (block->collidesWith(point)) {
            return true;
        }
    }
    return false;
}

bool BreakoutGame::loop() {
    static int loopcount = 0;

    if (startMenu_.has_value()) {
        startMenu_->tick(*this);
        if (!startMenu_->active()) {
            if (startMenu_->cancelled()) return false;
            reset(startMenu_->playerCount());
            startMenu_.reset();
        }
        return true;
    }

    switch (gameState_) {
        case pregame: {
            clear();
            const std::string playtext = "PRESS A TO PLAY";
            const std::string rbutton  = "A: cycle color";
            const std::string bbutton  = "B: warp ball";

            for (int i = 0; i < 4; i++) {
                drawText(static_cast<cube::ScreenNumber>(i), cube::Vec2i{cube::Font::centered, 20}, cube::Color::white(), playtext);
                drawText(static_cast<cube::ScreenNumber>(i), cube::Vec2i{cube::Font::centered, 30}, cube::Color::white() * 0.5F, rbutton);
                drawText(static_cast<cube::ScreenNumber>(i), cube::Vec2i{cube::Font::centered, 36}, cube::Color::white() * 0.5F, bbutton);
                drawText(static_cast<cube::ScreenNumber>(i), cube::Vec2i{cube::Font::right, 58}, cube::Color::white() * 0.5F, std::to_string(currentHighScore));
            }
            if (joystick_.justPressed(cube::Btn::A)) {
                gameState_ = ingame;
                for (auto ball : balls_) {
                    ball->die();
                    ball->position(cube::Vec3f{static_cast<float>(cube::CUBE_CENTER), static_cast<float>(cube::CUBE_CENTER), static_cast<float>(cube::CUBE_CENTER)});
                }
            }
            break;
        }
        case ingame: {
            clear();
            ballLoop();
            blockLoop();
            playerLoop();

            for (auto ball : balls_) {
                ball->render();
            }

            int scrNrCounter = 0;
            for (auto player : players_) {
                drawText(static_cast<cube::ScreenNumber>(scrNrCounter), cube::Vec2i{0, 58}, player->color(), std::string("P") + std::to_string(player->getId()) + ": ");
                drawText(static_cast<cube::ScreenNumber>(scrNrCounter), cube::Vec2i{24, 58}, cube::Color::white(), std::to_string(player->score()));
                drawText(static_cast<cube::ScreenNumber>(scrNrCounter + 2), cube::Vec2i{0, 58}, player->color(), std::string("P") + std::to_string(player->getId()) + ": ");
                drawText(static_cast<cube::ScreenNumber>(scrNrCounter + 2), cube::Vec2i{24, 58}, cube::Color::white(), std::to_string(player->score()));
                scrNrCounter++;
            }

            drawText(cube::ScreenNumber::front, cube::Vec2i{cube::Font::right, 58}, cube::Color::white(), std::to_string(remainingSeconds_));
            drawText(cube::ScreenNumber::back, cube::Vec2i{cube::Font::right, 58}, cube::Color::white(), std::to_string(remainingSeconds_));

            if (loopcount % getFps() == 0) {
                remainingSeconds_--;
            }

            if (remainingSeconds_ < 0) {
                gameState_ = postgame;
                postgameCounter = 10;
            }

            if (joystick_.justPressed(cube::Btn::StickLeft)) {
                beginMenu();
            }
            break;
        }
        case postgame: {
            clear();
            int scrNrCounter = 0;
            for (auto p : players_) {
                drawText(static_cast<cube::ScreenNumber>(scrNrCounter), cube::Vec2i{cube::CUBE_CENTER - 24, cube::Font::centered}, p->color(), std::string("PLAYER ") + std::to_string(p->getId()) + ": ");
                drawText(static_cast<cube::ScreenNumber>(scrNrCounter), cube::Vec2i{cube::CUBE_CENTER + 20, cube::Font::centered}, cube::Color::white(), std::to_string(p->score()));
                drawText(static_cast<cube::ScreenNumber>(scrNrCounter + 2), cube::Vec2i{cube::CUBE_CENTER - 24, cube::Font::centered}, p->color(), std::string("PLAYER ") + std::to_string(p->getId()) + ": ");
                drawText(static_cast<cube::ScreenNumber>(scrNrCounter + 2), cube::Vec2i{cube::CUBE_CENTER + 20, cube::Font::centered}, cube::Color::white(), std::to_string(p->score()));
                scrNrCounter++;
            }
            if (loopcount % getFps() == 0) {
                postgameCounter--;
            }
            if (postgameCounter < 0) {
                if (getLeadingPlayer() != nullptr) {
                    updateHighScoreFromToFile(getLeadingPlayer()->score());
                }
                beginMenu();
            }
            break;
        }
    }
    loopcount++;
    return true;
}

bool BreakoutGame::updateHighScoreFromToFile(int score, std::string filename) {
    try {
        std::filesystem::path p(filename);
        if (p.has_parent_path()) {
            std::filesystem::create_directories(p.parent_path());
        }
    } catch (...) {}

    bool returnValue = false;
    std::ifstream checkFile(filename);
    if (checkFile) {
        checkFile >> currentHighScore;
    } else {
        std::ofstream createFile(filename);
        createFile << 0;
        currentHighScore = 0;
    }

    if (score > currentHighScore) {
        currentHighScore = score;
        returnValue = true;
        std::ofstream updateFile(filename);
        updateFile << currentHighScore;
    }
    return returnValue;
}

// ─── Player ──────────────────────────────────────────────────────────────────

BreakoutGame::Player::Player(cube::CubeApp * renderCube, int id, int joystickId) {
    ca_ = renderCube;
    id_ = id;
    joystickId_ = joystickId;
    score_ = 0;
    width_ = 10;
    height_ = 10;
    vel_ = 0.0F;

    if (id_ == 0) {
        color_ = cube::Color::red();
        pos_ = static_cast<float>(cube::VIRTUAL_CUBE_CENTER);
        minPos_ = width_ / 2;
        maxPos_ = cube::VIRTUAL_CUBE_MAX_INDEX - width_ / 2;
    } else {
        color_ = cube::Color::blue();
        pos_ = static_cast<float>(cube::VIRTUAL_CUBE_CENTER + 128);
        minPos_ = 128 + width_ / 2;
        maxPos_ = 128 + cube::VIRTUAL_CUBE_MAX_INDEX - width_ / 2;
    }
    blinkCount_ = 0;
    lastBall_ = nullptr;
}

void BreakoutGame::Player::render() {
    cube::Color paddleColor = color();
    for (auto p : paddlePixels_) {
        ca_->setPixel3D(p, paddleColor);
    }
}

void BreakoutGame::Player::step() {
    if (joystickId_ == 0 && joystick_.isConnected()) {
        vel_ = joystick_.axis(cube::Axis::LeftX) * 1.8F;
        if (lastBall_ != nullptr) {
            if (joystick_.isHeld(cube::Btn::ShoulderRight) && score_ > 0) {
                lastBall_->setSpeed(0.5F);
                addToScore(-0.025F);
            } else {
                lastBall_->resetSpeed();
            }
            if (joystick_.justPressed(cube::Btn::B)
                && score_ > 100
                && ca_->getScreenNumber(lastBall_->iPosition()) != cube::ScreenNumber::top
                && !ca_->isOnEdge(lastBall_->iPosition())
                && (lastBall_->velocity().x != 0.0F || lastBall_->velocity().y != 0.0F || lastBall_->velocity().z > 0.0F)) {
                lastBall_->velocity(cube::Vec3f{0.0F, 0.0F, -1.0F});
                lastBall_->resetSpeed();
                addToScore(-100.0F);
            }
        }
    } else {
        doKIMove();
    }
    pos_ += vel_;
    pos_ = constrain(pos_, static_cast<float>(minPos_), static_cast<float>(maxPos_));
    generatePaddle();
}

void BreakoutGame::Player::doKIMove() {
    if (lastBall_ != nullptr) {
        cube::Vec3i BallPos = lastBall_->iPosition();
        int xPosBall = 0;
        if (ca_->getScreenNumber(BallPos) == cube::ScreenNumber::front) {
            xPosBall = BallPos.x;
        } else if (ca_->getScreenNumber(BallPos) == cube::ScreenNumber::right) {
            xPosBall = BallPos.y + cube::CUBE_SIZE;
        } else if (ca_->getScreenNumber(BallPos) == cube::ScreenNumber::back) {
            xPosBall = cube::CUBE_SIZE - BallPos.x + 128;
        } else if (ca_->getScreenNumber(BallPos) == cube::ScreenNumber::left) {
            xPosBall = cube::CUBE_SIZE - BallPos.y + cube::CUBE_SIZE + 128;
        }

        xPosBall += 10 - std::rand() % 20;
        if (xPosBall < pos_) {
            vel_ = -1.0F;
        } else if (xPosBall > pos_) {
            vel_ = 1.0F;
        } else {
            vel_ = 0.0F;
        }
    } else {
        if (id_ == 0) {
            pos_ = static_cast<float>(cube::VIRTUAL_CUBE_CENTER);
        } else {
            pos_ = static_cast<float>(cube::VIRTUAL_CUBE_CENTER + 128);
        }
    }
}

void BreakoutGame::Player::generatePaddle() {
    paddlePixels_.clear();
    int intPos = static_cast<int>(std::round(pos_));
    for (int i = 8; i < 8 + height_; i++) {
        for (int j = intPos - (width_ / 2); j < intPos + (width_ / 2); j++) {
            cube::Vec3i tempPoint{0, 0, 0};
            int realJ = j;
            if (realJ >= 0 && realJ < cube::CUBE_SIZE) { // front face
                tempPoint = {realJ, 0, i};
            } else if (realJ >= cube::CUBE_SIZE && realJ < 128) { // right face
                tempPoint = {cube::VIRTUAL_CUBE_MAX_INDEX, realJ - cube::CUBE_SIZE, i};
            } else if (realJ >= 128 && realJ < 128 + cube::CUBE_SIZE) { // back face
                tempPoint = {cube::CUBE_SIZE - (realJ - 128), cube::VIRTUAL_CUBE_MAX_INDEX, i};
            } else if (realJ >= 128 + cube::CUBE_SIZE && realJ < 256) { // left face
                tempPoint = {0, cube::CUBE_SIZE - (realJ - 128 - cube::CUBE_SIZE), i};
            }
            paddlePixels_.push_back(tempPoint);
        }
    }
}

cube::Vec3i BreakoutGame::Player::centerPosition() {
    int intPos = static_cast<int>(std::round(pos_));
    cube::Vec3i tempPoint{0, 0, 8 + height_ / 2};
    int realJ = intPos;
    if (realJ >= 0 && realJ < cube::CUBE_SIZE) {
        tempPoint.x = realJ;
        tempPoint.y = 0;
    } else if (realJ >= cube::CUBE_SIZE && realJ < 128) {
        tempPoint.x = cube::VIRTUAL_CUBE_MAX_INDEX;
        tempPoint.y = realJ - cube::CUBE_SIZE;
    } else if (realJ >= 128 && realJ < 128 + cube::CUBE_SIZE) {
        tempPoint.x = cube::CUBE_SIZE - (realJ - 128);
        tempPoint.y = cube::VIRTUAL_CUBE_MAX_INDEX;
    } else if (realJ >= 128 + cube::CUBE_SIZE && realJ < 256) {
        tempPoint.x = 0;
        tempPoint.y = cube::CUBE_SIZE - (realJ - 128 - cube::CUBE_SIZE);
    }
    return tempPoint;
}

bool BreakoutGame::Player::collidesWith(cube::Vec3i pos) {
    for (auto pixel : paddlePixels_) {
        if (pixel == pos) {
            return true;
        }
    }
    return false;
}

void BreakoutGame::Player::blink(cube::Color color) {
    blinkCount_ = 10;
    blinkColor_ = color;
}

int BreakoutGame::Player::getId() {
    return id_;
}

cube::Color BreakoutGame::Player::color() {
    if (blinkCount_ > 0) {
        blinkCount_--;
        return blinkColor_;
    }
    return color_;
}

bool BreakoutGame::Player::addToScore(float value) {
    score_ += static_cast<int>(value);
    if (score_ < 0) {
        score_ = 0;
    }
    return true;
}

int BreakoutGame::Player::score() {
    return score_;
}

void BreakoutGame::Player::setLastBall(Ball * ball) {
    lastBall_ = ball;
}

BreakoutGame::Ball * BreakoutGame::Player::lastBall() {
    return lastBall_;
}

// ─── Ball ──────────────────────────────────────────────────────────────────

BreakoutGame::Ball::Ball(cube::CubeApp * renderCube, cube::Vec3f startPosition, cube::Vec3f startVelocity, float speed) {
    ca_ = renderCube;
    position_ = startPosition;
    defaultPosition_ = startPosition;
    velocity_ = startVelocity;
    defaultVelocity_ = startVelocity;
    color_ = cube::Color::white();
    isDead_ = false;
    respawnTimer_ = 0;
    speed_ = speed;
    defaultSpeed_ = speed;
    lastEdge_ = cube::EdgeNumber::anyEdge;
    lastIPosition_ = iPosition();
    lastPlayer_ = nullptr;
}

void BreakoutGame::Ball::render() {
    if (!isDead_) {
        ca_->setPixel3D(iPosition(), color_);
    }
}

void BreakoutGame::Ball::reflect(cube::Vec3f reflectionVector) {
    float similarity = velocity_.x * reflectionVector.x + velocity_.y * reflectionVector.y + velocity_.z * reflectionVector.z;
    if (similarity > 0.0F) {
        velocity_ = velocity_ - reflectionVector * (2.0F * similarity);
    }
}

void BreakoutGame::Ball::step() {
    if (isDead_) {
        respawnTimer_--;
        if (respawnTimer_ < 0) {
            reset();
        }
    } else {
        accelerate();
        move();
    }
}

void BreakoutGame::Ball::accelerate() {
    velocity_.x = constrain(velocity_.x, -1.0F, 1.0F);
    velocity_.y = constrain(velocity_.y, -1.0F, 1.0F);
    velocity_.z = constrain(velocity_.z, -1.0F, 1.0F);
}

void BreakoutGame::Ball::move() {
    position_ += velocity_ * speed_;

    position_.x = constrain(position_.x, 0.0F, static_cast<float>(cube::VIRTUAL_CUBE_MAX_INDEX));
    position_.y = constrain(position_.y, 0.0F, static_cast<float>(cube::VIRTUAL_CUBE_MAX_INDEX));
    position_.z = constrain(position_.z, 0.0F, static_cast<float>(cube::VIRTUAL_CUBE_MAX_INDEX));

    cube::Vec3i currentPosition = iPosition();
    cube::EdgeNumber currentEdge = ca_->getEdgeNumber(currentPosition);

    if (currentEdge != cube::EdgeNumber::anyEdge) {
        if (currentEdge != lastEdge_) {
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

            position_ = toF(currentPosition);

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
    lastIPosition_ = currentPosition;
    lastEdge_ = currentEdge;
}

void BreakoutGame::Ball::revertStep() {
    position_ -= velocity_ * speed_;
}

void BreakoutGame::Ball::die() {
    isDead_ = true;
    respawnTimer_ = 80;
}

bool BreakoutGame::Ball::isDead() {
    return isDead_;
}

void BreakoutGame::Ball::reset() {
    position_ = defaultPosition_;
    velocity_ = defaultVelocity_;
    isDead_ = false;
    speed_ = defaultSpeed_;
}

void BreakoutGame::Ball::setSpeed(float speed) {
    speed_ = speed;
}

void BreakoutGame::Ball::resetSpeed() {
    speed_ = defaultSpeed_;
}

void BreakoutGame::Ball::setLastPlayer(Player * player) {
    lastPlayer_ = player;
}

BreakoutGame::Player * BreakoutGame::Ball::lastPlayer() {
    return lastPlayer_;
}

cube::Vec3f BreakoutGame::Ball::position() {
    return position_;
}

cube::Vec3f BreakoutGame::Ball::velocity() {
    return velocity_;
}

cube::Vec3f BreakoutGame::Ball::acceleration() {
    return acceleration_;
}

cube::Vec3i BreakoutGame::Ball::iPosition() {
    return {static_cast<int>(std::round(position_.x)),
            static_cast<int>(std::round(position_.y)),
            static_cast<int>(std::round(position_.z))};
}

void BreakoutGame::Ball::position(cube::Vec3f pos) {
    position_ = pos;
}

void BreakoutGame::Ball::velocity(cube::Vec3f vel) {
    velocity_ = vel;
}

void BreakoutGame::Ball::acceleration(cube::Vec3f accel) {
    acceleration_ = accel;
}

cube::Color BreakoutGame::Ball::color() {
    return color_;
}

void BreakoutGame::Ball::color(cube::Color Col) {
    color_ = Col;
}

// ─── Block ──────────────────────────────────────────────────────────────────

BreakoutGame::Block::Block(cube::CubeApp * renderCube, cube::ScreenNumber screenNr, cube::Vec2i topLeftCorner, int size, int score, cube::Color color) {
    ca_ = renderCube;
    screenNr_ = screenNr;
    topLeftCorner_ = topLeftCorner;
    size_ = size;
    score_ = score;
    color_ = color;
    isDead_ = false;

    for (int i = topLeftCorner.x; i < topLeftCorner.x + size; i++) {
        for (int j = topLeftCorner.y; j < topLeftCorner.y + size; j++) {
            blockPixels_.push_back(cube::CubeApp::getPointOnScreen(screenNr_, cube::Vec2i{i, j}));
        }
    }
}

void BreakoutGame::Block::render() {
    if (!isDead_) {
        for (auto pixel : blockPixels_) {
            ca_->setPixel3D(pixel, color_);
        }
    }
}

bool BreakoutGame::Block::collidesWith(cube::Vec3f pos) {
    if (isDead_) {
        return false;
    }
    cube::Vec3i iPos = {static_cast<int>(std::round(pos.x)),
                        static_cast<int>(std::round(pos.y)),
                        static_cast<int>(std::round(pos.z))};
    for (auto pixel : blockPixels_) {
        if (pixel == iPos) {
            return true;
        }
    }
    return false;
}

cube::Vec3f BreakoutGame::Block::centerPosition() {
    cube::Vec2i centerPoint{topLeftCorner_.x + size_ / 2, topLeftCorner_.y + size_ / 2};
    return toF(cube::CubeApp::getPointOnScreen(screenNr_, centerPoint));
}

void BreakoutGame::Block::die() {
    isDead_ = true;
}

bool BreakoutGame::Block::isDead() {
    return isDead_;
}

int BreakoutGame::Block::score() {
    return score_;
}

int BreakoutGame::Block::size() {
    return size_;
}
