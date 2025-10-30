#include "ParticlePSO.h"


void
ParticlePSO::CalculateNewVelocity(PREC* _global_best_params, PREC* _best_params_avg)
{
  UNUSED( _best_params_avg );
  for (COUNT_T i = 0; i < this->config.searching_dimension; i++)
  {
    const PREC globalBestX = _global_best_params[i];
    const PREC localBestX = this->GetBestParamValue(i);
    const PREC w = this->GetWeight(i);
    const PREC v = this->GetVelocityValue(i);

    const PREC p = this->GetParamValue(i);
    const PREC c1 = this->Coefficient1;
    const PREC r1 = this->GetUniformRandom01();
    const PREC c2 = this->Coefficient2;
    const PREC r2 = this->GetUniformRandom01();

    PREC velo = this->Kappa * (w * v + (c1 * r1 * (localBestX - p)) + (c2 * r2 * (globalBestX - p)));

    if (velo == 0)
    {
      velo = this->GetUniformRandom01() * this->GetRandomDirection();
    }

    this->SetVelocityValue( i, velo );
  }
}
