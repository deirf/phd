#pragma once

#include "ParticlePSO.h"


class ParticlePSOIn : public ParticlePSO
{
private:
  PREC WeightStart = 1.2f;
  PREC WeightEnd = 0.1f;
  PREC Wight = 0.5f;
  PREC Kappa = 1;
  PREC Coefficient1 = 2;
  PREC Coefficient2 = 2;

protected:
  PREC GetWeight(COUNT_T iter);

public:
  ParticlePSOIn(Configuration& _config, COUNT_T _index, uint _pf_index) : ParticlePSO(_config, _index, _pf_index) {}
};
