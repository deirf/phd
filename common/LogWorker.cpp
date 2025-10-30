#include "LogWorker.h"

#include <string>
#include <stdarg.h>
#include "LogQueueItem.h"

void
LogWorker::send(const std::string& _desc, COUNT_T _spark_count, ...)
{
  create_arrays();

  va_list args;
  va_start(args, _spark_count);

  for(COUNT_T i = 0; i < this->prec_array_N; i++)
  {
    this->va_prec_arrays[i] = va_arg(args, TArray(PREC)*);
  }

  for(COUNT_T i = 0; i < this->count_array_N; i++)
  {
    this->va_count_arrays[i] = va_arg(args, TArray(COUNT_T)*);
  }

  for(COUNT_T i = 0; i < this->list_array_N; i++)
  {
    this->va_list_arrays[i] = va_arg(args, TLArray(PREC)*);
  }

  for(COUNT_T i = 0; i < this->int_N; i++)
  {
    this->va_ints[i] = va_arg(args, COUNT_T);
  }

  for (COUNT_T spark_index = 0; spark_index < _spark_count; spark_index++)
  {
    LogQueueItem* sparklog = new LogQueueItem(this->logindex, this->queuetype);
    sparklog->desc = _desc;

    for (COUNT_T larray_index = 0; larray_index < this->list_array_N; ++larray_index)
    {
      TArray(PREC)* array = va_list_arrays[ larray_index ]->getArrayPtr( spark_index );
      sparklog->arrays.push_back( std::move(array) );
    }

    for (COUNT_T array_index = 0; array_index < this->count_array_N; ++array_index)
    {
      TRecord(COUNT_T)* record = va_count_arrays[ array_index ]->getRecordPtr( spark_index );
      sparklog->count_records.push_back( std::move(record) );
    }

    for (COUNT_T record_index = 0; record_index < this->prec_array_N; ++record_index)
    {
      TRecord(PREC)* record = va_prec_arrays[ record_index ]->getRecordPtr( spark_index );
      sparklog->prec_records.push_back( std::move(record) );
    }

    for (COUNT_T int_index = 0; int_index < this->int_N; ++int_index)
    {
      COUNT_T value = va_ints[ int_index ];
      sparklog->ints.push_back( std::move(value) );
    }

    this->logthread->AddToQueue(sparklog);
  }
  this->logthread->Flush();

  va_end(args);

  free_arrays();

  delete this;
}

LogWorker::LogWorker(LogThread* _logthread,
                     QueueType _queuetype,
                     uint _logindex,
                     COUNT_T _prec_array_N,
                     COUNT_T _count_array_N,
                     COUNT_T _list_array_N,
                     COUNT_T _int_N
                    )
{
  this->prec_array_N = _prec_array_N;
  this->count_array_N = _count_array_N;
  this->list_array_N = _list_array_N;
  this->int_N = _int_N;
  this->logindex = _logindex;
  this->logthread = _logthread;
  this->queuetype = _queuetype;
}

void
LogWorker::create_arrays()
{
  if (this->prec_array_N > 0)
  {
    this->va_prec_arrays = new TArray(PREC)*[ this->prec_array_N ]();
  }

  if (this->count_array_N > 0)
  {
    this->va_count_arrays = new TArray(COUNT_T)*[ this->count_array_N ]();
  }

  if (this->list_array_N > 0)
  {
    this->va_list_arrays = new TLArray(PREC)*[ this->list_array_N ]();
  }

  if (this->int_N > 0)
  {
    this->va_ints = new COUNT_T[ this->int_N ]();
  }
}

void
LogWorker::free_arrays()
{
  if (this->list_array_N > 0)
  {
    delete[] this->va_list_arrays;
  }

  if (this->prec_array_N > 0)
  {
    delete[] this->va_prec_arrays;
  }

  if (this->count_array_N > 0)
  {
    delete[] this->va_count_arrays;
  }

  if (this->int_N > 0)
  {
    delete[] this->va_ints;
  }
}
