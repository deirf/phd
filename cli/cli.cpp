#include <string>
#include <getopt.h>

#include "TestFunctions.h"
#include "../SolverAlgorithms/nFWA/nFwa.h"
#include "../SolverAlgorithms/oFWA/oFwaPopulation.h"
#include "../SolverAlgorithms/PSO/Swarm.h"
#include "CliStats.h"

#include "../common/Types.h"

static void
printHelp()
{
  printf("New Style of IHCP investigator v1.0\n");
  printf("Common options:\n");
  printf("-h               This help\n");
  printf("-p <alg name>    Problem function:\n");
  printf("                   TestFn: Sphere, Scwefel12, Rosenbrock, SAckley, GGriewank, RHEll, Rastigin, ALLTEST \n");
  printf("                   HCP: HCP, REF\n");
  printf("\n");
  printf("Researching options: (-p HCP)\n");
  printf("  -s <alg name>    Solver algorithm:\n");
  printf("                     FWA: FWA, nFWA, cFWA, AFWA, EFWA, EFWADM, ALLFWA\n");
  printf("                     PSO: PSO, PSOCo, PSOIn, QPSOT1, QPSOT2, ALLPSO\n");
  printf("  -a <config file> Config filename\n");
  printf("  -l <value>       Logging options:\n");
  printf("                     no:    Do not create log file\n");
  printf("                     best:  Log only the last best result\n");
  printf("                     bests: Log only the bests values\n");
  printf("                     all:   Log all data\n");
  printf("  -r <number>      Rerun the Solver alg. for calculating the best fitness statistics: mean, median, std.deviation, std.error, min, max\n");
  printf("\n");
  printf("Reference cooling curve calculation options: (-p REF)\n");
  printf("  -T <dat file>    Reference HTC Value data file\n");
  printf("  -t <dat file>    Reference HTC Time data file\n");
  printf("  -c <dat file>    Calculated cooling curve time points\n");
  printf("  -o <file>        Calculated cooling curve data file (output file)\n");
  printf("\n");
}

static uint
findInArray(const std::string* _list, uint _size, std::string& _to_find, int* error)
{
  *error = 0;
  for ( uint i = 0; i < _size; ++i )
  {
      if ( _list[ i ].compare( _to_find ) == 0 )
      {
          return i;
      }
  }
  *error = -1;
  return 0;
}

std::string
GetTimeAsString(const char* _format)
{
  time_t rawtime;
  struct tm* timeinfo;
  char buffer[256];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, sizeof(buffer), _format, timeinfo);
  std::string str(buffer);

  return str;
}

static CalculationResults*
runTheSimulation(const std::string& _config,
                 uint _saindex,
                 uint _pfindex,
                 uint _loglevel,
                 const std::string& _starttime,
                 uint _round
                )
{
  TestFunctions testFunc;
  CalculationResults* result = NULL;

  switch(_saindex)
  {
    case SA_nFWA:
      result = testFunc.SimulationProblemFunction<nFwa>(_config, _saindex, _pfindex, _loglevel, _starttime, _round);
      break;

    case SA_FWA:
    case SA_cFWA:
    case SA_AFWA:
    case SA_EFWA:
    case SA_EFWADM:
      result = testFunc.SimulationProblemFunction<oFwaPopulation>(_config, _saindex, _pfindex, _loglevel, _starttime, _round);
      break;

    case SA_PSO:
    case SA_PSOCo:
    case SA_PSOIn:
    case SA_QPSOT1:
    case SA_QPSOT2:
      result = testFunc.SimulationProblemFunction<Swarm>(_config, _saindex, _pfindex, _loglevel, _starttime, _round);
      break;

    default:
      DOES_NOT_REACH();;
  }

  return result;
}

static TResults*
run_one_test(const std::string& _config,
             uint _sa_index,
             uint _pf_index,
             uint _rerun,
             uint _loglevel,
             const std::string& _starttime
            )
{
  TResults* results = new TResults;
  CliStats stats;

  for (uint i = 0; i < _rerun; i++)
  {
    simutation_stop_running = FALSE;
    stats.printNameAndRoundStats(problem_funcsName[ _pf_index ], solver_algsName[ _sa_index ], i + 1);
    CalculationResults* result = runTheSimulation(_config, _sa_index, _pf_index, _loglevel, _starttime, i);
    results->push_back(result);
  }
  return results;
}

static void
run_test(TPFTestResults* _tfResults,
         const std::string& _config,
         uint _sa_index,
         uint _pf_index,
         uint _rerun,
         uint _logLevel,
         const std::string& _starttime
        )
{
  PFTestResults* testresult = new PFTestResults;
  testresult->result = run_one_test(_config, _sa_index, _pf_index, _rerun, _logLevel, _starttime);
  testresult->pf_name = problem_funcsName[ _pf_index ];
  testresult->sa_name = solver_algsName[ _sa_index ];

  _tfResults->push_back(testresult);
}

sig_atomic_t simutation_stop_running;

static void
sig_handler(int _signal)
{
    if (simutation_stop_running)
    {
      exit(_signal);
    }

    simutation_stop_running = TRUE;
}

