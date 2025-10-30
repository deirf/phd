#pragma once

#include "ParticlePSO.h"


class ParticlePSOCo : public ParticlePSO
{
private:
  PREC Weight = 0.5f;
  PREC Kappa = 0.729f;
  PREC Coefficient1 = 2.05f;
  PREC Coefficient2 = 2.05f;

public:
  ParticlePSOCo(Configuration& _config, COUNT_T _index, uint _pf_index) : ParticlePSO(_config, _index, _pf_index) {}
};
