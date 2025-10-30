#pragma once

#include "../../common/Types.h"
#include "../../common/LogWorker.h"
#include "../../common/Configuration.h"
#include "../../ProblemFunctions_CPU/ProblemFunctionProcessor.h"

class oSpark;

class oFwa
{
private:

protected:
  PREC min_param = 0;
  PREC max_param = 0;
  COUNT_T param_N = 0;

  COUNT_T max_spark_iteration = 0;
  COUNT_T max_firework_iteration = 0;
  COUNT_T firework_iteration = 0;
  PREC min_fitness = 0.1f;
  COUNT_T max_spark_number = 50;
  COUNT_T min_spark_number = 5;
  COUNT_T spark_number = 50;

  COUNT_T calculation = 0;
  COUNT_T iteration = 0;
  COUNT_T FunctionEvaluations = 0;
  COUNT_T fwa_index = 0;

  COUNT_T logItemCount = 0;
  uint logindex =0;

  PREC* spark_params = NULL;
  PREC* spark_fitness = NULL;
  PREC* direction = NULL;
  PREC* shape_fitness = NULL;

  oFwa** oFwas = NULL;
  PREC best_firework_fitness = PREC_MAX;

  PREC* distances = NULL;
  PREC* sumDistances = NULL;
  PREC* probability = NULL;

  PREC* evaluation_FunctionValue_store = NULL;
  PREC* evaluation_FitnessFunction_store = NULL;

  oSpark** sparks = NULL;

  PREC best_fitness = PREC_MAX;
  PREC worst_fitness = 0;
  PREC best_amplitude = 0;
  COUNT_T best_index = 0;
  PREC* best_params = NULL;
  PREC* best_functionValue = NULL;

  UniformRnd<PREC> randomUniform;
  NormalRnd<PREC> randomNormal;
  NormalRnd<PREC> randomNormal11{1,1};

  bool finite(PREC f) { return ( std::isnormal(f) || !std::isnan(f) ); }
  oSpark* CreateOneSpark(COUNT_T i);
  void CreateSparks();
  void DestroySparks();
  void GenerateRandomSparkParams(COUNT_T _index);
  void GenerateSparks();
  void CalculateMinMaxFitness();
  void GenerateAmplitude();
  void BestEvaluate();
  void BatchEvaluate();
  void GenerateNextGeneration();
  void MoveSparks();
  void GaussianMutation();
  void SparkSelections();
  PREC CalculateDistance(const PREC* _A, const PREC* _B);
  void CalculateDistances();
  void CalculateProbability();
  void CalculateSparkNumber();
  void FitnessPostProcessing();
  void SelectBestFwaFitness();

  void IncLogItemCount( COUNT_T _count ) { this->logItemCount += _count; }

  void Logging( uint _logLevel );
  void SparkLogging();
  void BestLogging();
  oSpark* GetSpark( COUNT_T _index ) { return this->sparks[ _index ]; }
  void SetSpark( COUNT_T _index, oSpark* _spark ) { this->sparks[ _index ] = _spark; }

  void PrepareLogging(TArray(PREC)* _params, TArray(PREC)* _others, TArray(PREC)* _functionValue);
  void ConvertOneOtherArrayToTArray(TArray(PREC)* _others, COUNT_T _index, COUNT_T _type);
  void ConvertOneArrayToTArray(TArray(PREC)* _to, PREC* _from, COUNT_T _len);
  TArray(PREC)* ConvertParamsArrayToTArray();
  TArray(PREC)* ConvertOthersArrayToTArray();
  TArray(PREC)* ConvertFunctionValueArrayToTArray();

  TArray(PREC)* ConvertBestParamsArrayToTArray();
  TArray(PREC)* ConvertBestOthersArrayToTArray();
  TArray(PREC)* ConvertBestFunctionValueArrayToTArray();

  void PrintTheBest();

  Context& ctx;
  ProblemFunctionProcessor& processor;

  uint GetProplemFunction() { return this->ctx.pf_index; }
  int GetRandomDirection() { return randomUniform.getRandomDirection(); }
  bool GetRandomCoinSide() { return randomUniform.getRandomCoinSide(); }
  PREC GetUniformRandom01() { return randomUniform.getRand01(); }
  PREC GetUniformRandom(PREC min, PREC max)  { return randomUniform.getRand(min, max); }

  PREC GetNormalRandom()  { return randomNormal.getRand(); };
  PREC GetNormalRandom11()  { return randomNormal11.getRand(); };
  PREC GetNormalRandom_0T1B()  { return randomNormal.getRand01_0T1B(); };
  PREC GetNormalRandom_1T0B()  { return randomNormal.getRand01_0B1T(); };
  PREC GetNormalRandom_0B1T2B()  { return randomNormal.getRand02_0B1T2B(); };
  bool IsStopCondition();

public:
  oFwa(Context& _ctx, ProblemFunctionProcessor& _processor);

  virtual ~oFwa();
  virtual void Initialize( oFwa** _Fwas, COUNT_T _index );
  virtual void RunSimulation(COUNT_T _firework_iteration);

  void Evaluate();
  COUNT_T GetCalculation();
  COUNT_T GetIteration();
  PREC GetBestFitness();
  PREC* GetBestParams();
  COUNT_T GetBestIndex();
  PREC GetBestAmplitude();
  COUNT_T GetLogItemCount() { return this->logItemCount; }
  PREC GetBestShapeFitness();
  PREC* GetBestFunctionValue();
};
