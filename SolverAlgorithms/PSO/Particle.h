#pragma once

#include "../../common/Configuration.h"
#include "../../common/Random.h"
#include "../../common/SimpleShape.h"
#include "../../common/HTCShape.h"

class Particle
{
private:
  PREC* param = NULL;
  PREC* velocity = NULL;
  PREC* fitness = NULL;
  PREC* shape_fitness = NULL;
  PREC* best_param = NULL;
  PREC* best_fitness = NULL;
  PREC* moving_base = NULL;
  PREC* functionValue = NULL;
  ProblemShape* problemShape = NULL;

protected:
  Configuration& config;

private:
  COUNT_T index;
  uint pf_index = 0;

public:
  UniformRnd<PREC> randomUniform;
  NormalRnd<PREC> randomNormal;

  Particle(Configuration& _config, COUNT_T _index, uint _pf_index) : config(_config), index(_index), pf_index(_pf_index) {}
  virtual ~Particle() {}

  void Initialize(COUNT_T _ShapeFunction);
  void InitializeShapeFunction(COUNT_T _ShapeFunction);
  void MoveParticle();
  void FitnessPostProcessing();
  virtual void CalculateNewVelocity(PREC* _global_best_params, PREC* _best_params_avg) { UNUSED(_global_best_params); UNUSED(_best_params_avg); DOES_NOT_REACH(); }

  PREC* GetParam() { return this->param; }
  PREC* GetVelo() { return this->velocity; }
  PREC* GetFunctionValue() { return this->functionValue; }

  PREC GetMaxParam() { return this->config.max_param; }
  PREC GetMinParam() { return this->config.min_param; }

  void SetFitness(PREC* value) { this->fitness = value; }
  void SetShapeFitness(PREC* value) { this->shape_fitness = value; }
  void SetVelocity(PREC* value) { this->velocity = value; }
  void SetParam(PREC* value) { this->param = value; }
  void SetFunctionValue(PREC* value) { this->functionValue = value; }
  void SetBestParam(PREC* value) { this->best_param = value; }
  void SetBestFitness(PREC* value) { this->best_fitness = value; }

  void SetParamValue(COUNT_T index, PREC value) { this->param[index]  = value; }
  void SetVelocityValue(COUNT_T index, PREC value) { this->velocity[index]  = value; }
  void SetFitnessValue(PREC value) { *this->fitness = value; }
  void SetShapeFitnessValue(PREC value) { *this->shape_fitness = value; }
  void SetBestFitnessValue(PREC value) { *this->best_fitness = value; }
  void SetBestParamValue(COUNT_T index, PREC value) { this->best_param[index] = value; }

  PREC GetParamValue(COUNT_T index) { return this->param[index]; }
  PREC GetFunctionValueValue(COUNT_T index) { return this->functionValue[index]; }
  PREC GetFitnessValue() { return *this->fitness; }
  PREC GetShapeFitnessValue() { return *this->shape_fitness; }
  PREC GetBestFitnessValue() { return *this->best_fitness; }
  PREC GetVelocityValue(COUNT_T index) { return this->velocity[index]; }
  PREC* GetVelocity() { return this->velocity; }
  PREC GetBestParamValue(COUNT_T index) { return this->best_param[index]; }

  COUNT_T GetIndex() { return this->index; }
  COUNT_T GetParamCount() { return this->config.searching_dimension; }
  ProblemShape* GetProblemShape() { return this->problemShape; }

  PREC GetMovingBaseValue( COUNT_T _index ) { return this->moving_base[ _index ]; }
  void SetMovingBase( PREC* _moving_base ) { this->moving_base = _moving_base; }
  PREC* GetMovingBase() { return this->moving_base; }

  int GetRandomDirection() { return randomUniform.getRandomDirection(); }
  PREC GetUniformRandom01() { return randomUniform.getRand01(); }
  PREC GetUniformRandom(PREC min, PREC max)  { return randomUniform.getRand(min, max); }

  PREC GetNormalRandom_0T1B()  { return randomNormal.getRand01_0T1B(); };
  PREC GetNormalRandom_1T0B()  { return randomNormal.getRand01_0B1T(); };
  PREC GetNormalRandom_0B1T2B()  { return randomNormal.getRand02_0B1T2B(); };
};
