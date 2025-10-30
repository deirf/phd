
#include <stdio.h>
#include <stdlib.h>

#include "ReferenceFunctionCalculator1D.h"
#include "../common/Configuration.h"
#include "../common/Types.h"


static uint
read_htc_times(Configuration& config)
{
  uint M;
  uint N;

  config.IHCP.HTCTimeFunction = ConfigurationLoader::LoadArrayFromFile(config.HTCFunctionTimeFileName, config, N, M);
  return N;
}

static uint
read_htc_values(Configuration& config, PREC** _htc_values)
{
  uint M;
  uint N;

  *_htc_values = ConfigurationLoader::LoadArrayFromFile(config.HTCFunctionValueFileName, config, N, M);
  return N;
}

static uint
read_reference_function(Configuration& config)
{
  uint M;
  uint N;

  config.CoolingCurve.reference_function_times = ConfigurationLoader::LoadArrayFromFile(config.CoolingCurve.ReferenceFunctionFileName, config, N, M);
  assume(N > 0, "Count of reference function value is 0");
  config.CoolingCurve.reference_function_end_time = config.CoolingCurve.reference_function_times[ N - 1 ];

  return N;
}

static void
write_result(Configuration& config, PREC* _HeatFunctionValues, const std::string& _outputFile)
{
  FILE* result_file = fopen(ConfigurationLoader::ResolveTemplate(_outputFile, config).c_str(), "w");

  for (COUNT_T k = 0; k < config.CoolingCurve.reference_function_N; k++)
  {
    fprintf(
            result_file, PREC_REF_FORMAT " " PREC_REF_FORMAT "\n",
            config.CoolingCurve.reference_function_times[ k ],
            _HeatFunctionValues[ k ]
           );
  }

  fclose(result_file);
}

void
calculate_reference_function(Context& _ctx, ProblemFunctionProcessor& processor, const std::string& _outputFile)
{
  PREC* htc_values = NULL;
  Configuration& config = _ctx.config;
  config.IHCP.searching_dimension = read_htc_times(config);
  uint dim2 = read_htc_values(config, &htc_values);
  assume( config.IHCP.searching_dimension == dim2, "HTC values and Dim as not as same' dim='%d', htc count='%d'", config.IHCP.searching_dimension, dim2 );

  processor.DeInitializeHeatParams();
  config.CoolingCurve.reference_function_N = read_reference_function(config);
  processor.InitializeHeatParams();

  PREC* HeatFunctionValues = static_cast<PREC*>( _malloc( (config.CoolingCurve.reference_function_N ) * sizeof(PREC) ) );
  processor.CalculateTargetFunction(htc_values, HeatFunctionValues);

  write_result(config, HeatFunctionValues, _outputFile);

  _free(htc_values);
  _free(HeatFunctionValues);
}
