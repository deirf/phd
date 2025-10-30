#include "Firework.h"
#include "SparkPopulation.h"

void
Firework::Initialize(Fireworks* _fireworks, COUNT_T _fw_index, COUNT_T _fw_pop_index, const Boundaries& _boundaries)
{
  this->SetSparkPopulation ( new SparkPopulation(this->ctx, this->processor) );
  this->GetSparkPopulation()->Initialize(_fireworks, _boundaries, _fw_index, _fw_pop_index);
  this->CreateBestSpark();
  this->CreateWorkerSparks();

  this->fw_index = _fw_index;
}

void
Firework::CreateWorkerSparks()
{
  this->GetSparkStore()->CreateWorkerSparks(
                                            this->GetSparkPopulation()->GetMaxNumberOfFireworks(),
                                            this->GetSparkPopulation()->GetMaxNumberOfRandomSparks(),
                                            this->GetSparkPopulation()->GetMaxNumberOfGaussianSparks(),
                                            this->GetSparkPopulation()->GetMaxNumberOfQuantumSparks()
                                           );
}

void
Firework::CreateBestSpark()
{
  this->GetSparkStore()->CreateBestSpark( this->GetSparkPopulation()->GetMaxNumberOfBests() );
}

void
Firework::KillWorkerSparks()
{
  this->GetSparkStore()->KillWorkerSparks();
}

void
Firework::KillBestSpark()
{
  this->GetSparkStore()->KillBestSpark();
}

Firework::~Firework()
{
  delete this->GetSparkPopulation();
}

void
Firework::RunSimulation(COUNT_T _fw_iter, const Boundaries& _boundaries)
{
  this->GetSparkPopulation()->RunSimulation(_fw_iter, _boundaries);
}

PREC
Firework::GetBestShapeParamFitness()
{
  return this->GetSparkPopulation()->GetBestShapeParamFitness();
}

TRecord(PREC)*
Firework::GetBestShapeParams()
{
  return this->GetSparkPopulation()->GetBestShapeParams();
}

PREC
Firework::GetBestFitness()
{
  return this->GetSparkPopulation()->GetBestFitness();
}

PREC
Firework::GetBestShapeFitness()
{
  return this->GetSparkPopulation()->GetBestShapeFitness();
}

TRecord(PREC)*
Firework::GetBestParam()
{
  return this->GetSparkPopulation()->GetBestParam();
}
