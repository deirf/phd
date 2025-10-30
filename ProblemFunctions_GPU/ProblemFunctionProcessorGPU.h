#pragma once
#include "../ProblemFunctions_CPU/ProblemFunctionProcessor.h"


class ProblemFunctionProcessorGPU : public ProblemFunctionProcessor
{
public:
  explicit ProblemFunctionProcessorGPU(Context& ctx);
  ~ProblemFunctionProcessorGPU() {}
};
