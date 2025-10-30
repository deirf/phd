#include "ThreadPool.h"


void
ThreadPool::add_new_threads(COUNT_T _new_threads)
{
    const COUNT_T new_thread_count = this->thread_count + _new_threads;
    if ( _new_threads > 0 )
    {
        this->threaddata = static_cast<ThreadData*>( realloc(this->threaddata, new_thread_count * sizeof(ThreadData)) );
    }

    for(COUNT_T thread_index = 0; thread_index < new_thread_count; ++thread_index)
    {
        add_one_thread(thread_index);
    }

    this->thread_count = new_thread_count;
}

void
ThreadPool::add_one_thread(COUNT_T _thread_index)
{
    this->workers.emplace_back(
        [this, _thread_index]
        {
            this->threaddata[_thread_index] = { THREADTYPE_EMPTY , TS_SLEEP };

            for(;;)
            {
                TaskData task;

                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });

                    if (this->stop)
                        return;

                    task = std::move(this->tasks.front());
                    this->tasks.pop_front();

                    this->threaddata[_thread_index] = { task.type , TS_RUN };
                    this->thread_run_count++;
                }

                task.func();

                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->thread_run_count--;
                    this->threaddata[_thread_index] = { THREADTYPE_EMPTY , TS_SLEEP };
                }

                this->condition.notify_all();
            }
        }
    );
}

// the constructor just launches some amount of workers
ThreadPool::ThreadPool(COUNT_T _threads)
{
    this->stop = FALSE;
    this->thread_count = _threads;
    this->thread_run_count = 0;
    this->threaddata = new ThreadData[this->thread_count]();

    this->add_new_threads(0); // create threads in the pool

    for(std::thread& worker: this->workers)
        worker.detach();
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        this->stop = TRUE;

        while ( !this->tasks.empty() )
            this->tasks.pop_front();
    }

    this->wait_for_empty();
    delete[] this->threaddata;
    this->workers.clear();
    this->condition.notify_all();
}

ThreadType
ThreadPool::get_thread_type(COUNT_T _thread_index)
{
    return this->threaddata[_thread_index].type;
}

ThreadStatus
ThreadPool::get_thread_status(COUNT_T _thread_index)
{
    return this->threaddata[_thread_index].status;
}

ThreadType
ThreadPool::get_task_type(COUNT_T _task_index)
{
    return this->tasks.at(_task_index).type;
}

COUNT_T
ThreadPool::working_count(ThreadType _type)
{
    COUNT_T count = 0;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        for (COUNT_T thread_index = 0; thread_index < this->thread_count; ++thread_index)
        {
            if ((this->get_thread_type(thread_index) == _type) && (this->get_thread_status(thread_index) == TS_RUN))
                count++;
        }
    }
    return count;
}

COUNT_T
ThreadPool::waiting_count(ThreadType _type)
{
    COUNT_T count = 0;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        for (COUNT_T task_index = 0; task_index < this->tasks.size(); ++task_index)
        {
            if ((this->get_task_type(task_index) == _type))
                count++;
        }
    }
    return count;
}

COUNT_T
ThreadPool::waiting_count()
{
    return this->tasks.size();
}

COUNT_T
ThreadPool::working_count()
{
    return this->thread_run_count;
}

COUNT_T
ThreadPool::get_size()
{
    return this->working_count() + this->waiting_count();
}

void
ThreadPool::wait_for_empty()
{
    for (;;)
    {
        this->condition.notify_one();

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            this->condition.wait(lock, [this]{ return (this->get_size() == 0); });

            if (this->get_size() == 0)
                return;
        }
    }
}
