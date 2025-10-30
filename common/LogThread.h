#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <string>
#include <limits.h>
#include <condition_variable>
#include "LogQueue.h"
#include "LogQueueItem.h"
#include "LogCreator.h"

class LogThread
{
private:
  TFilestreamList* filestream;
  std::mutex log_thread_mutex;
  std::mutex log_thread_filestream_mutex;
  std::thread consumer_thread;
  std::condition_variable consumer_condition;
  LogQueue<LogQueueItem*>& queue;

  bool stop = FALSE;
  bool wait = FALSE;
  LogCreator* logcreator = NULL;
  COUNT_T logItemCounter = 0;

  void IncLogItemCounter() { this->logItemCounter++; }
  void PrintItem(LogQueueItem* _item);

  void WriteToFile(char* _str, COUNT_T _len, uint _index);
  void Consumer();

public:
  explicit LogThread(LogQueue<LogQueueItem*>& _queue);
  ~LogThread();

  uint OpenFile(const std::string& _filename);
  void CloseFile(uint index);

  void AddToQueue(LogQueueItem* _item);
  void Flush();
  void setWait(bool _wait) { this->wait = _wait; }

  COUNT_T GetLogItemCount() { return this->logItemCounter; }
};