int
main(int argc, char **argv)
{
  signal(SIGINT, sig_handler);
  simutation_stop_running = FALSE;

  setbuf(stdout, NULL);

  std::string config = "";

  std::string salg = "";
  std::string pfunc = "";
  std::string clogLevel = "";
  std::string starttime = GetTimeAsString("%Y%m%d%H%M%S");

  DevFunctionParameters devFunctionParameters;

  uint rerun = 1;

  int opt = 0;
  while ( ( opt = getopt(argc, argv, "a:hH:s:p:r:o:l:c:C:T:t:") ) != -1 )
  {
    switch( opt )
    {
      case 'l': // log level
        clogLevel.assign(optarg, strlen(optarg));
        break;

      case 's': // solver algo
        salg.assign(optarg, strlen(optarg));
        break;

      case 'p': // problem function
        pfunc.assign(optarg, strlen(optarg));
        break;

      case 'a': // config file
        config.assign(optarg, strlen(optarg));
        break;

      case 'o': // output file
        devFunctionParameters.outputFile.assign(optarg, strlen(optarg));
        break;

      case 'c': // reference cooling curve time data file
        devFunctionParameters.coolingCurveTimeFile.assign(optarg, strlen(optarg));
        break;

      case 'C': // reference cooling curve file
        devFunctionParameters.coolingCurveValueFile.assign(optarg, strlen(optarg));
        break;

      case 't': // HTC time file
        devFunctionParameters.htcTimeFile.assign(optarg, strlen(optarg));
        break;

      case 'T': // HTC values file
        devFunctionParameters.htcValueFile.assign(optarg, strlen(optarg));
        break;

      case 'r': // rerun
        rerun = (uint)atoi(optarg);
        break;

      case 'h': // help
      default :
        printHelp();
        return 0;
    }
  }

  if ( config == "" )
  {
    printf("Config file is mandatory!\n");
    printHelp();
    return 0;
  }

  int sa_error = 0;
  int pf_error = 0;
  int log_error = 0;
  uint loglevel = 0;
  uint sa_index = 0;

  uint pf_index = findInArray(problem_funcsName, ARRAY_SIZE(problem_funcsName), pfunc, &pf_error);
  if ( pf_error == -1 )
  {
    printf("Unknown problem function: %s\n", pfunc.c_str());
    printHelp();
    return 1;
  }
  printf("Problem function: %s\n", pfunc.c_str());

  switch(pf_index)
  {
    case PF_REF:
      if (
          (devFunctionParameters.htcTimeFile == "") ||
          (devFunctionParameters.htcValueFile == "") ||
          (devFunctionParameters.outputFile == "") ||
          (devFunctionParameters.coolingCurveTimeFile == "")
         )
      {
        printf("-t, -T, -c and -o is mandatory!\n");
        printHelp();
        return 1;
      }
      {
        TestFunctions testFunc;
        testFunc.Reference_function_calculation(config, &devFunctionParameters, pf_index, sa_index);
        return 0;
      }

    case PF_HCP:
    case PF_Sphere:
    case PF_Scwefel12:
    case PF_Rosenbrock:
    case PF_SAckley:
    case PF_GGriewank:
    case PF_RHEll:
    case PF_APHEll:
    case PF_Rastigin:
    case PF_ALLTEST:

      loglevel = findInArray(LogLevelName, ARRAY_SIZE(LogLevelName), clogLevel, &log_error);
      if ( log_error == -1 )
      {
        printf("Unknown loglevel: %s\n", clogLevel.c_str());
        printHelp();
        return -1;
      }
      if ( loglevel == 3 )
      {
        loglevel = 4;
      }

      sa_index = findInArray(solver_algsName, ARRAY_SIZE(solver_algsName), salg, &sa_error);
      if ( sa_error == -1 )
      {
        printf("Unknown solver algorithm: %s\n", salg.c_str());
        printHelp();
        return 1;
      }
      printf("Solver algorithm: %s\n", salg.c_str());
      break;

    default:
      DOES_NOT_REACH();
  }

  TPFTestResults tfResults;
  CliStats stats;

  stats.printStatsHeader();

  if ( pf_index == PF_ALLTEST )
  {
    for (uint pf = PF_Sphere; pf < PF_ALLTEST; pf++)
    {
      run_test(&tfResults, config, sa_index, pf, rerun, loglevel, starttime);
      stats.calulateAndPrintStats( &tfResults );
    }
  }
  else if ( sa_index == SA_ALLFWA)
  {
    for (uint sa = SA_FWA; sa < SA_FWAMAX; sa++)
    {
      run_test(&tfResults, config, sa, pf_index, rerun, loglevel, starttime);
      stats.calulateAndPrintStats( &tfResults );
    }
  }
  else if ( sa_index == SA_ALLPSO)
  {
    for (uint sa = SA_PSO; sa < SA_PSOMAX; sa++)
    {
      run_test(&tfResults, config, sa, pf_index, rerun, loglevel, starttime);
      stats.calulateAndPrintStats( &tfResults );
    }
  }
  else
  {
    run_test(&tfResults, config, sa_index, pf_index, rerun, loglevel, starttime);
    stats.calulateAndPrintStats( &tfResults );
  }

  tfResults.clear();
  return 0;
}
