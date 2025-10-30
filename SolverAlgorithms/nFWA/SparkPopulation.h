#pragma once

#include <stdarg.h>
#include "../../common/Configuration.h"
#include "../../ProblemFunctions_CPU/ProblemFunctionProcessor.h"
#include "Spark.h"
#include "SparkHistory.h"
#include "SparkStore.h"
#include "../../common/Array.h"
#include "../../common/LArray.h"
#include "../../common/Types.h"
#include "../../common/LogWorker.h"
#include "Stats.h"
#include "BestSpark.h"

class Firework;
class Fireworks;

typedef struct
{
  COUNT_T index = 0;
  PREC fitness = -1;
} FoundBestSpark;


class SparkPopulation
{
private:
  Context& ctx;
  void InitializeBestSparks(TBestSpark* _sparks);

  void StateEvaluate();
  void FitnessEvaluate();

  template <typename SparkVector>
    bool FindSparkMinFitness(PREC* FitnessMinValue, COUNT_T* FitnessMinIndex, SparkVector* sparks);

  void PrintTheBest();
  void BestEvaluate();
  void CalculateBestIndex( COUNT_T* _bestRandomIndex, COUNT_T* _bestGaussianIndex, COUNT_T* _bestQuantumIndex );

  void SetNewFirework();

  template <typename SparkClass, typename SparkVector>
    SparkCalculationRepeatingState CalculateSpark(SparkClass* _spark);
  template <typename SparkVector>
    COUNT_T FindBestSpark(SparkVector* _sparks);
  void CalculateRandomSparks();
  void CalculateGaussianSparks();
  void CalculateFireworkSparks();
  void CalculateBestSpark();
  void CalculateQuantumSparks();

  void SelectNewBestSpark(COUNT_T _bestRandom, COUNT_T _bestGaussian, COUNT_T _bestQuantum);

  void Logging( uint _logLevel );
  void PrepareLogging();
  void PrepareBestLogging(const std::string& _desc);

  void SetSparkStore(SparkStore* _sparkstore) { this->sparkstore = _sparkstore; }

  void CalculateAllSpark(uint _state);

  void IncIteration() { this->iter++; }
  void SetZeroIteration() { this->iter = 0; }

  void IncCalculation( COUNT_T _count ) { this->calculation += _count; }

  COUNT_T max_number_of_sparks[ST_MAX] = { 0, 0, 0, 0, 0, 0 };

  COUNT_T iter = 0;
  COUNT_T fw_iter = 0;
  COUNT_T fw_index = 0;
  COUNT_T fw_pop_index = 0;

  COUNT_T calculation = 0;

  PREC* evaluation_data_store = NULL;
  PREC* evaluation_data_time = NULL;
  PREC* evaluation_fitness_store = NULL;
  PREC* evaluation_calculatedFunctionValue_store = NULL;
  PREC* evaluation_fitnessFunction_store = NULL;
  PREC* evaluation_fitnessFunction_time = NULL;

  Boundaries boundaries;

  SparkStore* sparkstore = NULL;
  LogWorker* logworker = NULL;
  uint logindex = 0;
  COUNT_T logItemCount = 0;

  Fireworks* fireworks = NULL;

  NormalRnd<PREC> randomNormal;
  UniformRnd<PREC> randomUniform;

  TRecord(PREC)* bestShapeParams = NULL;
  PREC bestShapeParamFitness = PREC_MAX;
  void SelectNewBestShapeParam();

  uint calculationState = SCS_UNKNOWN;
  void SetCalculationState(uint _state) { this->calculationState = _state; }
  uint GetCalculationState() { return this->calculationState; }

  uint CalculationSubState = SCSS_UNKNOWN;
  void SetCalculationSubState(uint _state) { this->CalculationSubState = _state; }
  uint GetCalculationSubState() { return this->CalculationSubState; }

  bool CheckStateToCalculate(Spark* _spark);
  void SetNextCalculationState(Spark* _spark);

  template <typename prec_type> TArray(prec_type)* ConvertBestRecordToArray(TRecord(prec_type)* _record);
  TLArray(PREC)* ConvertBestArrayToLArray(TArray(PREC)* _array);

  void SetMaxNumberOfBests(COUNT_T _num) { this->max_number_of_sparks[ST_BEST] = _num; }
  void SetMaxNumberOfFireworks(COUNT_T _num) { this->max_number_of_sparks[ST_FIREWORK] = _num; }
  void SetMaxNumberOfRandomSparks(COUNT_T _num) { this->max_number_of_sparks[ST_RANDOM] = _num; }
  void SetMaxNumberOfGaussianSparks(COUNT_T _num) { this->max_number_of_sparks[ST_GAUSSIAN] = _num; }
  void SetMaxNumberOfQuantumSparks(COUNT_T _num) { this->max_number_of_sparks[ST_QUANTUM] = _num; }

