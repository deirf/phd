#pragma once

#include "oFwa.h"

class EFwa : public oFwa
{
protected:
  const PREC amplitudo_final = EPSILON;

  void GenerateAmplitude();
  void GaussianMutation();
  void SparkSelections();

public:
  EFwa(Context& _ctx, ProblemFunctionProcessor& _processor) : oFwa( _ctx, _processor ) {};
};
