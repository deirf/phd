#pragma once

#include "Particle.h"


class ParticleQPSOT2 : public Particle
{
private:
  PREC Alpha = 0.75;

protected:
  void CalculateNewVelocity(PREC* _global_best_params, PREC* _best_params_avg) override;

public:
  ParticleQPSOT2(Configuration& _config, COUNT_T _index, uint _pf_index) : Particle(_config, _index, _pf_index) {}
};
