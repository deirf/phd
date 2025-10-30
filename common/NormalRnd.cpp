#include "Random.h"

template class NormalRnd<PREC>;


template <typename RandomType> RandomType
NormalRnd<RandomType>::getRand02_0B1T2B()
{
  //values: 0:bottom, 1:top, 2:bottom
  const PREC random = getRand();
  return UNLIKELY(random > 2) ? 2 : (UNLIKELY(random < 0) ? 0 : random);
}

template <typename RandomType> RandomType
NormalRnd<RandomType>::getRand01_0T1B()
{
  //values: 0:top, 1:bottom
  return std::abs( this->getRand02_0B1T2B() - 1 );
}

template <typename RandomType> RandomType
NormalRnd<RandomType>::getRand01_0B1T()
{
  //values: 1:top, 0:bottom
  return 1 - this->getRand01_0T1B();
}

template <typename RandomType> RandomType
NormalRnd<RandomType>::getRand_MIN_T_MAX_B(PREC min, PREC max)
{
  //values: min:top, max:bottom
  return (max - min) * this->getRand01_0T1B() + min;
}

template <typename RandomType> RandomType
NormalRnd<RandomType>::getRand()
{
  return normal(this->rng);
}
