#pragma once

#include <mutex>
#include <queue>
#include "Types.h"


template <typename T>
class LogQueue
{
private:
  std::queue<T> data_queue;
  std::mutex queue_mutex;

public:
  LogQueue() {}
  ~LogQueue() {}

  bool
  empty()
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    return this->data_queue.empty();
  }

  COUNT_T
  size() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    return this->data_queue.size();
  }

  void
  add(T& t)
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    this->data_queue.push(t);
  }

  bool
  remove(T& t)
  {
    std::unique_lock<std::mutex> lock(queue_mutex);

    if (this->data_queue.empty())
    {
      return false;
    }

    t = std::move(data_queue.front());

    this->data_queue.pop();
    return true;
  }
};
