#pragma once

#include "../../common/Configuration.h"
#include "../../ProblemFunctions_CPU/ProblemFunctionProcessor.h"
#include "../../common/Random.h"
#include "../../common/LogWorker.h"

#include "ParticlePSO.h"
#include "ParticlePSOIn.h"
#include "ParticlePSOCo.h"
#include "ParticleQPSOT1.h"
#include "ParticleQPSOT2.h"

class Swarm
{
private:
  PREC* evaluation_FunctionValue_store;
  PREC* evaluation_FitnessFunction_store;
  uint logindex = 0;
  PREC min_fitness = PREC_MAX;

  COUNT_T logItemCount = 0;

  void IncLogItemCount( COUNT_T _count ) { this->logItemCount += _count; }

  TArray(PREC)* ConvertParamsArrayToTArray();
  TArray(PREC)* ConvertVelosArrayToTArray();
  TArray(PREC)* ConvertOthersArrayToTArray();
  TArray(PREC)* ConvertFunctionValueArrayToTArray();

  TArray(PREC)* ConvertBestParamsArrayToTArray();
  TArray(PREC)* ConvertBestVelosArrayToTArray();
  TArray(PREC)* ConvertBestOthersArrayToTArray();
  TArray(PREC)* ConvertBestFunctionValueArrayToTArray();

  void ConvertOneArrayToTArray(TArray(PREC)* _to, PREC* _from, COUNT_T _len);
  void ConvertOneArrayToTArray(TArray(PREC)* _to, uint _type, PREC _fitness, PREC _best, COUNT_T _iter, COUNT_T _index, PREC _shape_fitness);

  void PrepareLogging(TArray(PREC)* _params, TArray(PREC)* _velos, TArray(PREC)* _others, TArray(PREC)* _functionValue);
  void PrintTheBest();
  bool IsStopCondition();

protected:
  Context& ctx;
  ProblemFunctionProcessor& processor;

  COUNT_T swarm_size = 0;
  COUNT_T iteration = 0;
  COUNT_T calculation = 0;

  PREC* particle_params;
  PREC* particle_velocity;
  PREC* particle_best_params;
  PREC* particle_fitness;
  PREC* particle_best_fitness;
  PREC* particle_shape_fitness;


  void CreateParticles();
  void DestroyParticles();

  void Logging( uint _logLevel );
  void BestLogging();
  void ParticleLogging();


  PREC *last_best_fitnesses;

public:
  Swarm(Context& _ctx, ProblemFunctionProcessor& _processor);
  ~Swarm();

  Particle** particles;
  UniformRnd<PREC> random_Uniform;
  NormalRnd<PREC> random_Normal;

  PREC best_fitness = -1;
  PREC best_shape_fitness = 0;
  COUNT_T best_index = 0;
  PREC* best_params;
  PREC* best_velo;
  PREC* best_params_avg;
  PREC* best_evaluation_FunctionValue;

  void Initialize();
  void RunSimulation();

  void Evaluate();
  void Move();
  void CalculateBestSpark();

  COUNT_T GetIteration();
  COUNT_T GetCalculation();
  PREC GetBestFitness();
  PREC GetBestShapeFitness();
  PREC* GetBestParams();
  PREC* GetBestVelo();
  PREC* GetBestFunctionValue();
  COUNT_T GetLogItemCount() { return this->logItemCount; }
};
