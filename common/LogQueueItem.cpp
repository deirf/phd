#include <vector>
#include "LArray.h"
#include "Types.h"
#include "LogQueueItem.h"

LogQueueItem::LogQueueItem(uint _fileindex, QueueType _type)
{
  this->fileindex = _fileindex;
  this->type = _type;
}

LogQueueItem::~LogQueueItem()
{
  {
    const COUNT_T last_element_index = arrays.size() - 1;
    for ( COUNT_T i = 0; i < arrays.size(); i++)
    {
      arrays.at( last_element_index - i )->free();
    }
    arrays.clear();
  }

  {
    const COUNT_T last_element_index = prec_records.size() - 1;
    for ( COUNT_T i = 0; i < prec_records.size(); i++)
    {
      prec_records.at( last_element_index - i )->free();
    }
    prec_records.clear();
  }

  {
    const COUNT_T last_element_index = count_records.size() - 1;
    for ( COUNT_T i = 0; i < count_records.size(); i++)
    {
      count_records.at( last_element_index - i )->free();
    }
    count_records.clear();
  }
}
