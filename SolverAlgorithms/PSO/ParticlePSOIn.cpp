#include "ParticlePSOIn.h"


PREC
ParticlePSOIn::GetWeight(COUNT_T iter)
{
    const COUNT_T max_iteration = this->config.PSO.max_iteration;
    return ( this->WeightStart - this->WeightEnd ) * (PREC)( max_iteration - iter ) / (PREC)max_iteration + this->WeightEnd;
}
