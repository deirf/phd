#pragma once

#include "EFwa.h"

class EFwaDM : public EFwa
{
private:
  PREC* old_params = NULL;
  PREC* old_fitness = NULL;

  const PREC scaleFactor = 2.0f;
protected:
  void DifferentalMutation();
  void Evaluate();
  void BetterSelection();
  void SavePopAsOld();

  PREC* GetOldParams(COUNT_T i)
  {
    return &this->old_params[ i * this->spark_number ];
  }

public:
  EFwaDM(Context& _ctx, ProblemFunctionProcessor& _processor) : EFwa( _ctx, _processor )
  {
    this->old_params = new PREC[ this->max_spark_number * this->ctx.config.searching_dimension ]();
    this->old_fitness = new PREC[ this->max_spark_number ]();
  }

};
