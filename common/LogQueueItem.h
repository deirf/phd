#pragma once

#include <string>
#include "LArray.h"
#include "Types.h"
#include "LogQueue.h"

class LogQueueItem
{
private:
  uint fileindex;
  QueueType type;

public:
  TVLArray(PREC) arrays;

  TVArray(PREC) prec_records;
  TVArray(COUNT_T) count_records;

  TVector(COUNT_T) ints;
  std::string desc;

  LogQueueItem(uint _fileindex, QueueType _type);
  ~LogQueueItem();

  uint GetFileIndex() { return this->fileindex; }
  QueueType GetQueueType() { return this->type; }
};
