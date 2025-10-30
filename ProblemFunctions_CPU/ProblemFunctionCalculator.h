#pragma once
#include <thread>
#include "../common/Configuration.h"

class ProblemFunctionCalculator
{
protected:
  Configuration &config;
  uint pf_index;

public:
  ProblemFunctionCalculator(Configuration& _config,  uint _pfindex) : config(_config), pf_index(_pfindex) {}
  virtual ~ProblemFunctionCalculator() {}

  virtual void CalculateTargetFunction(PREC* _inputFunction, PREC* _targetFunctionValues) = 0;
  virtual void CalculateFitnessMany(PREC* _inputFunction, COUNT_T N, PREC* _fitness, PREC* heatFunction, PREC* _fitnessFunction) = 0;
  virtual void CalculateFitnessManyAsync(PREC* _inputFunction, COUNT_T N, PREC* _fitness, PREC* heatFunction, PREC* _fitnessFunction) = 0;
  virtual void CalculateFitness(PREC* _targetFunction, PREC*_fitness, PREC*_fitnessFunction) = 0;
  virtual void Initialize() = 0;
  virtual void DeInitialize() = 0;
  virtual HeatCalcParams* GetHeatCalcParams() = 0;
};
