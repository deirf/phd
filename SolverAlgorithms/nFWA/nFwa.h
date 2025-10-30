#pragma once

#include "../../ProblemFunctions_CPU/ProblemFunctionProcessor.h"
#include "FireworkPopulation.h"

class nFwa
{
private:
  FireworkPopulation* firework_population = NULL;
  Boundaries boundaries;

protected:
  Context& ctx;
  ProblemFunctionProcessor& processor;

public:
  nFwa(Context& _ctx, ProblemFunctionProcessor& _processor);
  ~nFwa();

  void Initialize();
  void RunSimulation();

  PREC GetBestFitness();
  PREC GetBestShapeFitness();
  TRecord(PREC)* GetBestParam();
  COUNT_T GetIteration();
  COUNT_T GetCalculation();
  COUNT_T GetLogItemCount() { return this->firework_population->GetLogItemCount(); }
  FireworkPopulation* GetFireworkPopulation() { return this->firework_population; }
  Boundaries* GetBoundaries() { return &this->boundaries; }
};