  void IncLogItemCount( COUNT_T _count ) { this->logItemCount += _count; }
  bool IsStopCondition();

protected:
  ProblemFunctionProcessor& processor;

public:
  SparkPopulation(Context& _ctx, ProblemFunctionProcessor& _processor) : ctx( _ctx ), processor( _processor ) {}
  ~SparkPopulation();

  void Initialize(Fireworks* _fireworks, const Boundaries& _boundaries, COUNT_T _fw_index, COUNT_T _fw_pop_index);
  void RunSimulation(COUNT_T _fw_iter, const Boundaries& _fw_boundaries);

  COUNT_T GetIteration();
  COUNT_T GetMaxIteration();
  PREC GetBestFitness();
  PREC GetBestShapeFitness();
  TRecord(PREC)* GetBestParam();
  TRecord(PREC)* GetBestShapeParams() { return this->bestShapeParams; }
  PREC GetBestShapeParamFitness() { return this->bestShapeParamFitness; }

  PREC* GetInitFireworkSparkHTCValues() { return this->ctx.config.IHCP.HTCInitValueFunction; }

  COUNT_T GetLogItemCount() { return this->logItemCount; }
  COUNT_T GetCalculation() { return this->calculation; }

  void BestSparkReset() { this->GetSparkStore()->GetBestSpark()->Reset(); }

  COUNT_T GetNumberOfRandomSparks() { return this->GetSparkStore()->GetRandomSparks()->getCount(); }
  COUNT_T GetNumberOfGaussianSparks() { return this->GetSparkStore()->GetGaussianSparks()->getCount(); }
  COUNT_T GetNumberOfQuantumSparks() { return this->GetSparkStore()->GetQuantumSparks()->getCount(); }
  COUNT_T GetNumberOfFireworkSparks() { return this->GetSparkStore()->GetFireworkSparks()->getCount(); }
  COUNT_T GetNumberOfBestSparks() { return this->GetSparkStore()->GetBestSparks()->getCount(); }

  COUNT_T GetNumberOfAllSparks() { return
                                         this->GetNumberOfRandomSparks() +
                                         this->GetNumberOfGaussianSparks() +
                                         this->GetNumberOfQuantumSparks() +
                                         this->GetNumberOfFireworkSparks() +
                                         this->GetNumberOfBestSparks();
                                 }

  COUNT_T GetNumberOfStoredSparks() { return this->GetNumberOfAllSparks(); }

  SparkStore* GetSparkStore() { return this->sparkstore; }
  Firework* GetFirework(COUNT_T _fw_index) { return this->fireworks->GetFirework(_fw_index); }
  COUNT_T GetFwIndex() { return this->fw_index; }
  Firework* GetFirework() { return this->GetFirework( this->GetFwIndex() ); }

  Spark* GetSpark(COUNT_T _recordIndex) { return this->GetSparkStore()->GetSpark(_recordIndex); }

  COUNT_T GetMaxNumberOfBests() { return this->max_number_of_sparks[ST_BEST]; }
  COUNT_T GetMaxNumberOfFireworks() { return this->max_number_of_sparks[ST_FIREWORK]; }
  COUNT_T GetMaxNumberOfRandomSparks() { return this->max_number_of_sparks[ST_RANDOM]; }
  COUNT_T GetMaxNumberOfGaussianSparks() { return this->max_number_of_sparks[ST_GAUSSIAN]; }
  COUNT_T GetMaxNumberOfQuantumSparks() { return this->max_number_of_sparks[ST_QUANTUM]; }

  COUNT_T GetMaxNumberOfAllSparks() { return
                                         this->GetMaxNumberOfRandomSparks() +
                                         this->GetMaxNumberOfGaussianSparks() +
                                         this->GetMaxNumberOfQuantumSparks() +
                                         this->GetMaxNumberOfFireworks() +
                                         this->GetMaxNumberOfBests();
                                    }

  int GetRandomDirection() { return randomUniform.getRandomDirection(); }
  PREC GetUniformRandom01() { return randomUniform.getRand01(); }
  PREC GetUniformRandom(PREC min, PREC max)  { return randomUniform.getRand(min, max); }

  PREC GetNormalRandom_0T1B()  { return randomNormal.getRand01_0T1B(); };
  PREC GetNormalRandom_1T0B()  { return randomNormal.getRand01_0B1T(); };
  PREC GetNormalRandom_0B1T2B()  { return randomNormal.getRand02_0B1T2B(); };
};
