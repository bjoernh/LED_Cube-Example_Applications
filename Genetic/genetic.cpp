#include "genetic.h"

#include <cube/cube.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iterator>
#include <random>

Genetic::Genetic() : cube::CubeApp("genetic", 40) {
    width_ = cube::CUBE_SIZE;
    height_ = cube::CUBE_SIZE;
    popSize_ = width_ * height_;

    // Allocate memory
    children_ = new citizen[popSize_];
    parents_ = new citizen[popSize_];
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::random_device rd;
    rng_.seed(rd());

    // Set a random target_
    target_ = std::rand() & 0xFFFFFF;

    // Create the first generation of random children_
    for (int i = 0; i < popSize_; ++i) {
      children_[i].dna = std::rand() & 0xFFFFFF;
    }
}

bool Genetic::loop() {
  swap();
  sort();
  mate();
  std::shuffle(children_, children_ + popSize_, rng_);

  // Draw citizens to canvas
  for (int i = 0; i < popSize_; i++) {
    int c = children_[i].dna;
    int x = i % width_;
    int y = i / width_;
    cube::Color col{static_cast<std::uint8_t>(R(c)),
                    static_cast<std::uint8_t>(G(c)),
                    static_cast<std::uint8_t>(B(c))};
    for (int faceIdx = 0; faceIdx < 6; ++faceIdx) {
      screen(static_cast<cube::ScreenNumber>(faceIdx)).setPixel(x, y, col);
    }
  }

  // When we reach the 85% fitness threshold...
  if (is85PercentFit()) {
    // ...set a new random target_
    target_ = std::rand() & 0xFFFFFF;

    // Randomly mutate everyone for sake of new colors
    for (int i = 0; i < popSize_; ++i) {
      mutate(children_[i]);
    }
  }

  return true;
}

Genetic::~Genetic() {
  delete[] children_;
  delete[] parents_;
}

void Genetic::sort() {
  std::sort(parents_, parents_ + popSize_, comparer(target_));
}

void Genetic::mate() {
  const float eliteRate = 0.30F;
  const float mutationRate = 0.20F;

  const int numElite = static_cast<int>(popSize_ * eliteRate);
  for (int i = 0; i < numElite; ++i) {
    children_[i] = parents_[i];
  }

  for (int i = numElite; i < popSize_; ++i) {
    const float sexuallyActive = 1.0F - eliteRate;
    const int p1 = std::rand() % static_cast<int>(popSize_ * sexuallyActive);
    const int p2 = std::rand() % static_cast<int>(popSize_ * sexuallyActive);
    const unsigned int matingMask = (~0U) << (std::rand() % bitsPerPixel);

    // Make a baby
    unsigned int baby = (parents_[p1].dna & matingMask)
      | (parents_[p2].dna & ~matingMask);
    children_[i].dna = static_cast<int>(baby);

    // Mutate randomly based on mutation rate
    if ((static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) < mutationRate) {
      mutate(children_[i]);
    }
  }
}

void Genetic::swap() {
  citizen* temp = parents_;
  parents_ = children_;
  children_ = temp;
}

void Genetic::mutate(citizen& c) {
  c.dna ^= 1 << (std::rand() % bitsPerPixel);
}

bool Genetic::is85PercentFit() {
  int numFit = 0;
  for (int i = 0; i < popSize_; ++i) {
    if (calcFitness(children_[i].dna, target_) < 1) {
      ++numFit;
    }
  }
  return (static_cast<float>(numFit) / static_cast<float>(popSize_)) > 0.85F;
}

int Genetic::calcFitness(const int value, const int target) {
  int diffBits = 0;
  for (unsigned int diff = static_cast<unsigned int>(value ^ target); diff; diff &= diff - 1) {
    ++diffBits;
  }
  return diffBits;
}

Genetic::citizen::citizen() : dna(0) {}

Genetic::citizen::citizen(int chrom) : dna(chrom) {}

Genetic::comparer::comparer(int t) : target_(t) {}

inline bool Genetic::comparer::operator()(const citizen& c1, const citizen& c2) {
  return (calcFitness(c1.dna, target_) < calcFitness(c2.dna, target_));
}
