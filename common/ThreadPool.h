#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include "Types.h"

#define THREADPOOL(func, this, threadpool, type, ...) threadpool->enqueue(&func, type, this, __VA_ARGS__);
#define THREADPOOL_WA(func, this, threadpool, type) threadpool->enqueue(&func, type, this);

class ThreadPool;



typedef struct _threadpools
{
  ThreadPool* LogWorkerThreadpool;
  ThreadPool* ProblemFunctionThreadpool;
  ThreadPool* SolverFunctionThreadpool;
} ThreadPools;

typedef struct _threaddata
{
    ThreadType type;
    ThreadStatus status;
} ThreadData;

typedef struct _taskdata
{
    ThreadType type;
    std::function< void() > func;
} TaskData;


class ThreadPool
{
private:
    std::vector< std::thread > workers;
    std::deque< TaskData > tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic_bool stop;

    ThreadData* threaddata = NULL;
    COUNT_T thread_count = 0;
    COUNT_T thread_run_count = 0;
    COUNT_T dynamic_thread_count_max = 0;

public:
    explicit ThreadPool(COUNT_T _threads);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, ThreadType _type, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    void wait_for_empty();
    COUNT_T get_size();

    COUNT_T working_count(ThreadType _type);
    COUNT_T waiting_count(ThreadType _type);

    COUNT_T working_count();
    COUNT_T waiting_count();

    void add_new_threads(COUNT_T _new_threads);
    void add_one_thread(COUNT_T _thread_index);

    ThreadType get_thread_type(COUNT_T _thread_index);
    ThreadStatus get_thread_status(COUNT_T _thread_index);
    ThreadType get_task_type(COUNT_T _task_index);
};

// add new work item to the pool
template<class F, class... Args> auto
ThreadPool::enqueue(F&& f, ThreadType _type, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if (this->stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        TaskData taskdata { _type, [task](){ (*task)(); } };
        this->tasks.emplace_back(taskdata);
    }
    this->condition.notify_all();
    return res;
}
