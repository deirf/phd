#pragma once

#include "../common/Types.h"

void CPUCalculateFitness(HeatCalcParams* restrict _heatCalcParams, PREC* restrict _heatFunctionValues, PREC* restrict _fitness, PREC* restrict _fitnessVector);
void CPUCalculateTargetFunction(HeatCalcParams* _heatCalcParams, PREC* _htc, PREC* _heatFunctionValues);
void CPUHeatConductionWorker(HeatCalcParams* _heatCalcParams, COUNT_T _N, PREC* _fitnesses, PREC* _htcs, PREC* _heatFunctions, PREC* _fitnessFunctions);
