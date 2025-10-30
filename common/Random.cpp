#include "Random.h"

template class Random<PREC>;


template <typename RandomType>
Random<RandomType>::Random()
{
    // initialize the random number generator with time-dependent seed
    COUNT_T timeSeed = (COUNT_T)std::chrono::high_resolution_clock::now().time_since_epoch().count();
    // uint64_t timeSeed = 0;
    std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32) };

    rng.seed(ss);
}
