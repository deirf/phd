#pragma once

#include "Record.h"
#include "Types.h"

typedef enum
{
  ACD_TOARRAY,
  ACD_FROMARRAY,
} ArrayCopyDirection;

typedef enum
{
  ACNA_CREATE,
  ACNA_NOCREATE,
} ArrayCreateNewArray;


template <typename ArrayType>
class Array
{
private:
  std::unique_ptr<TVArray(ArrayType)> array;
  COUNT_T oneDataSize = 0;
  COUNT_T count = 0;
  void create(COUNT_T _numberOfRecords, COUNT_T _recordsize, ArrayType _defaultvalue);
  COUNT_T addRecordInternal(Record<ArrayType>* _record);

  void checkCountIsSize();
  void checkRecordReal( COUNT_T _recordindex );
  void checkRecordExistance( COUNT_T _recordindex );
  void checkRecordValidity( COUNT_T _recordindex );

public:
  Array(COUNT_T _recordsize, COUNT_T _numberOfRecords, ArrayType _defaultvalue);
  Array(COUNT_T _recordsize, COUNT_T _numberOfRecords);
  explicit Array(COUNT_T _numberOfRecords);
  Array();
  ~Array();
  void clear();
  void free();

  bool isRecordExist(COUNT_T _recordindex);
  bool isRecordReal(COUNT_T _recordindex);
  bool isRecordValid(COUNT_T _recordindex);

  COUNT_T getCount();
  void setCount(COUNT_T _count);
  void incCount();
  void decCount();
  COUNT_T getLastIndex();
  uint getType();

  COUNT_T getRecordCount(COUNT_T _recordindex);
  void setRecordCount(COUNT_T _recordindex, COUNT_T _index);

  COUNT_T getSize();
  COUNT_T getRecordSize(COUNT_T _recordindex);

  TRecord(ArrayType)* getRecordPtr(COUNT_T _recordindex);
  void setRecordPtr(TRecord(ArrayType)* _record, COUNT_T _recordindex);
  ArrayType* getRecordDataPtr(COUNT_T _recordindex);

  void setData(COUNT_T _recordindex, COUNT_T _index, ArrayType _newdata);
  ArrayType getData(COUNT_T _recordindex, COUNT_T _index);
  COUNT_T addData(COUNT_T _recordindex, COUNT_T _index, ArrayType _data);
  bool delData(COUNT_T _recordindex, COUNT_T _index, ArrayType _zero);

  COUNT_T appendRecord(Record<ArrayType>* _record);
  COUNT_T addRecord(Record<ArrayType>* _record);
  COUNT_T addRecord(COUNT_T _size);
  COUNT_T addRecord();

  bool delRecord(COUNT_T _recordindex);

  Record<ArrayType>* createRecord(COUNT_T _size, ArrayType _defaultvalue);

  void copyData(ArrayType* _dataptr, COUNT_T _recordindex, ArrayCopyDirection _direction);
  COUNT_T copyAllData(ArrayType* _dataptr, ArrayCopyDirection _direction);
  Array<ArrayType>* copyArray();
};

