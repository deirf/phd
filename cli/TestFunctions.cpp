#include "TestFunctions.h"
#include "../common/Configuration.h"
#include "../ProblemFunctions_CPU/ReferenceFunctionCalculator1D.h"
#include "../ProblemFunctions_GPU/ProblemFunctionProcessorGPU.h"

#include "../common/Types.h"
#include "../common/ThreadPool.h"

void
TestFunctions::Reference_function_calculation(std::string& _config_file_name,
                                              DevFunctionParameters* _devFunctionParameters,
                                              uint _pfindex,
                                              uint _saindex)
{
  ThreadPools threadpools;
  Context ctx = Context(_config_file_name, _saindex, _pfindex, NULL, &threadpools, 0, "", 0);

  ctx.config.HTCFunctionValueFileName = _devFunctionParameters->htcValueFile;
  ctx.config.HTCFunctionTimeFileName = _devFunctionParameters->htcTimeFile;
  ctx.config.CoolingCurve.ReferenceFunctionFileName = _devFunctionParameters->coolingCurveTimeFile;

  ProblemFunctionProcessorGPU proc = ProblemFunctionProcessorGPU(ctx);
  calculate_reference_function(ctx, proc, _devFunctionParameters->outputFile);
}
