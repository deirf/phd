#pragma once

#include "../../common/Types.h"
#include "../../common/Configuration.h"
#include "../../common/Random.h"
#include "../../common/SimpleShape.h"
#include "../../common/HTCShape.h"


class oSpark
{
private:
  Configuration& config;

  COUNT_T param_N = 0;
  COUNT_T index = 0;
  uint pf_index = 0;
  PREC* param = NULL;

  PREC* fitness = NULL;
  PREC amplitude = 0;

  PREC* functionValue = NULL;
  PREC* shape_fitness = NULL;
  PREC* moving_base = NULL;
  PREC* origparam = NULL;
  PREC* direction = NULL;

  ProblemShape* problemShape = NULL;

public:
  oSpark(Configuration& _config, COUNT_T _param_N, COUNT_T _index, uint _pf_index): config(_config), param_N(_param_N), index(_index), pf_index(_pf_index) {}
  ~oSpark() {}

  void Initialize(PREC* _param, PREC* _fitness, PREC* _direction, PREC* _shape_fitness, PREC* _function_value, COUNT_T _ShapeFunction);
  void InitializeShapeFunction(COUNT_T _ShapeFunction);
  PREC GetFitness() { return *this->fitness; }
  PREC* GetParams() { return this->param; }
  PREC* GetFunctionValue() { return this->functionValue; }
  COUNT_T GetIndex() { return this->index; }
  PREC GetAmplitude() { return this->amplitude; }
  void SetAmplitude(PREC _amplitude) { this->amplitude = _amplitude; }
  PREC* GetDirection() { return this->direction; }

  void Initialize();
  void FitnessPostProcessing();

  PREC GetMaxParam() { return this->config.max_param; }
  PREC GetMinParam() { return this->config.min_param; }

  void SetFitness(PREC* value) { this->fitness = value; }
  void SetShapeFitness(PREC* value) { this->shape_fitness = value; }
  void SetParam(PREC* value) { this->param = value; }
  void SetDirection(PREC* value) { this->direction = value; }
  void SetFunctionValue(PREC* value) { this->functionValue = value; }

  void SetParamValue(COUNT_T index, PREC value) { this->param[index]  = value; }
  void SetDirectionValue(COUNT_T index, PREC value) { this->direction[index]  = value; }
  void SetShapeFitnessValue(PREC value) { *this->shape_fitness = value; }
  PREC GetFunctionValueValue(COUNT_T index) { return this->functionValue[index]; }

  PREC GetParamValue(COUNT_T _index) { return this->param[ _index ]; }
  PREC GetShapeFitnessValue() { return *this->shape_fitness; }

  COUNT_T GetParamCount() { return this->config.searching_dimension; }
  ProblemShape* GetProblemShape() { return this->problemShape; }

  void GenerateParamByBase();

  void SetMovingBase( PREC* _moving_base ) { this->moving_base = _moving_base; }
  PREC* GetMovingBase() { return this->moving_base; }
};
