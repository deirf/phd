#pragma once
#include "Types.h"

typedef enum
{
  RCD_TORECORD,
  RCD_FROMRECORD,
} RecordCopyDirection;

template <typename RecordType>
class Record
{
private:
  std::unique_ptr<TVector(RecordType)> record;
  COUNT_T oneDataSize = 0;
  COUNT_T count = 0;

  void checkDataExistance( COUNT_T _recordindex );
  void checkRecordReal( COUNT_T _recordindex );
  void checkCountLessThanSize();
  void checkCountIsSize();

public:
  Record(COUNT_T _size, RecordType _defaultvalue);
  explicit Record(COUNT_T _size);
  Record();
  ~Record();
  void create(COUNT_T _size);
  void clear();
  void free();


  void setData(COUNT_T _index, RecordType _data);
  COUNT_T addData(RecordType _data);
  bool delData(COUNT_T _index, RecordType _zero);
  RecordType getData(COUNT_T _index);

  void setSize(COUNT_T _size);
  COUNT_T getSize();
  void setCount(COUNT_T _count);
  COUNT_T getCount();
  COUNT_T getLastIndex();
  void incCount();
  void decCount();
  void clearCount();
  uint getType();

  void fill(RecordType _data);
  void append(RecordType _data);
  RecordType* getRecordDataPtr();
  TVector(RecordType)* getRecordPtr();
  TRecord(RecordType)* copy();
  void copyTo(TRecord(RecordType)* _data);
  void copyFrom(TRecord(RecordType)* _data);
  void copyTo(RecordType* _data, COUNT_T _N);
  void copyFrom(RecordType* _data, COUNT_T _N);
  void copy(RecordType* _dataptr, RecordCopyDirection _direction);
  void copyPart(RecordType* _dataptr, COUNT_T _count, RecordCopyDirection _direction);
  bool isDataExist(COUNT_T _index);
  bool isRecordReal(COUNT_T _index);
  RecordType getMax();
};

