#include "LArray.h"

template class LArray<PREC>;


template <typename ArrayType> uint
LArray<ArrayType>::getType()
{
  return typeID.at( std::type_index( typeid( ArrayType ) ) );
}

template <typename ArrayType>
LArray<ArrayType>::LArray(COUNT_T _numberOfArrays, COUNT_T _recordsize, COUNT_T _numberOfRecords, ArrayType _defaultvalue)
{
  this->create(_numberOfRecords, _numberOfArrays, _recordsize, _defaultvalue);
}

template <typename ArrayType>
LArray<ArrayType>::LArray(COUNT_T _numberOfArrays, COUNT_T _recordsize, COUNT_T _numberOfRecords)
{
  this->create(_numberOfRecords, _numberOfArrays, _recordsize, 0);
}

template <typename ArrayType>
LArray<ArrayType>::LArray(COUNT_T _numberOfArrays, COUNT_T _numberOfRecords)
{
  this->create(_numberOfRecords, _numberOfArrays, 0, 0);
}

template <typename ArrayType>
LArray<ArrayType>::LArray(COUNT_T _numberOfArrays)
{
  this->create(0, _numberOfArrays, 0, 0);
}

template <typename ArrayType>
LArray<ArrayType>::LArray()
{
  this->create(0, 0, 0, 0);
}

template <typename ArrayType> void
LArray<ArrayType>::create(COUNT_T _numberOfRecords, COUNT_T _numberOfArrays, COUNT_T _recordsize, ArrayType _defaultvalue)
{
  this->array = std::unique_ptr<TVLArray(ArrayType)>( new TVLArray(ArrayType)(_numberOfRecords) );
  this->setCount(_numberOfArrays);
  this->oneDataSize = sizeof(ArrayType);
  if (_numberOfRecords > 0)
  {
    for (COUNT_T _arrayindex = 0; _arrayindex < _numberOfArrays; _arrayindex++)
    {
      this->array->at(_arrayindex) = this->createArray(_recordsize, _numberOfRecords, _defaultvalue);
    }
  }
}

template <typename ArrayType> void
LArray<ArrayType>::checkArrayExistance( COUNT_T _arrayindex )
{
  assume( this->isArrayExist( _arrayindex ), "Array does not exist; index='%ld'", _arrayindex );
}

template <typename ArrayType> void
LArray<ArrayType>::checkArrayReal( COUNT_T _arrayindex )
{
  assume( this->isArrayReal( _arrayindex ), "Array does not real; index='%ld'", _arrayindex );
}

template <typename ArrayType> void
LArray<ArrayType>::checkCountIsSize()
{
  assume( this->getCount() == this->getSize(), "Count != size; count='%ld', size='%ld'", this->getCount(), this->getSize() );
}

template <typename ArrayType> bool
LArray<ArrayType>::isArrayExist(COUNT_T _arrayindex)
{
  return (_arrayindex < this->getCount());
}

template <typename ArrayType> bool
LArray<ArrayType>::isArrayReal(COUNT_T _arrayindex)
{
  return (_arrayindex < this->getSize());
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::getSize()
{
  return this->array->size();
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::getCount()
{
  return this->count;
}

template <typename ArrayType> void
LArray<ArrayType>::setCount(COUNT_T _count)
{
  this->count = _count;
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::getLastIndex()
{
  assume( this->getCount() > 0, "Count = 0" );
  return this->getCount() - 1;
}

template <typename ArrayType> void
LArray<ArrayType>::incCount()
{
  this->checkCountIsSize();
  this->count++;
}

template <typename ArrayType> void
LArray<ArrayType>::decCount()
{
  assume( this->getCount() > 0, "Count = 0" );
  this->count--;
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::getArrayCount(COUNT_T _arrayindex)
{
  this->checkArrayExistance(_arrayindex);
  return this->getArrayPtr(_arrayindex)->getCount();
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::getArraySize(COUNT_T _arrayindex)
{
  this->checkArrayExistance(_arrayindex);
  return this->getArrayPtr(_arrayindex)->getSize();
}

template <typename ArrayType> TArray(ArrayType)*
LArray<ArrayType>::getArrayPtr(COUNT_T _arrayindex)
{
  this->checkArrayExistance(_arrayindex);
  return this->array->at(_arrayindex);
}

template <typename ArrayType> void
LArray<ArrayType>::setArrayPtr(TArray(ArrayType)* _array, COUNT_T _arrayindex)
{
  this->checkArrayReal(_arrayindex);
  this->array->at(_arrayindex) = _array;
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::appendArray(Array<ArrayType>* _array)
{
  this->checkCountIsSize();
  this->array->push_back( std::move( _array ) );
  this->setCount(this->getSize());
  return this->getLastIndex();
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::addArrayInternal(Array<ArrayType>* _array)
{
  if (this->getCount() < this->getSize())
  {
    this->setArrayPtr(_array, this->getCount());
    this->incCount();
  }
  else //(this->getCount() == this->getSize())
  {
    this->appendArray(_array);
  }

  return this->getLastIndex();
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::addArray(Array<ArrayType>* _array)
{
  return this->addArrayInternal(_array);
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::addArray(COUNT_T _size)
{
  return this->addArrayInternal( this->createArray(_size, 0, 0) );
}

template <typename ArrayType> COUNT_T
LArray<ArrayType>::addArray()
{
  return this->addArray((COUNT_T)0);
}

template <typename ArrayType> bool
LArray<ArrayType>::delArray(COUNT_T _arrayindex)
{
  this->checkArrayExistance(_arrayindex);

  COUNT_T last_index = this->getLastIndex();
  this->getArrayPtr(_arrayindex)->free();

  bool result = FALSE;
  if (last_index > _arrayindex)
  {
    result = TRUE;
    this->setArrayPtr(this->getArrayPtr(last_index), _arrayindex);
  }

  this->setArrayPtr(NULL, last_index);
  this->decCount();

  return result;
}

template <typename ArrayType> Array<ArrayType>*
LArray<ArrayType>::createArray(COUNT_T _arraysize, COUNT_T _numberOfRecords, ArrayType _defaultvalue)
{
  return new Array<ArrayType>(_arraysize, _numberOfRecords, _defaultvalue);
}

template <typename ArrayType> LArray<ArrayType>*
LArray<ArrayType>::copyArray()
{
  LArray<ArrayType>* result = new LArray<ArrayType>();
  for ( COUNT_T arrayindex = 0; arrayindex < (COUNT_T)this->getCount(); ++arrayindex )
  {
    TArray(ArrayType)* array_ = this->getArrayPtr(arrayindex)->copyArray();
    result->addArrayInternal( array_ );
  }
  return result;
}

template <typename ArrayType> void
LArray<ArrayType>::clear()
{
  while ( this->getCount() != 0 )
  {
    this->delArray(this->getCount() - 1);
  }

  this->array->clear();
  this->setCount(0);
}

template <typename ArrayType>
LArray<ArrayType>::~LArray()
{
  this->clear();
  this->array.reset();
}

template <typename ArrayType> void
LArray<ArrayType>::free()
{
  delete this;
}
