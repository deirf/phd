#pragma once

#include "Firework.h"
#include "../../common/Types.h"

class Fireworks;

class FireworkPopulation
{
private:
  COUNT_T fw_pop_index = 0;
  COUNT_T fw_iter = 0;
  COUNT_T best_fw_index = 0;

  TArray(PREC)* avgParams = NULL;

  Boundaries boundaries;

  Fireworks* fireworks = NULL;
  uint logindex = 0;
  COUNT_T logItemCount = 0;

  template <typename ArraType> Array<ArraType>* CreateArray(COUNT_T _number_of_data, COUNT_T _number_of_array);
  void DestroyFireworks();
  void CreateFireworks();
  void PrepareBoundaries();

  void Evaluate();
  void SelectTheBestFirework();

  void IncIteration() { this->fw_iter++; }
  void SetZeroIteration() { this->fw_iter = 0; }
  COUNT_T GetMaxIteration() { return this->boundaries.fw_max_iter; }
  void PrintTheBest();

  void Logging( uint _logLevel );
  void PrepareBestLogging();
  TLArray(PREC)* ConvertBestArrayToLArray(TArray(PREC)* _array);
  template <typename prec_type> TArray(prec_type)* ConvertBestRecordToArray(TRecord(prec_type)* _record);

  void CheckIterations();

  COUNT_T GetBestFireworkIndex() { return this->best_fw_index; }
  void SetBestFireworkIndex( COUNT_T _index ) { this->best_fw_index = _index; }

  void IncLogItemCount( COUNT_T _count ) { this->logItemCount += _count; }
  COUNT_T GetAllFWLogItemCount();

  bool IsStopCondition();
  void CalculateAVGShapeParams();

protected:
  Context& ctx;
  ProblemFunctionProcessor& processor;

public:
  FireworkPopulation(Context& _ctx, ProblemFunctionProcessor& _processor) : ctx( _ctx ), processor ( _processor ) {}
  ~FireworkPopulation()
  {
    this->DestroyFireworks();
    delete avgParams;
  }

  void Initialize(COUNT_T _fw_pop_index, const Boundaries& _boundaries);
  void RunSimulation();

  Firework* GetFirework(COUNT_T _index);
  Fireworks* GetFireworks() { return this->fireworks; }

  COUNT_T GetBestCalculation();
  COUNT_T GetAllCalculations();
  COUNT_T GetIteration() { return this->fw_iter; }
  PREC GetBestFitness();
  PREC GetBestShapeFitness();
  TRecord(PREC)* GetBestParam();

  COUNT_T GetFireworkCount() { return this->boundaries.fw_count; }
  COUNT_T GetLogItemCount() { return this->logItemCount + this->GetAllFWLogItemCount(); }
};
