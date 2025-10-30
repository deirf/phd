#pragma once
#include "../common/Configuration.h"
#include "ProblemFunctionCalculator.h"
#include <vector>
#include <memory>
#include <string>
#include <mutex>


class ProblemFunctionProcessor
{
protected:
	Context& ctx;
	std::vector<ProblemFunctionCalculator*> processors;
	ProblemFunctionCalculator* InitProcessor(uint index);

public:
	void CalculateTargetFunction(PREC* _inputFunction, PREC* _calculatedFunction);
  void CalculateFitness(PREC* _calculatedHeatFunction, PREC* _fitness, PREC* _fitnessFunction);
	void BatchEvaluation(PREC* _inputFunction,
                       COUNT_T _inputFunctionLen,
                       PREC *_fitness,
                       COUNT_T _number_of_all_sparks,
                       PREC* _calculatedFunction,
                       COUNT_T _calculatedFunctionLen,
                       PREC* _fitnessFunction,
                       COUNT_T _fitnessFunctionLen
                      );

	explicit ProblemFunctionProcessor(Context& _ctx);
  ~ProblemFunctionProcessor();
  void InitializeHeatParams();
  void DeInitializeHeatParams();
  HeatCalcParams* GetHeatCalcParams(COUNT_T index);
};
