#pragma once

#include "../../common/Array.h"
#include "../../common/Types.h"
#include "../../common/Configuration.h"
#include "../../ProblemFunctions_CPU/ProblemFunctionProcessor.h"
#include "SparkPopulation.h"

class Fireworks;

class Firework
{
protected:
  Context& ctx;
  ProblemFunctionProcessor& processor;
  SparkPopulation* sparkpopulation = NULL;
  COUNT_T fw_index = 0;

public:
  Firework(Context& _ctx, ProblemFunctionProcessor& _processor) : ctx( _ctx ), processor( _processor ) {}
  ~Firework();

  void Initialize(Fireworks* _fireworks, COUNT_T _fw_index, COUNT_T _fw_pop_index, const Boundaries& _boundaries);
  void RunSimulation(COUNT_T _fw_iter, const Boundaries& _boundaries);

  COUNT_T GetIteration() { return this->GetSparkPopulation()->GetIteration(); }
  COUNT_T GetCalculation() { return this->GetSparkPopulation()->GetCalculation(); }
  PREC GetBestFitness();
  PREC GetBestShapeFitness();
  TRecord(PREC)* GetBestParam();

  PREC GetBestShapeParamFitness();
  TRecord(PREC)* GetBestShapeParams();

  TRandomSpark* GetRandomSparks() { return this->GetSparkStore()->GetRandomSparks(); }
  TGaussianSpark* GetGaussianSparks() { return this->GetSparkStore()->GetGaussianSparks(); }
  TFireworkSpark* GetFireworkSparks() { return this->GetSparkStore()->GetFireworkSparks(); }
  TBestSpark* GetBestSparks() { return this->GetSparkStore()->GetBestSparks(); }
  TQuantumSpark* GetQuantumSparks() { return this->GetSparkStore()->GetQuantumSparks(); }

  BestSpark* GetBestSpark() { return this->GetBestSparks()->getData(BEST_SPARK_INDEX); }
  RandomSpark* GetRandomSpark(COUNT_T _index) { return this->GetRandomSparks()->getData(_index); }
  GaussianSpark* GetGaussianSpark(COUNT_T _index) { return this->GetGaussianSparks()->getData(_index); }
  FireworkSpark* GetFireworkSpark() { return this->GetFireworkSparks()->getData(FIREWORK_SPARK_INDEX); }
  QuantumSpark* GetQuantumSpark(COUNT_T _index) { return this->GetQuantumSparks()->getData(_index); }

  SparkStore* GetSparkStore() { return this->GetSparkPopulation()->GetSparkStore(); }
  SparkPopulation* GetSparkPopulation() { return this->sparkpopulation; }
  void SetSparkPopulation( SparkPopulation* _sparkpopulation ) { this->sparkpopulation = _sparkpopulation; }

  COUNT_T GetLogItemCount() { return this->GetSparkPopulation()->GetLogItemCount(); }

  void KillBestSpark();
  void KillWorkerSparks();

  void CreateWorkerSparks();
  void CreateBestSpark();
};
