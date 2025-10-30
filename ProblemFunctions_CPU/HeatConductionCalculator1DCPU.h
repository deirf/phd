#pragma once

#include "ProblemFunctionCalculator.h"
#include "../common/ThreadPool.h"


class HeatConductionCalculator1DCPU : public ProblemFunctionCalculator
{
  uint cpu_id;

  void CalculateFitnessManyAsyncWorker(PREC* _htcs, COUNT_T _N, PREC* _fitnesses, PREC* _heatFunctions, PREC* _fitnessFunctions);
  void FillHeatCalcParams(HeatCalcParams* heatCalcParams);

protected:
  HeatCalcParams* heatCalcParams;

public:
  HeatConductionCalculator1DCPU(ThreadPool* _threadpool, Configuration& config, uint cpu_id, uint _pfindex);
  ~HeatConductionCalculator1DCPU();

  ThreadPool* threadpool;
  void CalculateTargetFunction(PREC* _htc, PREC* _heatFunctionValue) override;
  void CalculateFitnessMany(PREC* _htcs, COUNT_T _N, PREC* _fitnesses, PREC* _heatFunctions, PREC* _fitnessFunctions) override;
  void CalculateFitnessManyAsync(PREC* _htcs, COUNT_T _N, PREC* _fitnesses, PREC* _heatFunctions, PREC* _fitnessFunctions) override;
  void CalculateFitness(PREC* _heatFunctionValue, PREC*_fitness, PREC*_fitnessFunction) override;
  void Initialize() override;
  void DeInitialize() override;
  HeatCalcParams* GetHeatCalcParams() override;
};
