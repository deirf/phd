#pragma once

#include "Particle.h"


class ParticlePSO : public Particle
{
private:
  const PREC Weight = 0.5f;
  const PREC Kappa = 1;
  const PREC Coefficient1 = 2;
  const PREC Coefficient2 = 2;

protected:
  PREC CalculateSpeed();
  void CalculateNewVelocity(PREC* _global_best_params, PREC* _best_params_avg) override;
  PREC GetWeight(COUNT_T iter) { UNUSED(iter); return this->Weight; }

public:
  ParticlePSO(Configuration &_config, COUNT_T _index, uint _pf_index) : Particle(_config, _index, _pf_index) {}
};
