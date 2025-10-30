#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include "LogThread.h"
#include "LArray.h"
#include "Types.h"
#include "FileIOHelper.h"


LogThread::LogThread(LogQueue<LogQueueItem*>& _queue) : queue(_queue)
{
  this->consumer_thread = std::thread(&LogThread::Consumer, this);
  this->filestream = new TFilestreamList();
  this->logcreator = new LogCreator();
}

void
LogThread::Consumer()
{
  LogQueueItem* item;

  while ( !( this->stop && this->queue.empty() && !this->wait ) )
  {
    {
      std::unique_lock<std::mutex> lock(log_thread_mutex);
      consumer_condition.wait(lock, [this]() { return ( this->stop || !this->queue.empty() ); } );
      if (this->queue.remove(item))
      {
        this->PrintItem(item);
        if (this->queue.empty())
        {
          this->filestream->at( item->GetFileIndex() )->flush();
        }
        delete item;
      }
    }
  }
}

LogThread::~LogThread()
{
  this->consumer_condition.notify_all();
  this->stop = TRUE;
  this->consumer_thread.join();
  assume(this->filestream->size() < UINT_MAX, "Too many files are open");
  const uint file_count = (uint)this->filestream->size();

  std::unique_lock<std::mutex> lock(log_thread_mutex);
  for (uint i = 0; i < file_count; ++i)
  {
    this->CloseFile(i);
  }
  this->filestream->clear();
  delete this->filestream;
  delete this->logcreator;
}

uint
LogThread::OpenFile(const std::string& _filename)
{
  std::unique_lock<std::mutex> lock( this->log_thread_filestream_mutex );

  const std::string path = FileIOHelper::dirname( _filename );
  assume( ( FileIOHelper::makedir( path )), "Cannot create directory; path='%s'", path.c_str() );

  std::ofstream* logfile = new std::ofstream;
  logfile->open(_filename + ".dat", std::ofstream::out | std::ofstream::app | std::ofstream::binary);
  this->filestream->push_back(std::move(logfile));
  assume(this->filestream->size() < UINT_MAX, "Too many files are open");
  return (uint)this->filestream->size() - 1; //get the inserted index
}

void
LogThread::WriteToFile(char* _str, COUNT_T _len, uint _index)
{
  this->filestream->at(_index)->write(_str, (long int)_len);
  this->IncLogItemCounter();
}

void
LogThread::Flush()
{
  this->consumer_condition.notify_one();
}

void
LogThread::AddToQueue(LogQueueItem* item)
{
  std::unique_lock<std::mutex> lock( this->log_thread_mutex );

  this->queue.add(item);
}

void
LogThread::CloseFile(uint index)
{
  this->filestream->at(index)->close();
  delete this->filestream->at(index);
}

void
LogThread::PrintItem(LogQueueItem* _item)
{
  COUNT_T len = 0;
  switch (_item->GetQueueType())
  {
    case QT_nFWA:
      {
        len = this->logcreator->PrintnFWA(_item);
        break;
      }
    case QT_PSO:
      {
        len = this->logcreator->PrintSwarm(_item);
        break;
      }
    case QT_oFWA:
      {
        len = this->logcreator->PrintoFWA(_item);
        break;
      }
    case QT_HEAT:
      {
        break;
      }
    default:
      {
        DOES_NOT_REACH();
      }
  }
  WriteToFile(this->logcreator->GetLineBuf(), len, _item->GetFileIndex());
}
