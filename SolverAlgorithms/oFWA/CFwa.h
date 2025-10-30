#pragma once

#include "oFwa.h"

class CFwa : public oFwa
{
protected:
  void MoveSparks();

public:
  CFwa(Context& _ctx, ProblemFunctionProcessor& _processor) : oFwa( _ctx, _processor ) {};
};
