#pragma once
#include <string>
#include <time.h>

#include "../common/Configuration.h"
#include "../ProblemFunctions_GPU/ProblemFunctionProcessorGPU.h"

#include "../common/LogQueue.h"
#include "../common/LogQueueItem.h"
#include "../common/Types.h"
#include "../common/LogThread.h"
#include "../common/ThreadPool.h"


class TestFunctions
{
private:
  template<class SolverAlgorithmClass>
  CalculationResults* GetCalculationResults(SolverAlgorithmClass* _solverAlgorithm, const long* _ts, const std::string& _msg);

  long to_usec(const struct timespec& ts) { return (long int)((double)ts.tv_sec * 1e6 + (double)ts.tv_nsec * 1e-3); }

public:
  template<class SolverAlgorithmClass>
  CalculationResults* SimulationProblemFunction(const std::string& _config_file_name,
                                                uint _saindex,
                                                uint _pfindex,
                                                uint _logLevel,
                                                const std::string& _starttime,
                                                uint _round
                                               );

  void Reference_function_calculation(std::string& _config,
                                      DevFunctionParameters* _devFunctionParameters,
                                      uint _pfindex,
                                      uint _saindex
                                     );
};

template<class SolverAlgorithmClass>
CalculationResults*
TestFunctions::GetCalculationResults(SolverAlgorithmClass* _solverAlgorithm, const long* _ts, const std::string& _msg)
{
  CalculationResults* result = new CalculationResults;

  result->fitness = _solverAlgorithm->GetBestFitness();
  result->shapefitness = _solverAlgorithm->GetBestShapeFitness();
  result->iteration = _solverAlgorithm->GetIteration();
  result->calculation = _solverAlgorithm->GetCalculation();
  result->msg = _msg;
  result->ts = *_ts;

  return result;
}

template<class SolverAlgorithmClass>
CalculationResults*
TestFunctions::SimulationProblemFunction(const std::string& _config_file_name,
                                         uint _saindex,
                                         uint _pfindex,
                                         uint _loglevel,
                                         const std::string& _starttime,
                                         uint _round
                                        )
{
  struct timespec ts;
  long clock_start = -1;
  long clock_finish = -1;
  long elapsed_ts = -1;

  LogQueue<LogQueueItem*> logqueue;
  LogThread* logthread = new LogThread(logqueue);

  ThreadPools threadpools;
  threadpools.LogWorkerThreadpool = new ThreadPool(THREADPOOL_SIZE);
  threadpools.ProblemFunctionThreadpool = new ThreadPool(THREADPOOL_SIZE);
  threadpools.SolverFunctionThreadpool = new ThreadPool(THREADPOOL_SIZE);

  while ( clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == -1 ) { }
  clock_start = to_usec(ts);

  Context ctx = Context(_config_file_name,
                        _saindex,
                        _pfindex,
                        logthread,
                        &threadpools,
                        _loglevel,
                        _starttime,
                        _round
                       );

  ProblemFunctionProcessorGPU proc = ProblemFunctionProcessorGPU(ctx);
  SolverAlgorithmClass SolverAlgorithm = SolverAlgorithmClass(ctx, proc);

  SolverAlgorithm.Initialize();
  SolverAlgorithm.RunSimulation();

  while ( clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == -1 ) { }
  clock_finish = to_usec(ts);

  logthread->setWait( TRUE );

  COUNT_T last = 0;
  uint progress_counter = 0;
  std::string msg = "";
  while ( SolverAlgorithm.GetLogItemCount() != (last = logthread->GetLogItemCount()) )
  {
    printf("\r\033[%dCWaiting for logs: %ld%*c\r", PRINT_STAT_LOG,
           SolverAlgorithm.GetLogItemCount() - logthread->GetLogItemCount(),
           15, ' '
          );

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // there is no progress within (LOGGING_PROGRESS_COUNTER_MAX + 1) * 500ms
    if ( last == logthread->GetLogItemCount() )
    {
      if ( progress_counter < LOGGING_PROGRESS_COUNTER_MAX )
      {
        progress_counter++;
        continue;
      }

      msg = "missing logs: " +  std::to_string(SolverAlgorithm.GetLogItemCount() - logthread->GetLogItemCount());
      break;
    }
    else
    {
      progress_counter = 0;
    }
  }
  printf("\r\033[%dC%*c\r", PRINT_STAT_LOG, 30, ' ' );

  logthread->setWait( FALSE );

  delete threadpools.ProblemFunctionThreadpool;
  delete threadpools.SolverFunctionThreadpool;
  delete threadpools.LogWorkerThreadpool;
  delete logthread;

  elapsed_ts = clock_finish - clock_start;

  return this->GetCalculationResults<SolverAlgorithmClass>(&SolverAlgorithm, &elapsed_ts, msg);
}
