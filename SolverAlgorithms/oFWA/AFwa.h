#pragma once

#include "EFwa.h"

class AFwa : public EFwa
{
private:

  const PREC lambda = 1.3f;
protected:

  void GenerateAmplitude();

public:
  AFwa(Context& _ctx, ProblemFunctionProcessor& _processor) : EFwa( _ctx, _processor ) {};
};
