#pragma once

#include <vector>
#include <string>
#include "LogQueueItem.h"
#include "LArray.h"
#include "Types.h"

class LogCreator
{
private:
  char* lineBuf = NULL;
  COUNT_T maxBufLen = 0;
  COUNT_T posBuf = 0;
  COUNT_T lineItemCounter = 0;

  template<typename... Args> void addToLinebuf(Args... args);

  template <typename ItemType> void CollectLocation(TVArray(ItemType) _data, COUNT_T _item_index);
  template <typename ItemType> void CollectInts(TVector(ItemType) _data, const std::string info[]);
  template <typename ItemType> void CollectRecord(TVArray(ItemType) _data,
                                                  COUNT_T _item_index,
                                                  const std::string _info[],
                                                  const uint _type[]
                                                  );
  template <typename ItemType> void CollectCustomPropertyRecord(TVArray(ItemType) _data,
                                                                COUNT_T _item_index,
                                                                const std::string _info[],
                                                                const uint _type[],
                                                                uint _sparktype
                                                                );
  template <typename ItemType> void CollectCustomLocation(TVLArray(ItemType) _data,
                                                          COUNT_T _item_index,
                                                          const std::string _info[],
                                                          uint _sparktype
                                                          );
  template <typename ItemType> void CollectData(ItemType _data,
                                                const std::string& _info,
                                                const uint _type
                                                );
  template <typename ItemType> void CheckIndexData(ItemType _data,
                                                   uint _max,
                                                   std::string _errormsg
                                                  );

  COUNT_T IncAndGetLineItemCounter() { return ++this->lineItemCounter; }
  COUNT_T DecAndGetLineItemCounter() { return --this->lineItemCounter; }
  void ZeroLineItemCounter() { this->lineItemCounter = 0; }
  COUNT_T GetLineItemCounter() { return this->lineItemCounter; }

  void ZeroPosBuf() { this->posBuf = 0; }
  COUNT_T GetPosBuf() { return this->posBuf; }
public:
  LogCreator();
  ~LogCreator();

  COUNT_T PrintnFWA(LogQueueItem* item);
  COUNT_T PrintSwarm(LogQueueItem* item);
  COUNT_T PrintoFWA(LogQueueItem* item);

  char* GetLineBuf() { return this->lineBuf; }
};

