#include <cmath>

#include "ProblemFunctionProcessor.h"
#include "HeatConductionCalculator1DCPU.h"


void
ProblemFunctionProcessor::CalculateTargetFunction(PREC* _inputFunction, PREC* _calculatedFunction)
{
  this->processors[0]->CalculateTargetFunction(_inputFunction, _calculatedFunction);
}

void
ProblemFunctionProcessor::InitializeHeatParams()
{
  for (uint index = 0; index < this->processors.size(); index++)
  {
    this->processors[ index ]->Initialize();
  }
}

void
ProblemFunctionProcessor::DeInitializeHeatParams()
{
  for (uint index = 0; index < this->processors.size(); index++)
  {
    this->processors[ index ]->DeInitialize();
  }
}

HeatCalcParams*
ProblemFunctionProcessor::GetHeatCalcParams(COUNT_T index)
{
  return this->processors[ index ]->GetHeatCalcParams();
}

void
ProblemFunctionProcessor::CalculateFitness(PREC* _calculatedHeatFunction, PREC* _fitness, PREC* _fitnessFunction)
{
  this->processors[0]->CalculateFitness(_calculatedHeatFunction, _fitness, _fitnessFunction);
}

void
ProblemFunctionProcessor::BatchEvaluation(PREC* _inputFunction,
                                          COUNT_T _inputFunctionLen,
                                          PREC *_fitness,
                                          COUNT_T _number_of_all_sparks,
                                          PREC* _calculatedFunction,
                                          COUNT_T _calculatedFunctionLen,
                                          PREC* _fitnessFunction,
                                          COUNT_T _fitnessFunctionLen
                                         )
{
  if ( this->ctx.config.Environment.CPU_N == 0 )
  {
    return;
  }

  COUNT_T spark_start_index = 0;
  const COUNT_T avg_spark_load = _number_of_all_sparks / this->ctx.config.Environment.CPU_N;
  const COUNT_T all_spark_mod = _number_of_all_sparks % this->ctx.config.Environment.CPU_N;
  for (uint index = 0; index < this->processors.size(); index++)
  {
    const COUNT_T number_of_sparks = avg_spark_load + (index < all_spark_mod);
    this->processors[index]->CalculateFitnessManyAsync(
                                                       _inputFunction + spark_start_index * _inputFunctionLen,
                                                       number_of_sparks,
                                                       _fitness + spark_start_index,
                                                       _calculatedFunction + spark_start_index * _calculatedFunctionLen,
                                                       _fitnessFunction + spark_start_index * _fitnessFunctionLen
                                                      );

    spark_start_index += number_of_sparks;
  }

  // wait for background threads
  this->ctx.threadpools->ProblemFunctionThreadpool->wait_for_empty();
}

ProblemFunctionCalculator*
ProblemFunctionProcessor::InitProcessor(uint index)
{
  switch (this->ctx.config.Environment.dimension)
  {
    case 1:
      switch (this->ctx.pf_index)
      {
        case PF_HCP:
        case PF_REF:
          this->ctx.config.searching_dimension = this->ctx.config.IHCP.searching_dimension;
          this->ctx.config.max_param = this->ctx.config.IHCP.max_H;
          this->ctx.config.min_param = this->ctx.config.IHCP.min_H;
          this->ctx.config.reference_function_N = this->ctx.config.CoolingCurve.reference_function_N;
          return new HeatConductionCalculator1DCPU(this->ctx.threadpools->ProblemFunctionThreadpool, this->ctx.config, index, this->ctx.pf_index);

        default:
          break;
      }
      break;

    default:
      break;
  }

  DOES_NOT_REACH();
  return NULL;
}

ProblemFunctionProcessor::ProblemFunctionProcessor(Context& _ctx) : ctx ( _ctx )
{
  for (uint cpui = 0; cpui < this->ctx.config.Environment.CPU_N; cpui++)
  {
    ProblemFunctionCalculator *processor = this->InitProcessor(cpui);
    this->processors.emplace_back(processor);
  }
}

ProblemFunctionProcessor::~ProblemFunctionProcessor()
{
  for (uint cpui = 0; cpui < this->ctx.config.Environment.CPU_N; cpui++)
  {
    delete this->processors.back();
    this->processors.pop_back();
  }
}
