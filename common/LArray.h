#pragma once
#include "Array.h"
#include "Types.h"



template <typename ArrayType>
class LArray
{
private:
  std::unique_ptr<TVLArray(ArrayType)> array;
  COUNT_T oneDataSize = 0;
  COUNT_T count = 0;
  void create(COUNT_T _numberOfRecords, COUNT_T _numberOfArrays, COUNT_T _recordsize, ArrayType _defaultvalue);
  COUNT_T addArrayInternal(Array<ArrayType>* _array);

  void checkCountIsSize();
  void checkArrayReal( COUNT_T _arrayindex );
  void checkArrayExistance( COUNT_T _arrayindex );

public:
  LArray(COUNT_T _numberOfArrays, COUNT_T _recordsize, COUNT_T _numberOfRecords, ArrayType _defaultvalue);
  LArray(COUNT_T _numberOfArrays, COUNT_T _recordsize, COUNT_T _numberOfRecords);
  LArray(COUNT_T _numberOfArrays, COUNT_T _numberOfRecords);
  explicit LArray(COUNT_T _numberOfArrays);
  LArray();
  ~LArray();
  void clear();
  void free();

  bool isArrayExist(COUNT_T _arrayindex);
  bool isArrayReal(COUNT_T _arrayindex);

  Array<ArrayType>* createArray(COUNT_T _recordsize, COUNT_T _numberOfRecords, ArrayType _defaultvalue);

  COUNT_T getCount();
  void setCount(COUNT_T _count);
  void incCount();
  void decCount();
  COUNT_T getLastIndex();
  uint getType();

  COUNT_T getArrayCount(COUNT_T _arrayindex);
  void setArrayCount(COUNT_T _arrayindex, COUNT_T _index);

  COUNT_T getSize();
  COUNT_T getArraySize(COUNT_T _arrayindex);

  TArray(ArrayType)* getArrayPtr(COUNT_T _arrayindex);
  void setArrayPtr(TArray(ArrayType)* _Array, COUNT_T _arrayindex);

  COUNT_T appendArray(Array<ArrayType>* _Array);
  COUNT_T addArray(Array<ArrayType>* _Array);
  COUNT_T addArray(COUNT_T _size);
  COUNT_T addArray();

  bool delArray(COUNT_T _arrayindex);

  LArray<ArrayType>* copyArray();
};


