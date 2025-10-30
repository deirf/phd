#include "ParticleQPSOT2.h"


void
ParticleQPSOT2::CalculateNewVelocity(PREC* _global_best_params, PREC* _best_params_avg)
{

  for (COUNT_T i = 0; i < this->config.searching_dimension; i++)
  {
    const PREC globalBestX = _global_best_params[i];
    const PREC localBestX = this->GetBestParamValue(i);
    const PREC p = this->GetParamValue(i);
    const PREC C = _best_params_avg[i];
    const PREC fi = this->GetUniformRandom01();
    const PREC mixedBest = fi * localBestX + ( 1 - fi ) * globalBestX;
    const PREC rand = this->GetUniformRandom01();
    const PREC direction = (PREC)this->GetRandomDirection();

    const PREC velo = mixedBest + direction * (PREC)( this->Alpha * std::abs( p - C ) * log( 1 / rand ) );

    this->SetVelocityValue( i, velo - p );
  }
}
