#include "ProblemFunctionProcessorGPU.h"


ProblemFunctionProcessorGPU::ProblemFunctionProcessorGPU(Context& ctx) : ProblemFunctionProcessor(ctx)
{
  for (uint gpui = 0; gpui < ctx.config.Environment.GPU_N; gpui++)
  {
      ProblemFunctionCalculator *processor = InitProcessor(gpui);
      processors.emplace_back(processor);
  }
}
