#pragma once

#include "Types.h"
#include "LogThread.h"
#include "Array.h"
#include "LArray.h"

class LogWorker
{
private:
  LogThread* logthread = NULL;
  QueueType queuetype = QT_UNKNOWN;
  uint logindex = 0;
  COUNT_T prec_array_N = 0;
  COUNT_T count_array_N = 0;
  COUNT_T list_array_N = 0;
  COUNT_T int_N = 0;
  TLArray(PREC)** va_list_arrays = NULL;
  TArray(PREC)** va_prec_arrays = NULL;
  TArray(COUNT_T)** va_count_arrays = NULL;
  COUNT_T* va_ints = NULL;

protected:
public:
  LogWorker(LogThread* _logthread,
            QueueType _queuetype,
            uint _logindex,
            COUNT_T _count_array_N,
            COUNT_T _prec_array_N,
            COUNT_T list_array_N,
            COUNT_T _int_N
           );
  ~LogWorker() {}

  void send(const std::string& _desc, COUNT_T _spark_count, ...);
  void create_arrays();
  void free_arrays();
};
