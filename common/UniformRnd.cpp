#include "Random.h"

template class UniformRnd<PREC>;


template <typename RandomType> RandomType
UniformRnd<RandomType>::getRand01()
{
  return uniform(this->rng);
}

template <typename RandomType> RandomType
UniformRnd<RandomType>::getRand(PREC min, PREC max)
{
  return (max - min) * this->getRand01() + min;
}

template <typename RandomType> RandomType
UniformRnd<RandomType>::getRand(COUNT_T min, COUNT_T max)
{
  const RandomType random = ((RandomType)(max - min)) * this->getRand01();
  return random + (RandomType)min;
}

template <typename RandomType> int
UniformRnd<RandomType>::getRandomDirection()
{
  int direction = 0;

  while ( direction == 0 )
  {
    direction = ( this->getRand01() > 0.5 ) * 2 - 1;
  }

  return direction;
}

template <typename RandomType> bool
UniformRnd<RandomType>::getRandomCoinSide()
{
  return this->getRandomDirection() == 1;
}
