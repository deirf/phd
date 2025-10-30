#pragma once

#include <random>
#include "Types.h"

template <typename RandomType>
class Random
{
public:
    std::mt19937_64 rng{};

  Random();
  ~Random() {}
};


template <typename RandomType>
class UniformRnd : public Random<RandomType>
{
  private:
    const PREC min = 0;
    const PREC max = 1;

  public:
    std::uniform_real_distribution<RandomType> uniform{ this->min, this->max };

    UniformRnd() {}
    ~UniformRnd() {}

    RandomType getRand01();
    int getRandomDirection();
    bool getRandomCoinSide();
    RandomType getRand(PREC min, PREC max);
    RandomType getRand(COUNT_T min, COUNT_T max);
};


template <typename RandomType>
class NormalRnd : public Random<RandomType>
{
  private:
    const PREC default_mean = 1;
    const PREC default_stddev = (PREC)0.293;

  public:
    std::normal_distribution<RandomType> normal;

    NormalRnd()
    {
      this->normal = std::normal_distribution<RandomType>{ this->default_mean, this->default_stddev };
    }

    NormalRnd(PREC _mean, PREC _stddev)
    {
      this->normal = std::normal_distribution<RandomType>{ _mean, _stddev };
    }

    ~NormalRnd() {}

    RandomType getRand02_0B1T2B();
    RandomType getRand01_0T1B();
    RandomType getRand01_0B1T();
    RandomType getRand_MIN_T_MAX_B(PREC min, PREC max);
    RandomType getRand();
};
