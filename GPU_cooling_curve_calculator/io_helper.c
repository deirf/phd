#include "gpu.h"
#include <string>

static void
read_htc(Config* config, std::string time, std::string values)
{
  FILE* src_htc_time = fopen(time.c_str(), "r");
  FILE* src_htc_value = fopen(values.c_str(), "r");

  int line_count = 0;
  while ( line_count != config->htc_N )
  {
    size_t len = 0;
    char* line = NULL;
    int res;

    res = getline(&line, &len, src_htc_value);
    if (res == -1)
    {
      printf("short htc values\n");
    }
    config->htc_values[ line_count ] = (PREC)atof(line);

    res = getline(&line, &len, src_htc_time);
    if (res == -1)
    {
      printf("short htc values\n");
    }
    config->htc_times[ line_count ] = (PREC)atof(line);

    line_count++;
  }

  config->end_time = config->htc_times[ line_count - 1 ];
  fclose(src_htc_value);
  fclose(src_htc_time);
}

static void
reproduce_cooling_curve(Config* config)
{
  const PREC time_step = config->end_time / (config->cooling_curve_N - 1);
  for (int k = 0; k < config->cooling_curve_N - 1; k++)
  {
    config->cooling_curve_times[k] = k * time_step;
  }
  config->cooling_curve_times[config->cooling_curve_N - 1] = config->end_time;
}

static void
reproduce_htc_values(Config* config, int _number_of_curve)
{
  for (int i = 1; i < _number_of_curve; i++)
  {
    for ( int t = 0; t < config->htc_N; t++)
    {
      config->htc_values[ i * config->htc_N + t ] = config->htc_values[ t ];
    }
  }
}

static void
write_result(Config* config, int _number_of_curve)
{
  FILE* result_file = fopen("./test_data/gpu_result.dat", "w");

  for (int k = 0; k < config->cooling_curve_N; k++)
  {
    fprintf(result_file, "%f", config->cooling_curve_times[k]);
    for (int t = 0; t < _number_of_curve; t++)
    {
      fprintf(result_file, " %f", config->cooling_curve_values[ k + t * config->cooling_curve_N ]);
    }
    fprintf(result_file, "\n");
  }
  fclose(result_file);
}