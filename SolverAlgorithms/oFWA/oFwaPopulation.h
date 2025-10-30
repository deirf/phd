#pragma once

#include "../../ProblemFunctions_CPU/ProblemFunctionProcessor.h"
#include "oFwa.h"
#include "CFwa.h"
#include "AFwa.h"
#include "EFwa.h"
#include "EFwaDM.h"

class oFwaPopulation
{
private:
  COUNT_T max_firework_number = 0;
  COUNT_T iteration = 0;
  COUNT_T max_iteration;
  COUNT_T param_N;
  COUNT_T evaluateCount = 0;
  PREC min_fitness = 0;

  uint logindex = 0;
  PREC* best_params = NULL;
  PREC best_fitness = PREC_MAX;
  COUNT_T best_index = 0;
  COUNT_T calculation = 0;
  COUNT_T logItemCount = 0;

  PREC* fwas_params;
  PREC* fwas_fitness;
  COUNT_T* fwas_index;

  oFwa** Fwas;

  void CreateFwas();
  void DestroyFwas();
  void SetWinnerFWA();
  void PrintTheBest();

  void ConvertOneArrayToTArray(TArray(PREC)* _to, PREC* _from, COUNT_T _len);
  TArray(PREC)* ConvertBestParamsArrayToTArray();
  void ConvertOneOtherArrayToTArray(TArray(PREC)* _others);
  TArray(PREC)* ConvertBestOthersArrayToTArray();
  TArray(PREC)* ConvertBestFunctionValueArrayToTArray();

  void PrepareLogging(TArray(PREC)* _params, TArray(PREC)* _others, TArray(PREC)* _functionValue);
  void BestLogging();
  void Logging( uint _logLevel );
  bool IsStopCondition();
  void IncLogItemCount( COUNT_T _count ) { this->logItemCount += _count; }

protected:
  Context& ctx;
  ProblemFunctionProcessor& processor;

public:
  oFwaPopulation(Context& _ctx, ProblemFunctionProcessor& _processor);
  virtual ~oFwaPopulation();

  virtual void Initialize();
  virtual void RunSimulation();

  void Evaluate();

  COUNT_T GetCalculation( COUNT_T index );
  COUNT_T GetCalculation();
  COUNT_T GetIteration( COUNT_T index );
  COUNT_T GetIteration();
  COUNT_T GetLogItemCount( COUNT_T index );
  COUNT_T GetLogItemCount();

  PREC GetBestAmplitude( COUNT_T index );
  PREC GetBestAmplitude();
  PREC GetBestFitness( COUNT_T index );
  PREC GetBestFitness();
  PREC GetBestShapeFitness( COUNT_T index );
  PREC GetBestShapeFitness();
  PREC* GetBestParams( COUNT_T index );
  PREC* GetBestParams();
  COUNT_T GetBestIndex( COUNT_T index );
  COUNT_T GetBestIndex();
  PREC* GetBestFunctionValue( COUNT_T index );
  PREC* GetBestFunctionValue();

};
